typedef unsigned long ysm_uns;
typedef long ysm_l;
typedef unsigned int ysm_ui;
typedef int ysm_i;
typedef unsigned short ysm_us;
typedef short ysm_s;
typedef unsigned char ysm_uc;
typedef char ysm_c;

#define SIZEOF_UNS sizeof(ysm_uns)/sizeof(ysm_uc)
#define SIZEOF_UC 1
#define SIZEOF_US sizeof(ysm_us)/sizeof(ysm_uc)
#define SIZEOF_UI sizeof(ysm_ui)/sizeof(ysm_uc)
#define SIZEOF_C 1
#define SIZEOF_S sizeof(ysm_s)/sizeof(ysm_uc)
#define SIZEOF_I sizeof(ysm_i)/sizeof(ysm_uc)
#define SIZEOF_L sizeof(ysm_l)/sizeof(ysm_uc)

#define ALIGNOF_UNS _Alignof(ysm_uns)/sizeof(ysm_uc)
#define ALIGNOF_UC 1
#define ALIGNOF_US _Alignof(ysm_us)/sizeof(ysm_uc)
#define ALIGNOF_UI _Alignof(ysm_ui)/sizeof(ysm_uc)
#define ALIGNOF_C 1
#define ALIGNOF_S _Alignof(ysm_s)/sizeof(ysm_uc)
#define ALIGNOF_I _Alignof(ysm_i)/sizeof(ysm_uc)
#define ALIGNOF_L _Alignof(ysm_l)/sizeof(ysm_uc)
