/**
  ******************************************************************************
  * @file    DAC1class.cpp
  * @author  Susloparov A.A.
  * @version V1.0
  * @date    04.09.2012
  * @brief   DAC1 class @ DAC_OUT1=PA.4 pin /w no DMA
  ******************************************************************************
  */
     
extern void readAds8320(void);     

class clsDAC1 
{
public:
   
   uint16_t dacData;
   
   clsDAC1() //Конструктор класса
   {
      
     /* Тактирование порта вывода GPIOA */
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* Тактирование ЦАП */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    /* Конфигурация GPIOA */
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //Пин PA.4 для DAC1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

   
    /* Конфигурация ЦАП */
     
    DAC_DeInit(); //Сброс настроек на стандартные
    
    /*
    DAC_InitTypeDef  DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    */
  
    DAC_Cmd(DAC_Channel_1, ENABLE); //Включение ЦАП, канал 1
    
    dacData = 2000;
    setData(dacData);

   }
   void setData(uint16_t Data) //Послать данные на ЦАП (от 0 до 4095 *Vref)
   {    
     
      dacData = Data;
      DAC_SetChannel1Data(DAC_Align_12b_R, dacData);
    
   }
   
   void tuneDac(void)
   {
      char dac_string[64];
      
      timer3.stopTimer();
      
      int needDacTune = 1;
         
      if( dacData > 0xffe)
        dacData = 0xffe;
      setData(dacData);
      
      // Wait for transition to finish
      //vTaskDelay(delay200);
      
      sprintf(dac_string, "Offset tuning. Please wait ... \r\n");
      uart.transmitMessage(dac_string);
       
      while(needDacTune)
      {
         readAds8320();
         
         if(common.ads8320Data < 1000)
         {
            setData(dacData);
            if(dacData < 0x0ffe)
               dacData += 1;
            else
            {
               sprintf(dac_string, "DAC on the top rail!!!\r\n");
               uart.transmitMessage(dac_string);
               sprintf(dac_string, "DAC data = 0x%x\r\n", dacData);
               uart.transmitMessage(dac_string);
            }
         }
         else if(common.ads8320Data > 5000)
         {
            setData(dacData);
            if(dacData > 1)
               dacData -= 1;
            else
            { 
               sprintf(dac_string, "DAC on the bottom rail!!!\r\n");
               uart.transmitMessage(dac_string);
               sprintf(dac_string, "DAC data = 0x%x\r\n", dacData);
               uart.transmitMessage(dac_string);
            }
         }
         else // everything ok
            needDacTune = 0;
         
         if(dacData%30 == 0)
         {
            sprintf(dac_string, ">");
            //sprintf(dac_string, "dac = %d, dU = %d\r", dacData, common.ads8320Data);
            uart.transmitMessage(dac_string);
         }
         
         //vTaskDelay(delay30);
         vTaskDelay(delay5);
         
         
      }// end while
      
      sprintf(dac_string, "\r\nReady to work.\r\n", 
              dacData, common.ads8320Data);
      uart.transmitMessage(dac_string);
      
      timer3.startTimer();
   }
 
};