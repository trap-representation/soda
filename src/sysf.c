#include <stdio.h>

#include "types.h"
#include "sysf.h"

void ds_putstr(ysm_c *code, ysm_l off, ysm_l *r) {
  while (code[off] != 0) {
    if (fputc(code[off], stdout) == EOF) {
      *r = 1;
      return;
    }
    off++;
  }
  *r = 0;
}

void ds_put(ysm_l e, ysm_l *r) {
  if (fprintf(stdout, "%" PRIYSML, e) < 0) {
    *r = 1;
    return;
  }
  *r = 0;
}

void ds_putchar(ysm_c e, ysm_l *r) {
  if (fputc(e, stdout) == EOF) {
    *r = 1;
    return;
  }
  *r = 0;
}

void ds_putfmt(ysm_c *code, ysm_l off, ysm_l *stack, ysm_l *sp, ysm_l *r) {
  while (code[off] != 0) {
    if (code[off] == '%') {
      switch (code[off + 1]) {
      case '%':
	ds_putchar('%', r);
	if (*r) {
	  return;
	}
	break;

      case 'd':
	ds_put(stack[--(*sp)], r);
	if (*r) {
	  return;
	}
	break;

      case 'c':
	ds_putchar(stack[--(*sp)], r);
	if (*r) {
	  return;
	}
	break;

      case 's':
	ds_putstr(code, stack[--(*sp)], r);
	if (*r) {
	  return;
	}
	break;
      }
      off += 2;
    }
    else {
      ds_putchar(code[off], r);
      if (*r) {
	return;
      }
      off++;
    }
  }
  *r = 0;
}
