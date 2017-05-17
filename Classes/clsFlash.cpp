//This class provides flash operations for power-off and iap processing
#include "flash_if.h"
#include "flash_if.c"

class clsFlash
{
public:
  clsFlash()
  {
    FLASH_If_Init();
  }
  /*
          0 - power off ok
          1 - power off err
          2 - iap
  */
  void writePowerOffOk()
  {
            FLASH_If_Erase_Sector(FLASH_Sector_1); //erase sector before writing
            
            uint32_t addr = ADDR_FLASH_SECTOR_1, //address to write
            wrval = 0, //value to write
            cnt = 1; //count of units to write
            
            FLASH_If_Write(&addr, &wrval ,(uint16_t)cnt); //write to flash
  }
  void writePowerOffErr()
  {
            FLASH_If_Erase_Sector(FLASH_Sector_1); //erase sector before writing
            
            uint32_t addr = ADDR_FLASH_SECTOR_1, //address to write
            wrval = 1, //value to write
            cnt = 1; //count of units to write
            
            FLASH_If_Write(&addr, &wrval ,(uint16_t)cnt); //write to flash
  }
  void writeIap()
  {
    
            FLASH_If_Erase_Sector(FLASH_Sector_1); //erase sector before writing
            
            uint32_t addr = ADDR_FLASH_SECTOR_1, //address to write
            wrval = 2, //value to write
            cnt = 1; //count of units to write
            
            FLASH_If_Write(&addr, &wrval ,(uint16_t)cnt); //write to flash
  }
  
};