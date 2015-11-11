/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include "com.h"
#include "hal_SPI.h"
#include "hal_hardware_board.h"
#include "MMC.h"
#include "mmc.c"

static volatile
DSTATUS Stat = STA_NOINIT;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	int result;

  if (Stat & STA_NODISK) return Stat; // No SD card

  // Initialisation of the MMC/SD-card
  //TXString("disk initialize\r\n");
  unsigned int timeout = 0;

  // Initialisation of the MMC/SD-card
  while ((Stat & STA_NOINIT) == STA_NOINIT)
  {
    if (mmcInit() == 0) Stat &= ~STA_NOINIT; // Turn off NOINIT flag
    timeout++;
    if (timeout == 50)                     // Try 50 times till error
    {
      TXString("disk initialize timeout...\r\n");
      Stat |= STA_NOINIT;
    }
  }
  return Stat;

}


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;
	//int result;


  //TXString("disk_status...\r\n");
  stat = mmcPing();

  return stat;
	//return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DRESULT res;
  //TXString("disk_read...\r\n");
  //TXString("sector:\r\n");
  //TXInt(sector);
  //TXString("count:\r\n");
  //TXInt(count);
  
  res = mmcReadBlock((sector * 512), (count * 512), buff);
  //TXInt(res);
  //TXString("Done reading block\r\n");

  return res;

	//return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DRESULT res;
  //TXString("disk_write...\r\n");
  res = mmcWriteBlock((sector * 512), (count * 512), buff); 
  //TXInt(res);
  return res;
}

DWORD get_fattime () {
  //TXString("get_fattime...\r\n");
  return 0;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
  int chk;

  //TXString("disk_ioctl...\r\n");

  switch (cmd) {
  case CTRL_SYNC:
        //TXString("CTRL_SYNC\r\n");
        while ( chk != 0xFF ) chk = mmcCheckBusy();
        res = RES_OK;
        break;

  case GET_SECTOR_COUNT:
        TXString("GET_SECTOR_COUNT\r\n");
        res = RES_OK;
        break;
  case GET_SECTOR_SIZE:
        TXString("GET_SECTOR_SIZE\r\n");
        res = RES_OK;
        break;
  case GET_BLOCK_SIZE:
        TXString("GET_BLOCK_SIZE\r\n");
        res = RES_OK;
        break;
  case CTRL_ERASE_SECTOR:
        TXString("CTRL_ERASE_SECTOR\r\n");
        res = RES_OK;
        break;
  }

  return res;
}
#endif
