#include <stdio.h>

#include "regs.h"
#include "sysf.h"
#include "temp.h"

static void ds_putstr(ysm_c *code, ysm_l off, ysm_l *r, ysm_i *output_device, ysm_l *output_device_pt) {
  while (code[off] != 0) {
    output_device[*output_device_pt] = code[off];
    (*output_device_pt)++;
    off++;
  }
  output_device[*output_device_pt] = -1;
  *r = 0;
}

static void ds_puti(ysm_l e, ysm_l *r, ysm_i *output_device, ysm_l *output_device_pt) {
  if (e == 0) {
    output_device[*output_device_pt] = '0';
    (*output_device_pt)++;
    output_device[*output_device_pt] = -1;
  }
  else {
    ysm_l len = 0;
    ysm_l e_m = e;
    while (e_m != 0) {
      e_m /= 10;
      len++;
    }

    *output_device_pt += len;

    while (e != 0) {
      output_device[--(*output_device_pt)] = (e % 10) + '0';
      e /= 10;
    }

    *output_device_pt += len;
    output_device[*output_device_pt] = -1;
  }

  *r = 0;
}

static void ds_putchar(ysm_c e, ysm_l *r, ysm_i *output_device, ysm_l *output_device_pt) {
  output_device[*output_device_pt] = e;
  (*output_device_pt)++;
  output_device[*output_device_pt] = -1;
  *r = 0;
}

static void ds_putfmt(ysm_c *code, ysm_l off, ysm_l *stack, ysm_l *sp, ysm_l *r, ysm_i *output_device, ysm_l *output_device_pt) {
  while (code[off] != 0) {
    if (code[off] == '%') {
      switch (code[off + 1]) {
      case '%':
	ds_putchar('%', r, output_device, output_device_pt);
	if (*r) {
	  return;
	}
	break;

      case 'd':
	ds_puti(stack[--(*sp)], r, output_device, output_device_pt);
	if (*r) {
	  return;
	}
	break;

      case 'c':
	ds_putchar(stack[--(*sp)], r, output_device, output_device_pt);
	if (*r) {
	  return;
	}
	break;

      case 's':
	ds_putstr(code, stack[--(*sp)], r, output_device, output_device_pt);
	if (*r) {
	  return;
	}
	break;
      }
      off += 2;
    }
    else {
      ds_putchar(code[off], r, output_device, output_device_pt);
      if (*r) {
	return;
      }
      off++;
    }
  }
  *r = 0;
}

enum error open(ysm_l class) {
  switch (class) {
  case 0: /* input/output */
    break;

  default:
    return err_open;
  }

  return err_success;
}

enum error invoke(ysm_l class, ysm_l type, void *code, void *heap, ysm_ui heap_alignment, ysm_l *stack, ysm_l *reg, ysm_i *input_device, ysm_l *input_device_pt, ysm_i *output_device, ysm_l *output_device_pt) {
  TEMP_USE(heap);
  TEMP_USE(heap_alignment);
  TEMP_USE(input_device);
  TEMP_USE(input_device_pt);

  switch (class) {
  case 0: /* input/output */
    switch(type) {
    case 0: /* putstr */
      ds_putstr((char *)code, stack[--reg[reg_sp]], &reg[reg_gpr0], output_device, output_device_pt);
      break;

    case 1: /* puti */
      ds_puti(stack[--reg[reg_sp]], &reg[reg_gpr0], output_device, output_device_pt);
      break;

    case 2: /* putchar */
      ds_putchar(stack[--reg[reg_sp]], &reg[reg_gpr0], output_device, output_device_pt);
      break;

    case 3: /* putfmt */
      ds_putfmt(code, stack[--reg[reg_sp]], stack, &reg[reg_sp], &reg[reg_gpr0], output_device, output_device_pt);
      break;

    default:
      return err_invoke;
    }
    break;

  default:
    return err_invoke;
  }

  return err_success;
}
