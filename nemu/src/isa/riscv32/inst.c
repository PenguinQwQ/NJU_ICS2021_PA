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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <isa.h>

//R[addr]
#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

#define MSTATUS   0x300
#define MTVEC     0x305
#define MSCRATCH  0x340
#define MEPC      0x341
#define MCAUSE    0x342

#ifdef CONFIG_FTRACE
void ftrace_display(uint32_t addr);
#endif

//This Function Read the content stored in the CSR_REG[CSR_INDEX]!
word_t CSR_READ(uint32_t inst)
{
  int32_t CSR_ADDR = BITS(inst,31,20); //Decode the inst to find the CSR_ADDR
  if(CSR_ADDR > 1024 || CSR_ADDR < 0)
  {
    panic("The CSR Register Address is invalid!!!\n");
    return 0;
  }
  return cpu.CSR_REG[CSR_ADDR];
}
//This Function Write data to the CSR_REG[CSR_INDEX]!
void CSR_WRITE(uint32_t inst, word_t DATA)
{
  int32_t CSR_ADDR = BITS(inst,31,20); //Decode the inst to find the CSR_ADDR
  if(CSR_ADDR > 1024 || CSR_ADDR < 0)
  {
    panic("The CSR Register Address is invalid!!!\n");
    return ;
  }
  cpu.CSR_REG[CSR_ADDR] = DATA;
  return;
}


enum {
  TYPE_I, TYPE_U, TYPE_S, TYPE_J, TYPE_B, TYPE_R,
  TYPE_N, // none
};
//0,1,2,3
//The Marcos below defined reading source operands from registers
//Signal Extension/ Zero Extension methods
#define src1R() do { *src1 = R(rs1); } while (0) //
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 20) | (BITS(i, 19, 12) << 12) | (BITS(i, 20, 20) << 11) | (BITS(i, 30, 21) << 1);} while(0)
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | (BITS(i, 7, 7) << 11) | (BITS(i, 30, 25) << 5) | (BITS(i, 11, 8) << 1);} while(0);


//here decode the instruction, finding the rd,rs1,rs2
static void decode_operand(Decode *s, int *dest, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rd  = BITS(i, 11, 7);
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *dest = rd;
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J:                   immJ(); break;
    case TYPE_R: src1R(); src2R();         break;
    case TYPE_B: src1R(); src2R(); immB(); break;
    case TYPE_N: break;
  }
}
//this is the riscv32 decode process
static int decode_exec(Decode *s) {
  int dest = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  //src1,src2 are two ALU input source operands
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &dest, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(dest) = imm);
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc, U, R(dest) = s->pc + imm);
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal, J, s->dnpc = s->pc + imm ; R(dest) = s->snpc; 
                                                                                    #ifdef CONFIG_FTRACE
                                                                                    ftrace_display(s->dnpc);
                                                                                    #endif
                                                                                    );
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr, I, s->dnpc = src1 + imm ; R(dest) = s->snpc; 
                                                                                    #ifdef CONFIG_FTRACE
                                                                                    ftrace_display(s->dnpc);
                                                                                    #endif
                                                                                    );

//B-type Instructions
//BNE
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne, B, if(src1 != src2) s->dnpc = s->pc + imm);
//BEQ
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq, B, if(src1 == src2) s->dnpc = s->pc + imm);
//BLT
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt, B, if((sword_t)src1 < (sword_t)src2) s->dnpc = s->pc + imm);
//BGE
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge, B, if((sword_t)src1 >= (sword_t)src2) s->dnpc = s->pc + imm);
//BLTU
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu, B, if(src1 < src2) s->dnpc = s->pc + imm);
//BGEU
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu, B, if(src1 >= src2) s->dnpc = s->pc + imm);
//load instructions
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb      ,I  , R(dest) = SEXT(Mr(src1 + imm,1),8));
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh      ,I  , R(dest) = SEXT(Mr(src1 + imm,2),16));
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw      ,I  , R(dest) = Mr(src1 + imm, 4));
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu     ,I  , R(dest) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu     ,I  , R(dest) = Mr(src1 + imm, 2));
    
//store instructions
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb      ,S  , Mw(src1 + imm, 1, src2));
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh      ,S  , Mw(src1 + imm, 2, src2));
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw      ,S  , Mw(src1 + imm, 4, src2));
  INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd      ,S  , Mw(src1 + imm, 8, src2));
    
//imm operations    
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi    ,I  , R(dest) = (src1 + imm) );
  INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti    ,I  , R(dest) = ((sword_t)src1 < (sword_t)imm) );
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu   ,I  , R(dest) = (src1 < imm) );
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori    ,I  , R(dest) = (src1 ^ imm) );
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori     ,I  , R(dest) = (src1 | imm) );
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi    ,I  , R(dest) = (src1 & imm) );
  INSTPAT("000000 ?????? ????? 001 ????? 00100 11", slli    ,I  , R(dest) = (src1 << imm) );
  INSTPAT("000000 ?????? ????? 101 ????? 00100 11", srli    ,I  , R(dest) = (src1 >> imm) );
  INSTPAT("010000 ?????? ????? 101 ????? 00100 11", srai    ,I  , R(dest) = ((sword_t)src1 >> (sword_t)imm) );
  
//MULH
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh, R, R(dest) = BITS(1LL * SEXT(src1, 32) * SEXT(src2, 32), 63, 32));
//MULHU  
  INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu, R, R(dest) = BITS(1LL * src1 * src2, 63, 32));
//MUL
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul, R, R(dest) = src1 * src2);


//  INSTPAT("0000001 ????? ????? 010 ????? 01100 11", mulhsu  ,R  , R(dest)=((__int128_t)src1 * (__uint128_t) src2) >> 64);
//  INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu   ,R  , R(dest) = ((__uint128_t)src1*(__uint128_t)src2) >> 64);
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div     ,R  , R(dest)=(sword_t)src1 /  (sword_t)src2);
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu    ,R  , R(dest)=src1 / src2);
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem     ,R  , R(dest)=(sword_t)src1 % (sword_t)src2);
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu    ,R  , R(dest)=src1 % src2);
   
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add     ,R  , R(dest) = (src1 + src2) );
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub     ,R  , R(dest) = (src1 - src2) );
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll     ,R  , R(dest) = (src1 << src2) );
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt     ,R  , R(dest) = ((sword_t)src1 < (sword_t)src2) );
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu    ,R  , R(dest) = (src1 < src2) );
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor     ,R  , R(dest) = (src1 ^ src2) );
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl     ,R  , R(dest) = (src1 >> src2) );
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra     ,R  , R(dest) = ((sword_t)src1 >> (sword_t)src2) );
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or      ,R  , R(dest) = (src1 | src2) );
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and     ,R  , R(dest) = (src1 & src2) );    

  INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall   ,N  , s->dnpc = isa_raise_intr(11 ,s->pc));
  INSTPAT("0011000 00010 00000 000 00000 11100 11", mret    ,N  , s->dnpc = cpu.CSR_REG[MEPC]; cpu.CSR_REG[MSTATUS] &= ~(0x1800););

  INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw   ,I  , R(dest) = CSR_READ(s->isa.inst.val); CSR_WRITE(s->isa.inst.val, src1););
  INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrs   ,I  , R(dest) = CSR_READ(s->isa.inst.val); CSR_WRITE(s->isa.inst.val, R(dest) | src1););
  INSTPAT("??????? ????? ????? 011 ????? 11100 11", csrrc   ,I  , R(dest) = CSR_READ(s->isa.inst.val); CSR_WRITE(s->isa.inst.val, R(dest) & (~src1)););



  INSTPAT("0000000 00000 00000 001 00000 00011 11", fencei , I, ); 
  
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  
  
  
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
