/** @file
 * Copyright (c) 2018-2021,2023-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/


/* Test sequence - Initialize a main memory region marked as WB,
 * outer shareable by the PE page tables. CPU Write to this region with
 * new data. Perform actions to maintain  software coherency.
 * Read the same data locations from the Exerciser with NS=1. The
 * exerciser must get the latest data. The exerciser updates the
 * location with newest data. PE reads the location and must get NEWEST VAL.
 */

#include "val/include/acs_val.h"
#include "val/include/acs_memory.h"
#include "val/include/acs_exerciser.h"
#include "val/include/acs_smmu.h"
#include "val/include/acs_pcie.h"
#include "val/include/acs_pcie_enumeration.h"

#define TEST_NUM   (ACS_EXERCISER_TEST_NUM_BASE + 8)
#define TEST_RULE  "PCI_IC_12, PCI_IC_14, RE_ORD_4, IE_ORD_4"
#define TEST_DESC  "Check PCIe Software Coherency         "

#define TEST_DATA_BLK_SIZE  (4*1024)
#define NEW_DATA 0xAD
#define NEWEST_DATA 0xBC

static
uint32_t
test_sequence2(void *dram_buf1_virt, void *dram_buf1_phys, uint32_t e_bdf, uint32_t instance)
{

  uint32_t dma_len;
  void *dram_buf2_virt;
  void *dram_buf2_phys;
  uint32_t tp_bit;

  /* Set up a second dram buffer to send NEWEST_DATA to exerciser memory */
  dram_buf2_virt = dram_buf1_virt + (TEST_DATA_BLK_SIZE / 2);
  dram_buf2_phys = dram_buf1_phys + (TEST_DATA_BLK_SIZE / 2);
  dma_len = TEST_DATA_BLK_SIZE / 2;

  /* Write dram_buf2 with known data and flush the buffer to main memory */
  val_memory_set(dram_buf2_virt, dma_len, NEWEST_DATA);
  val_pe_cache_clean_invalidate_range((uint64_t)dram_buf2_virt, (uint64_t)dma_len);

  /* Perform DMA OUT to copy contents of dram_buf2 to exerciser memory */
  val_exerciser_set_param(DMA_ATTRIBUTES, (uint64_t)dram_buf2_phys, dma_len, instance);
  val_exerciser_ops(START_DMA, EDMA_TO_DEVICE, instance);

  /* Perform DMA IN to copy content back from exerciser memory to dram_buf1 */
  val_exerciser_set_param(DMA_ATTRIBUTES, (uint64_t)dram_buf1_phys, dma_len, instance);
  val_exerciser_ops(START_DMA, EDMA_FROM_DEVICE, instance);

  /* Check if the transaction pending bit is cleared */
  tp_bit = val_is_transaction_pending_set(e_bdf);
  if (tp_bit) {
      val_print(ACS_PRINT_ERR, "\n       Transaction still pending in function %4x", instance);
      return 1;
  }

  /* Invalidate dram_buf1 and dram_buf2 contents present in CPU caches */
  val_pe_cache_invalidate_range((uint64_t)dram_buf1_virt, (uint64_t)dma_len);
  val_pe_cache_invalidate_range((uint64_t)dram_buf2_virt, (uint64_t)dma_len);

  /* Compare the contents of ddr_buf1 and ddr_buf2 for NEW_DATA */
  if (val_memory_compare(dram_buf1_virt, dram_buf2_virt, dma_len)) {
      val_print(ACS_PRINT_ERR, "\n       I/O coherency failure for Exerciser %4x", instance);
      return 1;
  }

  /* Return success */
  return 0;
}

static
uint32_t
test_sequence1(void *dram_buf1_virt, void *dram_buf1_phys, uint32_t e_bdf, uint32_t instance)
{

  uint32_t dma_len;
  void *dram_buf2_virt;
  void *dram_buf2_phys;
  uint32_t tp_bit;

  dram_buf2_virt = dram_buf1_virt + (TEST_DATA_BLK_SIZE / 2);
  dram_buf2_phys = dram_buf1_phys + (TEST_DATA_BLK_SIZE / 2);
  dma_len = TEST_DATA_BLK_SIZE / 2;

  /* Write dram_buf1 cache with new data */
  val_memory_set(dram_buf1_virt, dma_len, NEW_DATA);

  /* Maintain software coherency */
  val_pe_cache_clean_invalidate_range((uint64_t)dram_buf1_virt, (uint64_t)dma_len);

  /* Perform DMA OUT to copy contents of dram_buf1 to exerciser memory */
  val_exerciser_set_param(DMA_ATTRIBUTES, (uint64_t)dram_buf1_phys, dma_len, instance);
  val_exerciser_ops(START_DMA, EDMA_TO_DEVICE, instance);

  /* Perform DMA IN to copy the content from exerciser memory to dram_buf2 */
  val_exerciser_set_param(DMA_ATTRIBUTES, (uint64_t)dram_buf2_phys, dma_len, instance);
  val_exerciser_ops(START_DMA, EDMA_FROM_DEVICE, instance);

  /* Check if the transaction pending bit is cleared */
  tp_bit = val_is_transaction_pending_set(e_bdf);
  if (tp_bit) {
      val_print(ACS_PRINT_ERR, "\n       Transaction still pending in function %4x", instance);
      return 1;
  }

  /* Invalidate dram_buf2 contents present in CPU caches */
  val_pe_cache_invalidate_range((uint64_t)dram_buf2_virt, (uint64_t)dma_len);

  /* Compare the contents of ddr_buf1 and ddr_buf2 for NEW_DATA */
  if (val_memory_compare(dram_buf1_virt, dram_buf2_virt, dma_len)) {
      val_print(ACS_PRINT_ERR, "\n       I/O coherency failure for Exerciser %4x", instance);
      return 1;
  }

  /* Return success */
  return 0;
}


static
void
payload (void)
{

  uint32_t pe_index;
  uint32_t instance;
  uint32_t e_bdf;
  uint32_t smmu_index;
  void *dram_buf1_virt;
  void *dram_buf1_phys;

  dram_buf1_virt = NULL;
  dram_buf1_phys = NULL;

  pe_index = val_pe_get_index_mpid (val_pe_get_mpid());

  /* Read the number of excerciser cards */
  instance = val_exerciser_get_info(EXERCISER_NUM_CARDS);

  while (instance-- != 0) {

    /* if init fail moves to next exerciser */
    if (val_exerciser_init(instance))
        continue;

    /* Get the exerciser BDF */
    e_bdf = val_exerciser_get_bdf(instance);
    val_print(ACS_PRINT_DEBUG, "\n       Exerciser BDF - 0x%x", e_bdf);

    /* Find SMMU node index for this exerciser instance */
    smmu_index = val_iovirt_get_rc_smmu_index(PCIE_EXTRACT_BDF_SEG(e_bdf),
                                              PCIE_CREATE_BDF_PACKED(e_bdf));

    /* Disable SMMU globally so that the transaction passes
     * through the SMMU without any address modification.
     * Global attributes, such as memory type or Shareability,
     * might be applied from the SMMU_GBPA register of the SMMU.
     * Or, the SMMU_GBPA register might be configured to abort
     * all transactions. Do this only if the RC is behind an SMMU.
     */
    if (smmu_index != ACS_INVALID_INDEX) {
        if (val_smmu_disable(smmu_index)) {
            val_print(ACS_PRINT_ERR, "\n       Exerciser %x smmu disable error", instance);
            val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 2));
            return;
        }
    }

    /* Get a WB, outer shareable DDR Buffer of size TEST_DATA_BLK_SIZE */
    dram_buf1_virt = val_memory_alloc_cacheable(e_bdf, TEST_DATA_BLK_SIZE, &dram_buf1_phys);

    if (!dram_buf1_virt) {

      val_print(ACS_PRINT_ERR, "\n       WB and OSH mem alloc failure %x", 2);
      val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 2));
      return;
    }

    /* Program exerciser hierarchy to start sending/receiving TLPs
     * with No Snoop attribute header. This includes enabling
     * No snoop bit in exerciser control register.
     */
    if (val_exerciser_ops(TXN_NO_SNOOP_ENABLE, 0, instance)) {
       val_print(ACS_PRINT_ERR, "\n       Exerciser %x No Snoop enable error", instance);
       goto test_fail;
    }

    if (test_sequence1(dram_buf1_virt, dram_buf1_phys, e_bdf, instance) ||
            test_sequence2(dram_buf1_virt, dram_buf1_phys, e_bdf, instance))
        goto test_fail;

    /* Return this exerciser dma memory back to the heap manager */
    val_memory_free_cacheable(e_bdf, TEST_DATA_BLK_SIZE, dram_buf1_virt, dram_buf1_phys);

  }

  val_set_status(pe_index, RESULT_PASS(TEST_NUM, 0));
  return;

test_fail:
  val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 2));
  val_memory_free_cacheable(e_bdf, TEST_DATA_BLK_SIZE, dram_buf1_virt, dram_buf1_phys);
  return;
}

uint32_t
e008_entry (void)
{
  uint32_t num_pe = 1;
  uint32_t status = ACS_STATUS_FAIL;

  status = val_initialize_test (TEST_NUM, TEST_DESC, num_pe);

  if (status != ACS_STATUS_SKIP) {
      val_run_test_payload (TEST_NUM, num_pe, payload, 0);
  }

  /* Get the result from all PE and check for failure */
  status = val_check_for_error(TEST_NUM, num_pe, TEST_RULE);

  val_report_status(0, ACS_END(TEST_NUM), NULL);

  return status;
}
