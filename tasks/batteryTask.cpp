//This task provides battery charge read via ADC1

void batteryTask(void *pvParameters)
{
  
  
  while (1)
  {    
    
    common.batteryCharge = adc1.getBatteryVoltagePercent();
     
     // debug
     //common.batteryCharge = 83;
  
  //debug output----------------------------  
 /*         char buffer[32];
    
      sprintf(buffer, "charge %d interval %d\r\n",common.batteryCharge,diagnost.getInterval());
              

     xSemaphoreTake(uartMutex, portMAX_DELAY);
        uart.sendMessage(buffer);
      xSemaphoreGive(uartMutex);
    
    vTaskDelay(diagnost.getInterval()*delay1000);
    */
  //------------------------------------------
    
    vTaskDelay(30*delay1000); //delay 30s
    
  }
}