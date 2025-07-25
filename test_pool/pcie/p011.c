/** @file
 * Copyright (c) 2020-2021, 2023-2025, Arm Limited or its affiliates. All rights reserved.
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

#include "val/include/acs_val.h"
#include "val/include/acs_pcie.h"
#include "val/include/acs_pe.h"

#define TEST_NUM   (ACS_PCIE_TEST_NUM_BASE + 11)
#define TEST_RULE  "PCI_IN_18"
#define TEST_DESC  "Check RP Byte Enable Rules            "

static
void
payload(void)
{

  int8_t   i;
  uint32_t bdf;
  uint32_t dp_type;
  uint32_t pe_index;
  uint32_t tbl_index;
  uint32_t test_skip = 1;
  uint32_t test_fail = 0;
  uint32_t ecam_cr, ecam_cr_8, ecam_cr_16, ecam_cr_new, ecam_cr_save;
  uint32_t write_value = 0;
  uint32_t busnum_offset;
  addr_t ecam_base;
  pcie_device_bdf_table *bdf_tbl_ptr;

  tbl_index = 0;
  ecam_cr = 0;
  ecam_cr_8 = 0;
  ecam_cr_16 = 0;
  ecam_cr_new = 0;

  bdf_tbl_ptr = val_pcie_bdf_table_ptr();
  pe_index = val_pe_get_index_mpid(val_pe_get_mpid());

  while (tbl_index < bdf_tbl_ptr->num_entries)
  {
      bdf = bdf_tbl_ptr->device[tbl_index++].bdf;
      dp_type = val_pcie_device_port_type(bdf);

      if (dp_type == RP) {

        /* If test runs for atleast an endpoint */
        val_print(ACS_PRINT_DEBUG, "\n       BDF - 0x%x", bdf);
        test_skip = 0;

        /* Get the offset for bus number register in the config header which has
         * bit[23:0] as read/write. Only these 3 bytes are used for this test.
         */
        ecam_base = val_pcie_get_ecam_base(bdf);
        busnum_offset = PCIE_EXTRACT_BDF_BUS(bdf) * PCIE_MAX_DEV * PCIE_MAX_FUNC * PCIE_CFG_SIZE +
                             PCIE_EXTRACT_BDF_DEV(bdf) * PCIE_MAX_FUNC * PCIE_CFG_SIZE +
                             PCIE_EXTRACT_BDF_FUNC(bdf) * PCIE_CFG_SIZE +
                             TYPE1_PBN;

        /* Read Bus number register of RP with 8 Bit, 16 Bit, 32 Bit and compare it */
        ecam_cr = val_mmio_read(ecam_base + busnum_offset);
        for (i = 3; i >= 0; i--) {
          ecam_cr_8 = ecam_cr_8 << 8;
          ecam_cr_8 |= val_mmio_read8(ecam_base + busnum_offset + i);
        }

        for (i = 1; i >= 0; i--) {
          ecam_cr_16 = ecam_cr_16 << 16;
          ecam_cr_16 |= val_mmio_read16(ecam_base + busnum_offset + (i * 2));
        }

        if ((ecam_cr != ecam_cr_8) || (ecam_cr_8 != ecam_cr_16))
        {
          val_print(ACS_PRINT_ERR, "\n        Byte Enable Read Failed for Bdf: 0x%x", bdf);
          test_fail++;
        }

        /* Save the register to restore later */
        ecam_cr_save = val_mmio_read(ecam_base + busnum_offset);

        /* Check Read-Write-Read Behaviour for 8 Bit */
        for (i = 2; i >= 0; i--) {
            ecam_cr = val_mmio_read8(ecam_base + busnum_offset + i);
            write_value = (uint8_t)~ecam_cr;
            val_mmio_write8(ecam_base + busnum_offset + i, write_value);

            ecam_cr_new = val_mmio_read8(ecam_base + busnum_offset + i);
            if (write_value != ecam_cr_new)
            {
              val_print(ACS_PRINT_ERR, "\n        8 Bit Write Failed for Bdf: 0x%x", bdf);
              test_fail++;
            }
        }

        /* Restore the value */
        val_mmio_write(ecam_base + busnum_offset, ecam_cr_save);

        /* Check Read-Write-Read Behaviour for 16 Bit */
        ecam_cr = val_mmio_read16(ecam_base + busnum_offset);
        write_value = (uint16_t)~ecam_cr;
        val_mmio_write16(ecam_base + busnum_offset, write_value);

        ecam_cr_new = val_mmio_read16(ecam_base + busnum_offset);
        if (write_value != ecam_cr_new)
        {
          val_print(ACS_PRINT_ERR, "\n        16 Bit Write Failed for Bdf: 0x%x", bdf);
          test_fail++;
        }

        /* Restore the value */
        val_mmio_write(ecam_base + busnum_offset, ecam_cr_save);

        /* 32 Bit write have been performed during enumeration and
         * during pcie_create_device_bdf_table(). So the check has
         * not been performed
         */
      }
  }

  if (test_skip) {
      val_print(ACS_PRINT_DEBUG, "\n       No RP type device found. Skipping test", 0);
      val_set_status(pe_index, RESULT_SKIP(TEST_NUM, 1));
  }
  if (test_fail)
      val_set_status(pe_index, RESULT_FAIL(TEST_NUM, 1));
  else
      val_set_status(pe_index, RESULT_PASS(TEST_NUM, 1));
}

uint32_t
p011_entry(uint32_t num_pe)
{

  uint32_t status = ACS_STATUS_FAIL;

  num_pe = 1;  //This test is run on single processor

  status = val_initialize_test(TEST_NUM, TEST_DESC, num_pe);
  if (status != ACS_STATUS_SKIP)
      val_run_test_payload(TEST_NUM, num_pe, payload, 0);

  /* get the result from all PE and check for failure */
  status = val_check_for_error(TEST_NUM, num_pe, TEST_RULE);

  val_report_status(0, ACS_END(TEST_NUM), NULL);

  return status;
}
