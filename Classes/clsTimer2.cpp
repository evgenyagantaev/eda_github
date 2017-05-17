// This class provides functionality of timer TIM2

class clsTimer2
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
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
      TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
      TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
      //60MHz/60 = 1MHz; *1000000=1s=1Hz
      TIM_TimeBaseStructure.TIM_Prescaler = 60;
      TIM_TimeBaseStructure.TIM_Period = period;   // 1000000 -> 1 second
      TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
      TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

      //TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
      TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

   }// end init_tim2

   
   //******************** end private functions ********************************
   
      
//******************************************************************************   
public://                    PUBLIC ZONE
//******************************************************************************      

   //---------------------------------------------------------------------------
   // public constructor
   clsTimer2()
   {
      // timer initialization:
      setPeriod(495); // start period (500 uSec, 2 KHz)
      started = 0;
      
   }//end clsTimer2
   //--------------------------end----------------------------------------------
   
   
   //************************ properties ***************************************
        
   //********************** end properties *************************************
   int started;
   //********************* public functions ************************************
   // function starts timer2 and enables timer 2 IRQs
   void startTimer(void)
   {
      TIM_Cmd(TIM2, ENABLE);
      NVIC_EnableIRQ(TIM2_IRQn);
      started = 1;
   }
   
   // function stops timer2 and disables timer 2 IRQs
   void stopTimer(void)
   {
      TIM_Cmd(TIM2, DISABLE);
      NVIC_DisableIRQ(TIM2_IRQn);
      started = 0;
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

void timer2_interrupt_service()
{
    uint32_t aux0 = 0;
    
    if(measure_on)
    {
        measurement_counter++;
        for(int i=0; i<17; i++)
        {
            readAd7691();
            aux0 += common.ad7691Data;
            //readAds8320();
            //aux0 += common.ads8320Data;
        }
        aux0 /= 17;
        common.ad7691Data = (uint32_t)aux0;
        //common.ads8320Data = (uint32_t)aux0;
        if(data_buffer_index < BUFFER_LENGTH)
        {
            data_buffer[data_buffer_index] = common.ad7691Data;
            //data_buffer[data_buffer_index] = common.ads8320Data;
            data_buffer_index++;
        }
        if(measurement_counter >= samples_in_one_pulse)
            // stop measure
            measure_on = 0;
    }
    
    /*
    if(measure_on)
    {
        uint32_t x0, x1, x2;
        
        measurement_counter++;
        //readAds8320();
        //x0 = common.ads8320Data;
        //readAds8320();
        //x1 = common.ads8320Data;
        //readAds8320();
        //x2 = common.ads8320Data;
        //common.ads8320Data = (uint16_t)((x0+x1+x2)/3);
        if(data_buffer_index < BUFFER_LENGTH)
        {
            data_buffer[data_buffer_index] = common.ads8320Data;
            data_buffer_index++;
        }
        else
            //printf("PEREPOLNENIE!!!!!!!!!!\r\n");
        
        if(measurement_counter >= 100)
            // stop measure
            measure_on = 0;
        
    }
    */
        
}

// timer 2 interrupt service procedure
extern "C" void TIM2_IRQHandler(void)
{
    // clear bit
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
     TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    timer2_interrupt_service();
    
    /*
    if(measure_on)
    {
        measurement_counter++;
        if(measurement_counter >= 100)
            // stop measure
            measure_on = 0;
    }
    */
      
}
