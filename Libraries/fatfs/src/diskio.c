/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2012        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include <stdint.h>
#include "stm32_sdio_common.h"
#include "stm32_sdio_sd.h"
/* Definitions of physical drive number for each media */

#define SDIOTIMEOUT 0xFFFF;
DWORD systemTime = ((2013UL-1980) << 25)       // Year = 2013
      | (2UL << 21)       // Month = Feb
      | (12UL << 16)       // Day = 12
      | (10U << 11)       // Hour = 10
      | (30U << 5)       // Min = 30
      | (0U >> 1)       // Sec = 0
      ;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
  return 0;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
  if (drv) return STA_NOINIT;		/* Supports only single drive */
  return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
    // Read the data from the Card  
    //printf("---rd sect %d count %d\r\n",sector,count);

  watchdog.reload();
    memset(buff, 0xFF, 512 * count);  
  /*
    if(count>1)
    {
    */  
      if( SD_ReadMultiBlocks((BYTE*)buff, sector*512, 512, count) != SD_OK ) 
          return RES_ERROR;
      
      if( SD_WaitReadOperation() != SD_OK ) 
          return RES_ERROR;
/*
    }
    else
    {
      
      if( SD_ReadBlock((BYTE*)buff, sector*512, 512) != SD_OK ) 
          return RES_ERROR;
      
      if( SD_WaitReadOperation() != SD_OK ) 
          return RES_ERROR;

    }
  */  
#ifdef NOSDIOTIMEOUT
   
   while(SD_GetStatus() != SD_TRANSFER_OK);
   
#else
   uint32_t timeout = SDIOTIMEOUT;
   
   while( (SD_GetStatus() != SD_TRANSFER_OK) && (timeout>0) )
   {
     timeout--;
   }
   
   if(timeout==0) return RES_ERROR;
   
#endif
    
   return RES_OK; 
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
    // Write the data from the Card 
  /*
printf("---wr sect %d count %d\r\n",sector,count);
for(int i=0;i<10;i++)
{
printf("%c",buff[i]);
}
printf("\r\n");
*/
    watchdog.reload();
 /*  if(count>1)
    { 
   */   
      
    if( SD_WriteMultiBlocks((uint8_t*)buff, sector*512, 512, count) != SD_OK ) 
          return RES_ERROR;

    
    if( SD_WaitWriteOperation() != SD_OK ) 
          return RES_ERROR; 

      
    /*  for(int i=0;i<count;i++)
      {
        if( SD_WriteBlock((uint8_t*)buff, sector*512*(i+1), 512) != SD_OK ) 
          return RES_ERROR;

    
        if( SD_WaitWriteOperation() != SD_OK ) 
          return RES_ERROR;
        
      }
      */
      /*
    }
   else
   {
     
    if( SD_WriteBlock((uint8_t*)buff, sector*512, 512) != SD_OK ) 
          return RES_ERROR;

    
    if( SD_WaitWriteOperation() != SD_OK ) 
          return RES_ERROR;

   }
   */
#ifdef NOSDIOTIMEOUT
   
   while(SD_GetStatus() != SD_TRANSFER_OK);
   
#else
   uint32_t timeout = SDIOTIMEOUT;
   
   while( (SD_GetStatus() != SD_TRANSFER_OK) && (timeout>0) )
   {
     timeout--;
   }
   
   if(timeout==0) return RES_ERROR;
   
#endif
    
   return RES_OK;  
    
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT res = RES_OK;
  switch (ctrl) {
    
    
  case GET_SECTOR_COUNT :	  // Get number of sectors on the disk (DWORD)
    *(DWORD*)buff = 15652864;	// 
    res = RES_OK;
    break;
    
  case GET_SECTOR_SIZE :	  // Get R/W sector size (WORD) 
    *(WORD*)buff = 512;
    res = RES_OK;
    break;
    
  case GET_BLOCK_SIZE :	    // Get erase block size in unit of sector (DWORD)
    *(DWORD*)buff = 1;
    res = RES_OK;
  }
  
  return res;

}
#endif

/*-----------------------------------------------------------------------*/
/* Get current time                                                      */
/*-----------------------------------------------------------------------*/

DWORD get_fattime ()
{
  return systemTime;
}

void set_fattime (DWORD t)
{
  systemTime = t;
}