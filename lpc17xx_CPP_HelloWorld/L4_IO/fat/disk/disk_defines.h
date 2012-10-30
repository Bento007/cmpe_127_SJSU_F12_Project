#ifndef _DISK_DEFINES
#define _DISK_DEFINES
#ifdef __cplusplus
extern "C" {
#endif



/* Command code for disk_ioctrl() */
/* Generic command */
#define CTRL_SYNC           0   /* Mandatory for write functions */
#define GET_SECTOR_COUNT    1   /* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE     2
#define GET_BLOCK_SIZE      3   /* Mandatory for only f_mkfs() */
#define CTRL_POWER          4
#define CTRL_LOCK           5
#define CTRL_EJECT          6

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT      0x01    /* Drive not initialized */
#define STA_NODISK      0x02    /* No medium in the drive */
#define STA_PROTECT     0x04    /* Write protected */



#ifdef __cplusplus
}
#endif
#endif
