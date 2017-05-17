// This class provides functionality of timer TIM3

class clsTimer3
{
//******************************************************************************   
private://                   PRIVATE ZONE
//******************************************************************************   

   //************************* constants ***************************************
   //*********************** end constants *************************************   


   //************************* variables ***************************************
   int period;

   //*********************** end variables ************************************


   //********************* private functions **********************************
   void init_timer(void)
   {
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
      TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
      TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
      //60MHz/60 = 1MHz; *1000000=1s=1Hz
      TIM_TimeBaseStructure.TIM_Prescaler = 60000;
      TIM_TimeBaseStructure.TIM_Period = period;   // 1000000 -> 1 second
      TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
      TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

      //TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
      TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

      //TIM_Cmd(TIM3, ENABLE);
      //NVIC_EnableIRQ(TIM3_IRQn);
   }// end init_tim3

   
   //******************** end private functions ********************************
   
      
//******************************************************************************   
public://                    PUBLIC ZONE
//******************************************************************************      

   //---------------------------------------------------------------------------
   // public constructor
   clsTimer3()
   {
      
      // timer initialization:
      setPeriod(5000); // start period -> 5 sec
      init_timer();
   }//end clsTimer3
   //--------------------------end----------------------------------------------
   
   
   //************************ properties ***************************************
        
   //********************** end properties *************************************
   
   //********************* public functions ************************************
   // function starts timer3 and enables timer 3 IRQs
   void startTimer(void)
   {
      TIM_Cmd(TIM3, ENABLE);
      NVIC_EnableIRQ(TIM3_IRQn);
   }
   
   // function stops timer3 and disables timer 3 IRQs
   void stopTimer(void)
   {
      TIM_Cmd(TIM3, DISABLE);
      NVIC_DisableIRQ(TIM3_IRQn);
   }
   
   void setPeriod(int PERIOD)
   {
      period = PERIOD;
      init_timer();
   }
   //******************* end public functions **********************************
   
   
   //---------------------------------------------------------------------------
   // Method ...
   void someMethod(void)
   {
      
   }//end someMethod
   //--------------------------end----------------------------------------------
   
      
};

void timer3_interrupt_service()
{
    int i;
    double voltage = 0;
    float temperature;
    
    char voltage_message[21];
    char temperature_message[21];

    if(!measure_on)
    {
        // toggle led
        led.toggleLed();
        
        // get data from external adc
        for(i=0;i<100;i++)
        {
            readAds8320();
            voltage += (double)common.ads8320Data;
        }
        voltage /= 100.0;
        voltage = voltage/1000.0*31.0;
        // voltage data output
        //debug
        //printf("%d\r\n", common.ads8320Data);
        sprintf(voltage_message, "*%04d V", (int)voltage);
        for(i=0; i<21;i++)
            uart.transmitByte(voltage_message[i]);
        printf("%s\r\n", voltage_message);
        
        //get temperature
        temperature = thermometer.readThemperature(1);
        // temperature data output
        //debug
        //printf("%d\r\n", common.ads8320Data);
        sprintf(temperature_message, "*%04d C", (int)(temperature*100));
        printf("%s\r\n", temperature_message);
        //*/
    }
    
}

// timer 3 interrupt service procedure
extern "C" void TIM3_IRQHandler(void)
{
   //if(timer3IsrBypass)
   if(0)
   {
      // previous interrupt is not processed yet
      // we have to bypass an interrupt procedure
      // and set flag (abnormal isr)
      //timer3SeqInterrupted = 1;
   }
   else
   {
      // set flag, which signals that we started interrupt service procedure
      //timer3IsrBypass = 1;
      
     
      //portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE; 
      
      //printf("timer3\r\n");

      // clear bit
      if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
         TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
      
      // call function which services this interrupt
      //measure_voltage_temperature_flag = 1;
      // toggle led
      led.toggleLed();
      bluetooth_silence += 5;
      
      // give semaphore to the ISR handler task
      //xSemaphoreGiveFromISR(u_inMeasureTimerSemaphore, &xHigherPriorityTaskWoken);
      //GPIOB->BSRRH=GPIO_Pin_9;  //pb9 low
      
      // do the context switch, if necessary
      //portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
      
   }//end if(timer3IsrBypass)
}
