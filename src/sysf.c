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
