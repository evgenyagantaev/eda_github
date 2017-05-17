void ads8320SpiTask(void *parameters)
{
   uint16_t dacData = 0;
   
   while(1)
   {
      dac1.setData(dacData);
      dacData += 500;
      
      if(dacData > 4000)
         dacData = 0;
      
      // yield control to scheduler
      //taskYIELD();
      vTaskDelay(delay1000);
   }
   
}