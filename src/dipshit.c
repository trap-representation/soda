#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

#include "errors.h"
#include "opcodes.h"
#include "prof.h"
#include "sysf.h"
#include "regs.h"

static void *aligned_malloc(size_t alignment, size_t size){
  void *return_mem;
  if(size % alignment != 0){
    size += (size + alignment - 1) - ((size + alignment - 1) % alignment) - size;
  }
  return_mem = aligned_alloc(alignment, size);
  return return_mem;
}

EMSCRIPTEN_KEEPALIVE int eval(void *code, ysm_l cs, void *heap, ysm_l heap_size, ysm_ui heap_alignment, ysm_i *input_device, ysm_i *output_device) {
  ysm_l reg[1 + reg_gpr0 + 7];
  ysm_l *stack, *call_stack;
  ysm_uc *code_uc;
  ysm_us *code_us;
  ysm_ui *code_ui;
  ysm_c *code_c;
  ysm_s *code_s;
  ysm_i *code_i;
  ysm_l *code_l;
  ysm_uc *heap_uc;
  ysm_us *heap_us;
  ysm_ui *heap_ui;
  ysm_c *heap_c;
  ysm_s *heap_s;
  ysm_i *heap_i;
  ysm_l *heap_l;
  ysm_uc uc, uc_1;
  ysm_us us, us_1;
  ysm_ui ui, ui_1;
  ysm_nof_uns uns, uns_1;
  ysm_c c, c_1;
  ysm_s s, s_1;
  ysm_i i, i_1;
  ysm_l l, l_1;
  ysm_uc opcode;

  /* dipshit specific */

  ysm_l input_device_pt;
  ysm_l output_device_pt;

  /* initialize stuff */

  if ((stack = aligned_malloc(_Alignof(ysm_l), STACK_CELLS * sizeof(ysm_l))) == NULL) {
    return err_implalloc;
  }

  if ((call_stack = aligned_malloc(_Alignof(ysm_l), CALLSTACK_CELLS * sizeof(ysm_l))) == NULL) {
    free(stack);
    return err_implalloc;
  }

  reg[reg_sp] = 0;
  reg[reg_pc] = 0;
  reg[reg_cs] = cs;
  reg[reg_ia] = 0;
  reg[reg_hsz] = heap_size;
  reg[reg_clp] = 0;
  reg[reg_lop] = op_nop;

  code_uc = code;
  code_us = code;
  code_ui = code;
  code_c = code;
  code_s = code;
  code_i = code;
  code_l = code;
  heap_uc = heap;
  heap_us = heap;
  heap_ui = heap;
  heap_c = heap;
  heap_s = heap;
  heap_i = heap;
  heap_l = heap;

  /* dipshit specific */

  input_device_pt = 0;
  output_device_pt = 0;

  output_device[output_device_pt] = -1;

  /* --- */

  while (1) {
    if (reg[reg_pc]  >=  reg[reg_cs] || reg[reg_pc] < reg[reg_cb]) {
      reg[reg_ia] = reg[reg_pc];
      free(stack);
      free(call_stack);
      return err_trap;
    }

    opcode = code_uc[reg[reg_pc]];
    reg[reg_lop] = opcode;
    switch (opcode) {
    case op_trap:
      reg[reg_ia] = reg[reg_pc];
      free(stack);
      free(call_stack);
      return err_trap;

    case op_halt:
      free(stack);
      free(call_stack);
      return err_success;

    case op_haltr:
      l = stack[reg[reg_sp]-1];
      free(stack);
      free(call_stack);
      return l;

    case op_rbs:
      reg[reg_pc]++;
      break;

    case op_rbe:
      reg[reg_pc]++;
      break;

    case op_sts:
      stack[stack[reg[reg_sp]-1]] = stack[reg[reg_sp]-2];
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_pushuc:
      reg[reg_pc]++;
      stack[reg[reg_sp]] = code_uc[reg[reg_pc]];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_pushus:
      reg[reg_pc]++;
      memcpy(&us, &code_uc[reg[reg_pc]], sizeof(ysm_us));
      stack[reg[reg_sp]] = us;
      reg[reg_sp]++;
      reg[reg_pc] += SIZEOF_US;
      break;

    case op_pushui:
      reg[reg_pc]++;
      memcpy(&ui, &code_uc[reg[reg_pc]], sizeof(ysm_ui));
      stack[reg[reg_sp]] = ui;
      reg[reg_sp]++;
      reg[reg_pc] += SIZEOF_UI;
      break;

    case op_pushc:
      reg[reg_pc]++;
      stack[reg[reg_sp]] = ((ysm_c *) code)[reg[reg_pc]];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_pushs:
      reg[reg_pc]++;
      memcpy(&s, &code_uc[reg[reg_pc]], sizeof(ysm_s));
      stack[reg[reg_sp]] = s;
      reg[reg_sp]++;
      reg[reg_pc] += SIZEOF_S;
      break;

    case op_pushi:
      reg[reg_pc]++;
      memcpy(&i, &code_uc[reg[reg_pc]], sizeof(ysm_i));
      stack[reg[reg_sp]] = i;
      reg[reg_sp]++;
      reg[reg_pc] += SIZEOF_I;
      break;

    case op_pushl:
      reg[reg_pc]++;
      memcpy(&stack[reg[reg_sp]], &code_uc[reg[reg_pc]], sizeof(ysm_l));
      reg[reg_sp]++;
      reg[reg_pc] += SIZEOF_L;
      break;

    case op_pushpc:
      stack[reg[reg_sp]] = reg[reg_pc];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_pushcs:
      stack[reg[reg_sp]] = reg[reg_cs];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_pushsp:
      stack[reg[reg_sp]] = reg[reg_sp];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_pushn1:
      stack[reg[reg_sp]] = -1;
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_push0:
      stack[reg[reg_sp]] = 0;
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_push1:
      stack[reg[reg_sp]] = 1;
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_push2:
      stack[reg[reg_sp]] = 2;
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_push3:
      stack[reg[reg_sp]] = 3;
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_push4:
      stack[reg[reg_sp]] = 4;
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_push5:
      stack[reg[reg_sp]] = 5;
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_push7:
      stack[reg[reg_sp]] = 7;
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_adduc:
      stack[reg[reg_sp] - 2] = (ysm_uc) (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) + ((ysm_uc) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_addus:
      stack[reg[reg_sp] - 2] = (ysm_us) (((ysm_us)stack[reg[reg_sp] - 1] + 0u) + ((ysm_us) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_addui:
      stack[reg[reg_sp] - 2] = (ysm_ui) (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) + ((ysm_ui) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_addc:
      stack[reg[reg_sp] - 2 ]= (ysm_c) ((ysm_c) stack[reg[reg_sp] - 1] + (ysm_c) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_adds:
      stack[reg[reg_sp]-2] = (ysm_s) ((ysm_s) stack[reg[reg_sp] - 1] + (ysm_s) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_addi:
      stack[reg[reg_sp] - 2] = (ysm_i) ((ysm_i) stack[reg[reg_sp] - 1] + (ysm_i) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_addl:
      stack[reg[reg_sp] - 2] = stack[reg[reg_sp] - 1] + stack[reg[reg_sp] - 2];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_subuc:
      stack[reg[reg_sp] - 2] = (ysm_uc) (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) - ((ysm_uc) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_subus:
      stack[reg[reg_sp] - 2] = (ysm_us) (((ysm_us) stack[reg[reg_sp] - 1] + 0u) - ((ysm_us) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_subui:
      stack[reg[reg_sp] - 2] = (ysm_ui) (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) - ((ysm_ui) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_subc:
      stack[reg[reg_sp] - 2] = (ysm_c) ((ysm_c) stack[reg[reg_sp] - 1]-(ysm_c) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_subs:
      stack[reg[reg_sp] - 2] = (ysm_s) ((ysm_s) stack[reg[reg_sp] - 1] - (ysm_s) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_subi:
      stack[reg[reg_sp] - 2] = (ysm_i) ((ysm_i) stack[reg[reg_sp] - 1] - (ysm_i) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_subl:
      stack[reg[reg_sp] - 2] = stack[reg[reg_sp] - 1] - stack[reg[reg_sp] - 2];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_muluc:
      stack[reg[reg_sp] - 2] = (ysm_uc) (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) * ((ysm_uc) stack[reg[reg_sp]-2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_mulus:
      stack[reg[reg_sp] - 2] = (ysm_us) (((ysm_us) stack[reg[reg_sp] - 1] + 0u) * ((ysm_us) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_mului:
      stack[reg[reg_sp] - 2] = (ysm_ui) (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) * ((ysm_ui) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_mulc:
      stack[reg[reg_sp] - 2] = (ysm_c) ((ysm_c) stack[reg[reg_sp] - 1] * (ysm_c) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_muls:
      stack[reg[reg_sp] - 2] = (ysm_s) ((ysm_s) stack[reg[reg_sp] - 1] * (ysm_s) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_muli:
      stack[reg[reg_sp] - 2] = (ysm_i) ((ysm_i) stack[reg[reg_sp] - 1] * (ysm_i) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_mull:
      stack[reg[reg_sp] - 2] = stack[reg[reg_sp] - 1] * stack[reg[reg_sp] - 2];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_remuc:
      stack[reg[reg_sp] - 2] = (ysm_uc) (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) % ((ysm_uc) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_remus:
      stack[reg[reg_sp] - 2] = (ysm_us) (((ysm_us) stack[reg[reg_sp] - 1] + 0u) % ((ysm_us) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_remui:
      stack[reg[reg_sp] - 2] = (ysm_ui) (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) % ((ysm_ui) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_remc:
      stack[reg[reg_sp] - 2] = (ysm_c)((ysm_c)stack[reg[reg_sp]-1]%(ysm_c)stack[reg[reg_sp]-2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;
    case op_rems:
      stack[reg[reg_sp] - 2] = (ysm_s) ((ysm_s) stack[reg[reg_sp] - 1] % (ysm_s) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_remi:
      stack[reg[reg_sp] - 2] = (ysm_i)((ysm_i) stack[reg[reg_sp] - 1] % (ysm_i) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_reml:
      stack[reg[reg_sp] - 2] = stack[reg[reg_sp] - 1] % stack[reg[reg_sp] - 2];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_divuc:
      stack[reg[reg_sp] - 2] = (ysm_uc) (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) / ((ysm_uc) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_divus:
      stack[reg[reg_sp] - 2] = (ysm_us) (((ysm_us) stack[reg[reg_sp] - 1] + 0u) / ((ysm_us) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_divui:
      stack[reg[reg_sp] - 2] = (ysm_ui) (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) / ((ysm_ui) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_divc:
      stack[reg[reg_sp] - 2] = (ysm_c) ((ysm_c) stack[reg[reg_sp] - 1] / (ysm_c) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_divs:
      stack[reg[reg_sp] - 2] = (ysm_s) ((ysm_s) stack[reg[reg_sp] - 1] / (ysm_s) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_divi:
      stack[reg[reg_sp] - 2] = (ysm_i) ((ysm_i) stack[reg[reg_sp] - 1] / (ysm_i) stack[reg[reg_sp] - 2]);
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_divl:
      stack[reg[reg_sp] - 2] = stack[reg[reg_sp] - 1] / stack[reg[reg_sp] - 2];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_lshuc:
      stack[reg[reg_sp] - 2] = (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) << ((ysm_uc) stack[reg[reg_sp] - 2] + 0u)) & 0xff;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_lshus:
      stack[reg[reg_sp] - 2] = (((ysm_us) stack[reg[reg_sp] - 1] + 0u) << ((ysm_us) stack[reg[reg_sp] - 2] + 0u)) & 0xffff;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_lshui:
      stack[reg[reg_sp]-2] = (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) << ((ysm_ui) stack[reg[reg_sp] - 2] + 0u)) & 0xffffff;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_lshc:
      c = stack[reg[reg_sp]-1];
      c_1 = stack[reg[reg_sp]-2];
      memcpy(&uc, &c, sizeof(ysm_uc));
      memcpy(&uc_1, &c_1, sizeof(ysm_uc));
      uc = ((uc + 0u) << (uc_1 + 0u)) & 0xff;
      memcpy(&c, &uc, sizeof(ysm_c));
      stack[reg[reg_sp] - 2] = c;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_lshs:
      s = stack[reg[reg_sp] - 1];
      s_1 = stack[reg[reg_sp] - 2];
      memcpy(&us, &s, sizeof(ysm_us));
      memcpy(&us_1, &s_1, sizeof(ysm_us));
      us = ((us + 0u) << (us_1 + 0u)) & 0xffff;
      memcpy(&s, &us, sizeof(ysm_s));
      stack[reg[reg_sp] - 2] = s;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_lshi:
      i = stack[reg[reg_sp] - 1];
      i_1 = stack[reg[reg_sp] - 2];
      memcpy(&ui, &i, sizeof(ysm_ui));
      memcpy(&ui_1, &i_1, sizeof(ysm_ui));
      ui = ((ui + 0u) << (ui_1 + 0u)) & 0xffffff;
      memcpy(&i, &ui, sizeof(ysm_i));
      stack[reg[reg_sp]-2] = i;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_lshl:
      l = stack[reg[reg_sp] - 1];
      l_1 = stack[reg[reg_sp] - 2];
      memcpy(&uns, &l, sizeof(ysm_nof_uns));
      memcpy(&uns_1, &l_1, sizeof(ysm_nof_uns));
      uns = ((uns + 0u) << (uns_1 + 0u)) & 0xffffffff;
      memcpy(&l, &uns, sizeof(ysm_l));
      stack[reg[reg_sp] - 2] = l;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_rshuc:
      stack[reg[reg_sp] - 2] = (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) >> ((ysm_uc) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_rshus:
      stack[reg[reg_sp] - 2] = (((ysm_us) stack[reg[reg_sp] - 1] + 0u) >> ((ysm_us) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_rshui:
      stack[reg[reg_sp] - 2] = (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) >> ((ysm_ui) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_rshc:
      c = stack[reg[reg_sp] - 1];
      c_1 =stack[reg[reg_sp] - 2];
      memcpy(&uc, &c, sizeof(ysm_uc));
      memcpy(&uc_1, &c_1, sizeof(ysm_uc));
      uc = ((uc + 0u) >> (uc_1 + 0u));
      memcpy(&c, &uc, sizeof(ysm_c));
      stack[reg[reg_sp] - 2] = c;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_rshs:
      s = stack[reg[reg_sp] - 1];
      s_1 = stack[reg[reg_sp] - 2];
      memcpy(&us, &s, sizeof(ysm_us));
      memcpy(&us_1, &s_1, sizeof(ysm_us));
      us = ((us + 0u) >> (us_1 + 0u));
      memcpy(&s, &us, sizeof(ysm_s));
      stack[reg[reg_sp] - 2] = s;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_rshi:
      i = stack[reg[reg_sp] - 1];
      i_1 = stack[reg[reg_sp] - 2];
      memcpy(&ui, &i, sizeof(ysm_ui));
      memcpy(&ui_1, &i_1, sizeof(ysm_ui));
      ui = ((ui + 0u) >> (ui_1 + 0u));
      memcpy(&i, &ui, sizeof(ysm_i));
      stack[reg[reg_sp] - 2] = i;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_rshl:
      l = stack[reg[reg_sp] - 1];
      l_1 = stack[reg[reg_sp] - 2];
      memcpy(&uns, &l, sizeof(ysm_nof_uns));
      memcpy(&uns_1, &l_1, sizeof(ysm_nof_uns));
      uns=((uns + 0u) >> (uns_1 + 0u));
      memcpy(&l, &uns, sizeof(ysm_l));
      stack[reg[reg_sp] - 2] = l;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_anduc:
      stack[reg[reg_sp] - 2] = (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) & ((ysm_uc) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_andus:
      stack[reg[reg_sp] - 2] = (((ysm_us) stack[reg[reg_sp]-1] + 0u) & ((ysm_us) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_andui:
      stack[reg[reg_sp] - 2] = (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) & ((ysm_ui) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_andc:
      c = stack[reg[reg_sp] - 1];
      c_1 = stack[reg[reg_sp] - 2];
      memcpy(&uc, &c, sizeof(ysm_uc));
      memcpy(&uc_1, &c_1, sizeof(ysm_uc));
      uc = ((uc + 0u) & (uc_1 + 0u));
      memcpy(&c, &uc, sizeof(ysm_c));
      stack[reg[reg_sp] - 2] = c;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_ands:
      s = stack[reg[reg_sp] - 1];
      s_1 = stack[reg[reg_sp] - 2];
      memcpy(&us, &s, sizeof(ysm_us));
      memcpy(&us_1, &s_1, sizeof(ysm_us));
      us = ((us + 0u) & (us_1 + 0u));
      memcpy(&s, &us, sizeof(ysm_s));
      stack[reg[reg_sp] - 2] = s;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_andi:
      i = stack[reg[reg_sp] - 1];
      i_1 = stack[reg[reg_sp] - 2];
      memcpy(&ui, &i, sizeof(ysm_ui));
      memcpy(&ui_1, &i_1, sizeof(ysm_ui));
      ui = ((ui + 0u) & (ui_1 + 0u));
      memcpy(&i, &ui, sizeof(ysm_i));
      stack[reg[reg_sp] - 2] = i;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_andl:
      l = stack[reg[reg_sp] - 1];
      l_1 = stack[reg[reg_sp] - 2];
      memcpy(&uns, &l, sizeof(ysm_nof_uns));
      memcpy(&uns_1, &l_1, sizeof(ysm_nof_uns));
      uns = ((uns + 0u) & (uns_1 + 0u));
      memcpy(&l, &uns, sizeof(ysm_l));
      stack[reg[reg_sp] - 2] = l;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_oruc:
      stack[reg[reg_sp] - 2] = (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) | ((ysm_uc) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_orus:
      stack[reg[reg_sp] - 2] = (((ysm_us) stack[reg[reg_sp] - 1] + 0u) | ((ysm_us) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_orui:
      stack[reg[reg_sp] - 2] = (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) | ((ysm_ui) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_orc:
      c = stack[reg[reg_sp] - 1];
      c_1 = stack[reg[reg_sp] - 2];
      memcpy(&uc, &c, sizeof(ysm_uc));
      memcpy(&uc_1, &c_1, sizeof(ysm_uc));
      uc = ((uc + 0u) | (uc_1 + 0u));
      memcpy(&c, &uc, sizeof(ysm_c));
      stack[reg[reg_sp] - 2] = c;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_ors:
      s = stack[reg[reg_sp] - 1];
      s_1 = stack[reg[reg_sp] - 2];
      memcpy(&us, &s, sizeof(ysm_us));
      memcpy(&us_1, &s_1, sizeof(ysm_us));
      us = ((us + 0u) | (us_1 + 0u));
      memcpy(&s, &us, sizeof(ysm_s));
      stack[reg[reg_sp] - 2] = s;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_ori:
      i = stack[reg[reg_sp] - 1];
      i_1 = stack[reg[reg_sp] - 2];
      memcpy(&ui, &i, sizeof(ysm_ui));
      memcpy(&ui_1, &i_1, sizeof(ysm_ui));
      ui = ((ui + 0u) | (ui_1 + 0u));
      memcpy(&i, &ui, sizeof(ysm_i));
      stack[reg[reg_sp] - 2] = i;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_orl:
      l = stack[reg[reg_sp] - 1];
      l_1 = stack[reg[reg_sp] - 2];
      memcpy(&uns, &l, sizeof(ysm_nof_uns));
      memcpy(&uns_1, &l_1, sizeof(ysm_nof_uns));
      uns = ((uns + 0u) | (uns_1 + 0u));
      memcpy(&l, &uns, sizeof(ysm_l));
      stack[reg[reg_sp] - 2] = l;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_xoruc:
      stack[reg[reg_sp] - 2] = (((ysm_uc) stack[reg[reg_sp] - 1] + 0u) ^ ((ysm_uc) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_xorus:
      stack[reg[reg_sp] - 2] = (((ysm_us) stack[reg[reg_sp] - 1] + 0u) ^ ((ysm_us) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_xorui:
      stack[reg[reg_sp] - 2] = (((ysm_ui) stack[reg[reg_sp] - 1] + 0u) ^ ((ysm_ui) stack[reg[reg_sp] - 2] + 0u));
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_xorc:
      c = stack[reg[reg_sp] - 1];
      c_1 = stack[reg[reg_sp] - 2];
      memcpy(&uc, &c, sizeof(ysm_uc));
      memcpy(&uc_1, &c_1, sizeof(ysm_uc));
      uc = ((uc + 0u) ^ (uc_1 + 0u));
      memcpy(&c, &uc, sizeof(ysm_c));
      stack[reg[reg_sp] - 2] = c;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_xors:
      s = stack[reg[reg_sp] - 1];
      s_1 = stack[reg[reg_sp] - 2];
      memcpy(&us, &s, sizeof(ysm_us));
      memcpy(&us_1, &s_1, sizeof(ysm_us));
      us = ((us + 0u) ^ (us_1 + 0u));
      memcpy(&s, &us, sizeof(ysm_s));
      stack[reg[reg_sp] - 2] = s;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_xori:
      i = stack[reg[reg_sp] - 1];
      i_1 = stack[reg[reg_sp] - 2];
      memcpy(&ui, &i, sizeof(ysm_ui));
      memcpy(&ui_1, &i_1, sizeof(ysm_ui));
      ui = ((ui + 0u) ^ (ui_1 + 0u));
      memcpy(&i, &ui, sizeof(ysm_i));
      stack[reg[reg_sp] - 2] = i;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_xorl:
      l = stack[reg[reg_sp] - 1];
      l_1 = stack[reg[reg_sp] - 2];
      memcpy(&uns, &l, sizeof(ysm_nof_uns));
      memcpy(&uns_1, &l_1, sizeof(ysm_nof_uns));
      uns = ((uns + 0u) ^ (uns_1 + 0u));
      memcpy(&l, &uns, sizeof(ysm_l));
      stack[reg[reg_sp] - 2] = l;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_notuc:
      stack[reg[reg_sp] - 1] = ~ ((ysm_uc) stack[reg[reg_sp] - 1] + 0u) & 0xff;
      reg[reg_pc]++;
      break;

    case op_notus:
      stack[reg[reg_sp] - 1] = ~ ((ysm_us) stack[reg[reg_sp] - 1] + 0u) & 0xffff;
      reg[reg_pc]++;
      break;

    case op_notui:
      stack[reg[reg_sp]-1] = ~ ((ysm_ui) stack[reg[reg_sp] - 1] + 0u) & 0xffffff;
      reg[reg_pc]++;
      break;

    case op_notc:
      c = stack[reg[reg_sp] - 1];
      memcpy(&uc, &c, sizeof(ysm_uc));
      uc = ~ (uc + 0u) & 0xff;
      memcpy(&c, &uc, sizeof(ysm_c));
      stack[reg[reg_sp] - 1] = c;
      reg[reg_pc]++;
      break;

    case op_nots:
      s = stack[reg[reg_sp]-1];
      memcpy(&us, &s, sizeof(ysm_us));
      us = ~ (us + 0u) & 0xffff;
      memcpy(&s, &us, sizeof(ysm_s));
      stack[reg[reg_sp] - 1] = s;
      reg[reg_pc]++;
      break;

    case op_noti:
      i = stack[reg[reg_sp] - 1];
      memcpy(&ui, &i, sizeof(ysm_ui));
      ui = ~(ui + 0u) & 0xffffff;
      memcpy(&i, &ui, sizeof(ysm_i));
      stack[reg[reg_sp] - 1] = i;
      reg[reg_pc]++;
      break;

    case op_notl:
      l = stack[reg[reg_sp] - 1];
      memcpy(&uns, &l, sizeof(ysm_nof_uns));
      uns = ~ (uns + 0u) & 0xffffffff;
      memcpy(&l, &uns, sizeof(ysm_l));
      stack[reg[reg_sp] - 1] = l;
      reg[reg_pc]++;
      break;

    case op_ldcuc:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      memcpy(&uc, &code_uc[stack[reg[reg_sp]-1]], sizeof(ysm_uc));
      stack[reg[reg_sp] - 1] = uc;
      reg[reg_pc]++;
      break;

    case op_aldcuc:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      stack[reg[reg_sp] - 1] = code_uc[stack[reg[reg_sp]]];
      reg[reg_pc]++;
      break;

    case op_ldcus:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      memcpy(&us, &code_uc[stack[reg[reg_sp]-1]], sizeof(ysm_us));
      stack[reg[reg_sp] - 1] = us;
      reg[reg_pc]++;
      break;

    case op_aldcus:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      stack[reg[reg_sp] - 1] = code_us[stack[reg[reg_sp]] / SIZEOF_US];
      reg[reg_pc]++;
      break;

    case op_ldcui:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      memcpy(&ui, &code_uc[stack[reg[reg_sp]-1]], sizeof(ysm_ui));
      stack[reg[reg_sp] - 1] = ui;
      reg[reg_pc]++;
      break;

    case op_aldcui:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1]<reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      stack[reg[reg_sp] - 1] = code_ui[stack[reg[reg_sp]] / SIZEOF_UI];
      reg[reg_pc]++;
      break;

    case op_ldcc:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      memcpy(&c, &code_uc[stack[reg[reg_sp] - 1]], sizeof(ysm_c));
      stack[reg[reg_sp] - 1] = c;
      reg[reg_pc]++;
      break;

    case op_aldcc:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      stack[reg[reg_sp] - 1] = code_c[stack[reg[reg_sp]]];
      reg[reg_pc]++;
      break;

    case op_ldcs:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      memcpy(&s, &code_uc[stack[reg[reg_sp]-1]], sizeof(ysm_s));
      stack[reg[reg_sp] - 1] = s;
      reg[reg_pc]++;
      break;

    case op_aldcs:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      stack[reg[reg_sp] - 1] = code_s[stack[reg[reg_sp]] / SIZEOF_S];
      reg[reg_pc]++;
      break;

    case op_ldci:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      memcpy(&i, &code_uc[stack[reg[reg_sp]-1]], sizeof(ysm_i));
      stack[reg[reg_sp] - 1] = i;
      reg[reg_pc]++;
      break;

    case op_aldci:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1]<reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      stack[reg[reg_sp] - 1] = code_i[stack[reg[reg_sp]] / SIZEOF_I];
      reg[reg_pc]++;
      break;

    case op_ldcl:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1]<reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      memcpy(&stack[reg[reg_sp] - 1], &code_uc[stack[reg[reg_sp] - 1]], sizeof(ysm_l));
      reg[reg_pc]++;
      break;

    case op_aldcl:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      stack[reg[reg_sp] - 1] = code_l[stack[reg[reg_sp]] / SIZEOF_L];
      reg[reg_pc]++;
      break;

    case op_ldhuc:
      memcpy(&uc, &heap_uc[stack[reg[reg_sp] - 1]], sizeof(ysm_uc));
      stack[reg[reg_sp] - 1] = uc;
      reg[reg_pc]++;
      break;

    case op_aldhuc:
      stack[reg[reg_sp] - 1] = heap_uc[stack[reg[reg_sp]]];
      reg[reg_pc]++;
      break;

    case op_ldhus:
      memcpy(&us, &heap_uc[stack[reg[reg_sp] - 1]], sizeof(ysm_us));
      stack[reg[reg_sp] - 1] = us;
      reg[reg_pc]++;
      break;

    case op_aldhus:
      stack[reg[reg_sp] - 1] = heap_uc[stack[reg[reg_sp]] / SIZEOF_US];
      reg[reg_pc]++;
      break;

    case op_ldhui:
      memcpy(&ui, &heap_uc[stack[reg[reg_sp] - 1]], sizeof(ysm_ui));
      stack[reg[reg_sp] - 1] = ui;
      reg[reg_pc]++;
      break;

    case op_aldhui:
      stack[reg[reg_sp] - 1] = heap_ui[stack[reg[reg_sp]] / SIZEOF_UI];
      reg[reg_pc]++;
      break;

    case op_ldhc:
      memcpy(&c, &heap_uc[stack[reg[reg_sp]-1]], sizeof(ysm_c));
      stack[reg[reg_sp] - 1] = c;
      reg[reg_pc]++;
      break;

    case op_aldhc:
      stack[reg[reg_sp] - 1] = heap_c[stack[reg[reg_sp]]];
      reg[reg_pc]++;
      break;

    case op_ldhs:
      memcpy(&s, &heap_uc[stack[reg[reg_sp] - 1]], sizeof(ysm_s));
      stack[reg[reg_sp] - 1] = s;
      reg[reg_pc]++;
      break;

    case op_aldhs:
      stack[reg[reg_sp] - 1] = heap_s[stack[reg[reg_sp]] / SIZEOF_S];
      reg[reg_pc]++;
      break;

    case op_ldhi:
      memcpy(&i, &heap_uc[stack[reg[reg_sp]-1]], sizeof(ysm_i));
      stack[reg[reg_sp] - 1] = i;
      reg[reg_pc]++;
      break;

    case op_aldhi:
      stack[reg[reg_sp] - 1] = heap_i[stack[reg[reg_sp]] / SIZEOF_I];
      reg[reg_pc]++;
      break;

    case op_ldhl:
      memcpy(&stack[reg[reg_sp] - 1], &heap_uc[stack[reg[reg_sp] - 1]], sizeof(ysm_l));
      reg[reg_pc]++;
      break;

    case op_aldhl:
      stack[reg[reg_sp] - 1] = heap_l[stack[reg[reg_sp]] / SIZEOF_L];
      reg[reg_pc]++;
      break;

    case op_sthuc:
      uc=stack[reg[reg_sp] - 2];
      memcpy(&heap_uc[stack[reg[reg_sp] - 1]], &uc, sizeof(ysm_uc));
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_asthuc:
      heap_uc[stack[reg[reg_sp] - 1]] = stack[reg[reg_sp] - 2];
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_sthus:
      us=stack[reg[reg_sp] - 2];
      memcpy(&heap_uc[stack[reg[reg_sp] - 1]], &us, sizeof(ysm_us));
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_asthus:
      heap_us[stack[reg[reg_sp] - 1] / SIZEOF_US] = stack[reg[reg_sp] - 2];
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_sthui:
      ui=stack[reg[reg_sp] - 2];
      memcpy(&heap_uc[stack[reg[reg_sp] - 1]], &ui, sizeof(ysm_ui));
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_asthui:
      heap_ui[stack[reg[reg_sp] - 1] / SIZEOF_UI] = stack[reg[reg_sp] - 2];
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_sthc:
      c = stack[reg[reg_sp] - 2];
      memcpy(&heap_uc[stack[reg[reg_sp] - 1]], &c, sizeof(ysm_c));
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_asthc:
      heap_c[stack[reg[reg_sp] - 1]] = stack[reg[reg_sp] - 2];
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_sths:
      s=stack[reg[reg_sp] - 2];
      memcpy(&heap_uc[stack[reg[reg_sp] - 1]], &s, sizeof(ysm_s));
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_asths:
      heap_s[stack[reg[reg_sp] - 1] / SIZEOF_S] = stack[reg[reg_sp] - 2];
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_sthi:
      i=stack[reg[reg_sp] - 2];
      memcpy(&heap_uc[stack[reg[reg_sp] - 1]], &i, sizeof(ysm_i));
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_asthi:
      heap_i[stack[reg[reg_sp] - 1] / SIZEOF_I] = stack[reg[reg_sp] - 2];
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_sthl:
      l = stack[reg[reg_sp] - 2];
      memcpy(&heap_uc[stack[reg[reg_sp] - 1]], &l, sizeof(ysm_l));
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_asthl:
      heap_us[stack[reg[reg_sp] - 1] / SIZEOF_L] = stack[reg[reg_sp] - 2];
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_ldr0:
      stack[reg[reg_sp]] = reg[reg_gpr0];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_ldr1:
      stack[reg[reg_sp]] = reg[reg_gpr0 + 1];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_ldr2:
      stack[reg[reg_sp]] = reg[reg_gpr0 + 2];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_ldr3:
      stack[reg[reg_sp]] = reg[reg_gpr0 + 3];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_ldr4:
      stack[reg[reg_sp]] = reg[reg_gpr0 + 4];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_ldr5:
      stack[reg[reg_sp]] = reg[reg_gpr0 + 5];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_ldr6:
      stack[reg[reg_sp]] = reg[reg_gpr0 + 6];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_ldr7:
      stack[reg[reg_sp]] = reg[reg_gpr0 + 7];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_str0:
      reg[reg_gpr0] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_str1:
      reg[reg_gpr0 + 1] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_str2:
      reg[reg_gpr0 + 2] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_str3:
      reg[reg_gpr0 + 3] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_str4:
      reg[reg_gpr0 + 4] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_str5:
      reg[reg_gpr0 + 5] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_str6:
      reg[reg_gpr0 + 6] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_str7:
      reg[reg_gpr0 + 7] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_panic:
      if(stack[reg[reg_sp] - 1]){
	free(stack);
	free(call_stack);
        return err_panic;
      }
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_force_panic:
      free(stack);
      free(call_stack);
      return err_panic;

    case op_incsp:
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_decsp:
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_swap:
      l = stack[reg[reg_sp] - 1];
      stack[reg[reg_sp] - 1] = stack[reg[reg_sp] - 2];
      stack[reg[reg_sp] - 2] = l;
      reg[reg_pc]++;
      break;

    case op_pop:
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_copy:
      stack[reg[reg_sp] - 1] = stack[stack[reg[reg_sp] - 1]];
      reg[reg_pc]++;
      break;

    case op_pcopy:
      l = stack[reg[reg_sp] - 1];
      memmove(&stack[stack[reg[reg_sp] - 1]], &stack[stack[reg[reg_sp] - 1] + 1], reg[reg_sp] * sizeof(ysm_l));
      stack[reg[reg_sp] - 2] = l;
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_popa:
      l = stack[reg[reg_sp] - 1];
      memmove(&stack[stack[reg[reg_sp] - 1]], &stack[stack[reg[reg_sp] - 1] + 1], reg[reg_sp] * sizeof(ysm_l));
      reg[reg_sp] -= 2;
      reg[reg_pc]++;
      break;

    case op_dup:
      stack[reg[reg_sp]] = stack[reg[reg_sp] - 1];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_over:
      stack[reg[reg_sp]] = stack[reg[reg_sp] - 2];
      reg[reg_sp]++;
      reg[reg_pc]++;
      break;

    case op_rjgt:
      if(reg[reg_pc] + stack[reg[reg_sp]-1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] > stack[reg[reg_sp] - 3]){
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rjls:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] < stack[reg[reg_sp] - 3]){
        reg[reg_pc] += stack[reg[reg_sp]-1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rjeq:
      if(reg[reg_pc] + stack[reg[reg_sp]-1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] == stack[reg[reg_sp] - 3]){
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rjle:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] <= stack[reg[reg_sp] - 3]){
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rjge:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] >= stack[reg[reg_sp] - 3]){
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rjne:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] != stack[reg[reg_sp] - 3]){
        reg[reg_pc] += stack[reg[reg_sp]-1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rjz:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] == 0){
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 2;
      break;

    case op_rjnz:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] != 0){
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 2;
      break;

    case op_rjmp:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp]-1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      reg[reg_pc] += stack[reg[reg_sp]-1];
      reg[reg_sp]--;
      break;

    case op_jgt:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] > stack[reg[reg_sp] - 3]){
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_jls:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
	reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2]<stack[reg[reg_sp] - 3]){
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_jeq:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1]<reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] == stack[reg[reg_sp] - 3]){
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_jle:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] <= stack[reg[reg_sp] - 3]){
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_jge:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] >= stack[reg[reg_sp] - 3]){
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_jne:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2]!=stack[reg[reg_sp] - 3]){
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_jz:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] == 0){
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 2;
      break;

    case op_jnz:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2]!=0){
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 2;
      break;

    case op_jmp:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      reg[reg_pc] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      break;

    case op_ret:
      reg[reg_pc] = call_stack[reg[reg_clp] - 1];
      reg[reg_clp]--;
      break;

    case op_clgt:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] > stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] = stack[reg[reg_sp]-1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_clls:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] < stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_cleq:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] == stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_clle:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] <= stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_clge:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp]-2] >= stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_clne:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] != stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] = stack[reg[reg_sp]-1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_clz:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] == 0){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 2;
      break;

    case op_clnz:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] != 0){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] = stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 2;
      break;

    case op_call:
      if(stack[reg[reg_sp] - 1] >= reg[reg_cs] || stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
      reg[reg_clp]++;
      reg[reg_pc] = stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      break;

    case op_rclgt:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] > stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rclls:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] < stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rcleq:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] == stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rclle:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] <= stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rclge:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] >= stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rclne:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] != stack[reg[reg_sp] - 3]){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 3;
      break;

    case op_rclz:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] == 0){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 2;
      break;

    case op_rclnz:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      if(stack[reg[reg_sp] - 2] != 0){
        call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
        reg[reg_clp]++;
        reg[reg_pc] += stack[reg[reg_sp] - 1];
      }
      else{
        reg[reg_pc]++;
      }
      reg[reg_sp] -= 2;
      break;

    case op_rcall:
      if(reg[reg_pc] + stack[reg[reg_sp] - 1] >= reg[reg_cs] || reg[reg_pc] + stack[reg[reg_sp] - 1] < reg[reg_cb]){
        reg[reg_ia] = reg[reg_pc] + stack[reg[reg_sp] - 1];
	free(stack);
	free(call_stack);
        return err_trap;
      }
      call_stack[reg[reg_clp]] = reg[reg_pc] + 1;
      reg[reg_clp]++;
      reg[reg_pc] += stack[reg[reg_sp] - 1];
      reg[reg_sp]--;
      break;

    case op_cgt:
      if(stack[reg[reg_sp] - 1]>stack[reg[reg_sp] - 2]){
        stack[reg[reg_sp] - 2] = 1;
      }
      else{
        stack[reg[reg_sp] - 2] = 0;
      }
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_cls:
      if(stack[reg[reg_sp] - 1]<stack[reg[reg_sp] - 2]){
        stack[reg[reg_sp] - 2] = 1;
      }
      else{
        stack[reg[reg_sp] - 2] = 0;
      }
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_ceq:
      if(stack[reg[reg_sp] - 1] == stack[reg[reg_sp] - 2]){
        stack[reg[reg_sp] - 2] = 1;
      }
      else{
        stack[reg[reg_sp] - 2] = 0;
      }
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_cle:
      if(stack[reg[reg_sp] - 1] <= stack[reg[reg_sp] - 2]){
        stack[reg[reg_sp] - 2] = 1;
      }
      else{
        stack[reg[reg_sp] - 2] = 0;
      }
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_cge:
      if(stack[reg[reg_sp] - 1] >= stack[reg[reg_sp] - 2]){
        stack[reg[reg_sp] - 2] = 1;
      }
      else{
        stack[reg[reg_sp] - 2] = 0;
      }
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_cne:
      if(stack[reg[reg_sp] - 1]!=stack[reg[reg_sp] - 2]){
        stack[reg[reg_sp] - 2] = 1;
      }
      else{
        stack[reg[reg_sp] - 2] = 0;
      }
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_cz:
      if(stack[reg[reg_sp] - 1] == 0){
        stack[reg[reg_sp] - 1] = 1;
      }
      else{
        stack[reg[reg_sp] - 1] = 0;
      }
      reg[reg_pc]++;
      break;

    case op_cnz:
      if(stack[reg[reg_sp] - 1]!=0){
        stack[reg[reg_sp] - 1] = 1;
      }
      else{
        stack[reg[reg_sp] - 1] = 0;
      }
      reg[reg_pc]++;
      break;

    case op_nop:
      reg[reg_pc]++;
      break;

    case op_open:
      if (open(stack[reg[reg_sp] - 1]) == err_open) {
	return err_open;
      }
      reg[reg_sp]--;
      reg[reg_pc]++;
      break;

    case op_invoke:
      reg[reg_sp] -= 2;
      if (invoke(stack[reg[reg_sp]], stack[reg[reg_sp] + 1], code, heap, heap_alignment, stack, reg, input_device, &input_device_pt, output_device, &output_device_pt) == err_invoke) {
	return err_invoke;
      }
      reg[reg_pc]++;
      break;

    default:
      free(stack);
      free(call_stack);
      return err_illegalinstruction;
    }
  }

  return err_success;
}
