void powerOffProcedure()
{
   
   const int numberOfIntervals = 100;
   long pause;
   pause = (powerController.powerOffInterval()/numberOfIntervals)*decrementTimeoutRatio;
   
   int buttonPressed = 1;
   
   led.redLedOn(); // power button pressed
   led.greenLedOff(); 
   
   volatile int i = 0;
   while((i<numberOfIntervals) && buttonPressed)
   {
      watchdog.reload();
      // pause ************************
      int x=5, y=6;
      volatile long timeoutCounter = pause;
      while((x != y) && (timeoutCounter > 0))
      {
         x = 5;
         timeoutCounter--;
      }
      //******************************
      
      if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)) 
      {
        //button is not pressed
         buttonPressed = 0;
      }
      
      i++;
   }
   
   if(buttonPressed && (i >= numberOfIntervals)) // power off
   {
      sdio.closeFile();
      uint32_t rd = (*(__IO uint32_t*) ADDR_FLASH_SECTOR_1);
      if(rd!=2) flash.writePowerOffOk();
      // blink twice with red and green leds
      for(int i=0; i<2; i++)
      {
         watchdog.reload();
         led.greenLedOn(); 
         led.redLedOff(); 
         for(volatile long i=0; i<3000000; i++);
         led.greenLedOff(); 
         led.redLedOn(); 
         for(volatile long i=0; i<3000000; i++);
      }
      
      led.redLedOff(); 
      led.greenLedOff(); 
      GPIO_ResetBits(GPIOB,GPIO_Pin_15);       //reset ON/OFF pin
      while(1);
   }
   else // early releise of button
   {
      led.greenLedOn(); 
      led.redLedOff(); 
   }
}


void powerControlTask(void *parameters)
{
   
   
   
   while(1)
   {
      if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)) 
      {
         //button is pressed
         // start power off procedure
         powerOffProcedure();
      }
      
      // yield control to scheduler
      vTaskDelay(delay500);
   }
}

void powerOnProcedure()
{
   //GPIO_SetBits(GPIOB,GPIO_Pin_15);            //set ON/OFF pin  
   GPIO_ResetBits(GPIOB,GPIO_Pin_14);          //reset Delta R pin
   
   const int numberOfIntervals = 100;
   long pause;
   pause = (powerController.powerOnInterval()/numberOfIntervals)*decrementTimeoutRatio;
   
   int buttonPressed = 1;
   
   int counter = 0;
   for(int i=0; i<numberOfIntervals; i++)
   {
      counter++;
      
      // pause ************************
      int x=5, y=6;
      volatile long timeoutCounter = pause;
      while((x != y) && (timeoutCounter > 0))
      {
         x = 5;
         timeoutCounter--;
      }
      //******************************
      
      if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)) 
      {
        //button is not pressed
         buttonPressed = 0;
      }
   }
   
   if(!buttonPressed)
   {
     GPIO_ResetBits(GPIOB,GPIO_Pin_15); 
   }
   else
   {

   GPIO_SetBits(GPIOB,GPIO_Pin_15);       //set ON/OFF pin
   
   // blink twice with red and green leds
   for(int i=0; i<2; i++)
   {
      led.greenLedOn(); 
      led.redLedOff(); 
      for(volatile long i=0; i<3000000; i++);
      led.greenLedOff(); 
      led.redLedOn(); 
      for(volatile long i=0; i<3000000; i++);
   }
   }
}