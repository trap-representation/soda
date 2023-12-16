#ifndef TYPES_H
#define TYPES_H

typedef unsigned long ysm_nof_uns;
typedef int ysm_l;
typedef unsigned int ysm_ui;
typedef int ysm_i;
typedef unsigned short ysm_us;
typedef short ysm_s;
typedef unsigned char ysm_uc;
typedef char ysm_c;

#define SIZEOF_UC 1
#define SIZEOF_US sizeof(ysm_us)/sizeof(ysm_uc)
#define SIZEOF_UI sizeof(ysm_ui)/sizeof(ysm_uc)
#define SIZEOF_C 1
#define SIZEOF_S sizeof(ysm_s)/sizeof(ysm_uc)
#define SIZEOF_I sizeof(ysm_i)/sizeof(ysm_uc)
#define SIZEOF_L sizeof(ysm_l)/sizeof(ysm_uc)

#define ALIGNOF_UC 1
#define ALIGNOF_US _Alignof(ysm_us)/sizeof(ysm_uc)
#define ALIGNOF_UI _Alignof(ysm_ui)/sizeof(ysm_uc)
#define ALIGNOF_C 1
#define ALIGNOF_S _Alignof(ysm_s)/sizeof(ysm_uc)
#define ALIGNOF_I _Alignof(ysm_i)/sizeof(ysm_uc)
#define ALIGNOF_L _Alignof(ysm_l)/sizeof(ysm_uc)

#define YSML_MAX INT_MAX
#define YSML_MIN INT_MIN
#define YSMUI_MAX UINT_MAX
#define YSMI_MAX INT_MAX
#define YSMI_MIN INT_MIN
#define YSMUS_MAX USHORT_MAX
#define YSMS_MAX SHORT_MAX
#define YSMS_MIN SHORT_MIN
#define YSMUC_MAX UCHAR_MAX
#define YSMC_MAX CHAR_MAX
#define YSMC_MIN CHAR_MIN

#define PRIYSML "d"
#define PRIYSMUI "u"
#define PRIYSMxUI "x"
#define PRIYSMoUI "o"
#define PRIYSMI "d"
#define PRIYSMUS "hu"
#define PRIYSMxUS "hx"
#define PRIYSMoUS "ho"
#define PRIYSMS "hd"
#define PRIYSMUC "hhu"
#define PRIYSMxUC "hhx"
#define PRIYSMoUC "hho"
#define PRIYSMC "hhd"

#define SCNYSML "d"
#define SCNYSMUI "u"
#define SCNYSMxUI "x"
#define SCNYSMoUI "o"
#define SCNYSMI "d"
#define SCNYSMUS "hu"
#define SCNYSMxUS "hx"
#define SCNYSMoUS "ho"
#define SCNYSMS "hd"
#define SCNYSMUC "hhu"
#define SCNYSMxUC "hhx"
#define SCNYSMoUC "hho"
#define SCNYSMC "hhd"

#endif
