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

#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

#define CSR_REG_NUM 1025

typedef struct {
  word_t gpr[32];//General purpose registers, 32!
  vaddr_t pc;//program counter

  word_t CSR_REG[CSR_REG_NUM];
} riscv32_CPU_state;

// decode
typedef struct {
  union { //share the memory
    uint32_t val;
  } inst;//as 
} riscv32_ISADecodeInfo;

#define isa_mmu_check(vaddr, len, type) (cpu.CSR_REG[0x180] >> 31)
#endif
