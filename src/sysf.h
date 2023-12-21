#ifndef SYSF_H
#define SYSF_H

#include "types.h"
#include "errors.h"

enum error invoke(ysm_l class, ysm_l type, void *code, void *heap, ysm_ui heap_alignment, ysm_l *stack, ysm_l *reg, ysm_i *input_device, ysm_l *input_device_pt, ysm_i *output_device, ysm_l *output_device_pt);
enum error open(ysm_l class);

#endif
