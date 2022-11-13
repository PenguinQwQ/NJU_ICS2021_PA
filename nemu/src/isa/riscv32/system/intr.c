/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

extern enum {
  /*-- Machine Information Registers --*/
  MVERDORID = 0xF11,
  MARCHID = 0xF12,
  MIMPID = 0xF13,
  MHARTID = 0xF14,

  /*-- Machine Trap Setup --*/

  MSTATUS = 0x300,
  MISA = 0x301,
  MEDELEG = 0x302,
  MIDELEG = 0x303,
  MIE = 0x304,
  MTVEC = 0x305,
  MCOUNTEREN = 0x306,
  MSTATUSH = 0x310,

  /*-- Machine Trap Handling --*/

  MSCRATCH = 0x340,
  MEPC = 0x341,
  MCLAUSE = 0x342,
  MTVAL = 0x343,
  MIP = 0x344,
  MTINST = 0x34A,
  MTVAL2 = 0x34B,

  /*-- Machine Memory Protection --*/
  /*-- Not all used, necessarily add more --*/

  PMEMCFG0 = 0x3A0,
  PMEMCFG1 = 0x3A1,
  PMEMCFG2 = 0x3A2,
  PMEMCFG3 = 0x3A3,
  PMEMCFG4 = 0x3A4,
  PMEMCFG5 = 0x3A5,
  PMEMCFG6 = 0x3A6,
  PMEMCFG7 = 0x3A7
}CSR;

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.CSR_REG[MEPC] = epc;

  return 0;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
