#/** @file
# Copyright (c) 2016-2018,2023-2025, Arm Limited or its affiliates. All rights reserved.
# SPDX-License-Identifier : Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#**/


#ifdef TARGET_EMULATION
/* Private worker functions for ASM_PFX() */
#define _CONCATENATE(a, b)  __CONCATENATE(a, b)
#define __CONCATENATE(a, b) a ## b

/* The __USER_LABEL_PREFIX__ macro predefined by GNUC represents
   the prefix on symbols in assembly language.*/
#define __USER_LABEL_PREFIX__

#define ASM_PFX(name) _CONCATENATE (__USER_LABEL_PREFIX__, name)

#define GCC_ASM_EXPORT(func__)  \
       .global  _CONCATENATE (__USER_LABEL_PREFIX__, func__)    ;\
       .type ASM_PFX(func__), %function

#define GCC_ASM_IMPORT(func__)  \
       .extern  _CONCATENATE (__USER_LABEL_PREFIX__, func__)
#endif

.text
.align 2

GCC_ASM_EXPORT(ArmReadHcrEl2)
GCC_ASM_EXPORT(ArmReadAA64MMFR1EL1)
GCC_ASM_EXPORT(ArmReadAA64MMFR4EL1)
GCC_ASM_EXPORT(ArmReadCntFrq)
GCC_ASM_EXPORT(ArmReadCntPct)
GCC_ASM_EXPORT(ArmReadCntkCtl)
GCC_ASM_EXPORT(ArmWriteCntkCtl)
GCC_ASM_EXPORT(ArmReadCntkCtl12)
GCC_ASM_EXPORT(ArmWriteCntkCtl12)
GCC_ASM_EXPORT(ArmReadCntpTval)
GCC_ASM_EXPORT(ArmWriteCntpTval)
GCC_ASM_EXPORT(ArmReadCntpTval02)
GCC_ASM_EXPORT(ArmWriteCntpTval02)
GCC_ASM_EXPORT(ArmReadCntpCtl)
GCC_ASM_EXPORT(ArmReadCntpCtl02)
GCC_ASM_EXPORT(ArmReadCntvTval)
GCC_ASM_EXPORT(ArmWriteCntvTval)
GCC_ASM_EXPORT(ArmReadCntvTval02)
GCC_ASM_EXPORT(ArmWriteCntvTval02)
GCC_ASM_EXPORT(ArmReadCntvCtl)
GCC_ASM_EXPORT(ArmWriteCntvCtl)
GCC_ASM_EXPORT(ArmReadCntvCtl02)
GCC_ASM_EXPORT(ArmWriteCntvCtl02)
GCC_ASM_EXPORT(ArmReadCntvCt)
GCC_ASM_EXPORT(ArmReadCntpCval)
GCC_ASM_EXPORT(ArmWriteCntpCval)
GCC_ASM_EXPORT(ArmReadCntpCval02)
GCC_ASM_EXPORT(ArmWriteCntpCval02)
GCC_ASM_EXPORT(ArmReadCntvCval)
GCC_ASM_EXPORT(ArmWriteCntvCval)
GCC_ASM_EXPORT(ArmReadCntvCval02)
GCC_ASM_EXPORT(ArmWriteCntvCval02)
GCC_ASM_EXPORT(ArmReadCntvOff)
GCC_ASM_EXPORT(ArmWriteCntvOff)
GCC_ASM_EXPORT(ArmReadCnthpCtl)
GCC_ASM_EXPORT(ArmWriteCnthpCtl)
GCC_ASM_EXPORT(ArmReadCnthpTval)
GCC_ASM_EXPORT(ArmWriteCnthpTval)
GCC_ASM_EXPORT(ArmReadCnthvCtl)
GCC_ASM_EXPORT(ArmWriteCnthvCtl)
GCC_ASM_EXPORT(ArmReadCnthvTval)
GCC_ASM_EXPORT(ArmWriteCnthvTval)
GCC_ASM_EXPORT(ArmWriteCntpCtl)
GCC_ASM_EXPORT(ArmWriteCntpCtl02)

ASM_PFX(ArmReadHcrEl2):
  mrs   x0, hcr_el2           // Read HCR_EL2
  ret

ASM_PFX(ArmReadAA64MMFR1EL1):
  mrs   x0, id_aa64mmfr1_el1  // Read ID_AA64MMFR1_EL1
  ret

ASM_PFX(ArmReadAA64MMFR4EL1):
  //mrs   x0, id_aa64mmfr4_el1  // Read ID_AA64MMFR4_EL1
  mrs	x0, s3_0_c0_c7_4
  ret

ASM_PFX(ArmReadCntFrq):
  mrs   x0, cntfrq_el0           // Read CNTFRQ
  ret


ASM_PFX(ArmReadCntPct):
  mrs   x0, cntpct_el0           // Read CNTPCT (Physical counter register)
  ret


ASM_PFX(ArmReadCntkCtl):
  mrs   x0, cntkctl_el1          // Read CNTK_CTL (Timer PL1 Control Register)
  ret

ASM_PFX(ArmReadCntkCtl12):
  mrs   x0, cntkctl_el12         // Read CNTK_CTL (Timer PL1 Control Register) when EL2 Host
  ret

ASM_PFX(ArmWriteCntkCtl):
  msr   cntkctl_el1, x0          // Write to CNTK_CTL (Timer PL1 Control Register)
  isb
  ret

ASM_PFX(ArmWriteCntkCtl12):
  msr   cntkctl_el12, x0      // Write to CNTK_CTL (Timer PL1 Control Register) when EL2 Host
  isb
  ret

ASM_PFX(ArmReadCntpTval):
  mrs   x0, cntp_tval_el0     // Read CNTP_TVAL (PL1 physical timer value register)
  ret

ASM_PFX(ArmReadCntpTval02):
  mrs   x0, cntp_tval_el02    // Read CNTP_TVAL (PL1 physical timer value register) when EL2 Host
  ret

ASM_PFX(ArmWriteCntpTval):
  msr   cntp_tval_el0, x0        // Write to CNTP_TVAL (PL1 physical timer value register)
  isb
  ret

ASM_PFX(ArmWriteCntpTval02):
  msr   cntp_tval_el02, x0   // Write to CNTP_TVAL (PL1 physical timer value register) EL2 Host
  isb
  ret

ASM_PFX(ArmReadCntpCtl):
  mrs   x0, cntp_ctl_el0     // Read CNTP_CTL (PL1 Physical Timer Control Register)
  ret

ASM_PFX(ArmReadCntpCtl02):
  mrs   x0, cntp_ctl_el02    // Read CNTP_CTL (PL1 Physical Timer Control Register) when EL2 Host
  ret


ASM_PFX(ArmWriteCntpCtl):
  msr   cntp_ctl_el0, x0     // Write CNTP_CTL (PL1 Physical Timer Control Register)
  isb
  ret

ASM_PFX(ArmWriteCntpCtl02):
  msr   cntp_ctl_el02, x0    // Write CNTP_CTL (PL1 Physical Timer Control Register) when EL2 Host
  isb
  ret

ASM_PFX(ArmReadCntvTval):
  mrs   x0, cntv_tval_el0        // Read CNTV_TVAL (Virtual Timer Value register)
  ret

ASM_PFX(ArmReadCntvTval02):
  mrs   x0, cntv_tval_el02        // Read CNTV_TVAL_EL02 (Virtual Timer Value register)
  ret

ASM_PFX(ArmWriteCntvTval):
  msr   cntv_tval_el0, x0        // Write to CNTV_TVAL (Virtual Timer Value register)
  isb
  ret

ASM_PFX(ArmWriteCntvTval02):
  msr   cntv_tval_el02, x0        // Write to CNTV_TVAL_EL02 (Virtual Timer Value register)
  isb
  ret

ASM_PFX(ArmReadCntvCtl):
  mrs   x0, cntv_ctl_el0         // Read CNTV_CTL (Virtual Timer Control Register)
  ret

ASM_PFX(ArmReadCntvCtl02):
  mrs   x0, cntv_ctl_el02         // Read CNTV_CTL_EL02 (Virtual Timer Control Register)
  ret

ASM_PFX(ArmWriteCntvCtl):
  msr   cntv_ctl_el0, x0         // Write to CNTV_CTL (Virtual Timer Control Register)
  isb
  ret

ASM_PFX(ArmWriteCntvCtl02):
  msr   cntv_ctl_el02, x0         // Write to CNTV_CTL_EL02 (Virtual Timer Control Register)
  isb
  ret

ASM_PFX(ArmReadCntvCt):
  mrs  x0, cntvct_el0            // Read CNTVCT  (Virtual Count Register)
  ret


ASM_PFX(ArmReadCntpCval):
  mrs   x0, cntp_cval_el0        // Read CNTP_CTVAL (Physical Timer Compare Value Register)
  ret

ASM_PFX(ArmReadCntpCval02):
  mrs   x0, cntp_cval_el02       // Read CNTP_CTVAL_EL02 (Physical Timer Compare Value Register)
  ret

ASM_PFX(ArmWriteCntpCval):
  msr   cntp_cval_el0, x0        // Write to CNTP_CTVAL (Physical Timer Compare Value Register)
  isb
  ret

ASM_PFX(ArmWriteCntpCval02):
  msr   cntp_cval_el02, x0       // Write to CNTP_CTVAL_EL02 (Physical Timer Compare Value Register)
  isb
  ret

ASM_PFX(ArmReadCntvCval):
  mrs   x0, cntv_cval_el0        // Read CNTV_CTVAL (Virtual Timer Compare Value Register)
  ret

ASM_PFX(ArmReadCntvCval02):
  mrs   x0, cntv_cval_el02       // Read CNTV_CTVAL_EL02 (Virtual Timer Compare Value Register)
  ret

ASM_PFX(ArmWriteCntvCval):
  msr   cntv_cval_el0, x0        // write to  CNTV_CTVAL (Virtual Timer Compare Value Register)
  isb
  ret

ASM_PFX(ArmWriteCntvCval02):
  msr   cntv_cval_el02, x0       // write to  CNTV_CTVAL_EL02 (Virtual Timer Compare Value Register)
  isb
  ret

ASM_PFX(ArmReadCntvOff):
  mrs   x0, cntvoff_el2          // Read CNTVOFF (virtual Offset register)
  ret


ASM_PFX(ArmWriteCntvOff):
  msr   cntvoff_el2, x0          // Write to CNTVOFF (Virtual Offset register)
  isb
  ret

ASM_PFX(ArmReadCnthpCtl):
  mrs   x0, cnthp_ctl_el2
  ret


ASM_PFX(ArmWriteCnthpCtl):
  msr   cnthp_ctl_el2, x0
  isb
  ret

ASM_PFX(ArmReadCnthpTval):
  mrs   x0, cnthp_tval_el2
  ret


ASM_PFX(ArmWriteCnthpTval):
  msr   cnthp_tval_el2, x0
  isb
  ret

ASM_PFX(ArmReadCnthvCtl):
  mrs   x0, cnthv_ctl_el2
  ret


ASM_PFX(ArmWriteCnthvCtl):
  msr   cnthv_ctl_el2, x0
  isb
  ret

ASM_PFX(ArmReadCnthvTval):
  mrs   x0, cnthv_tval_el2
  ret


ASM_PFX(ArmWriteCnthvTval):
  msr   cnthv_tval_el2, x0
  isb
  ret

#ifndef TARGET_EMULATION
ASM_FUNCTION_REMOVE_IF_UNREFERENCED
#endif
