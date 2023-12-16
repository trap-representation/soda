#ifndef SYSF_H
#define SYSF_H

void ds_putstr(ysm_c *code, ysm_l off, ysm_l *r);
void ds_put(ysm_l e, ysm_l *r);
void ds_putchar(ysm_c e, ysm_l *r);
void ds_putfmt(ysm_c *code, ysm_l off, ysm_l *stack, ysm_l *sp, ysm_l *r);

#endif
