/**
  ******************************************************************************
  * @file    ADC1class.cpp
  * @author  Susloparov A.A.
  * @version V1.0
  * @date    04.09.2012
  * @brief   ADC1 class @ PA.6 pin /w TIM3 @ 250Hz Trigger /w no DMA
  *          TIM3 TRGO Event -> ADC1 -> ADC Interrupt -> memory
  ******************************************************************************
  */

class clsADC1 
{ 
public:
 
   clsADC1() //Construct
   {
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;
    //DMA_InitTypeDef       DMA_InitStructure;
    
    //Fill empty buffer
    for(int i=0;i<ADC1BUFFERLEN;i++)
    {
      common.adc1Buffer[i] = 0;
    }
    
  /* ADC1, TIM3, DMA2 and GPIOA clock enable **********************************/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  //  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    
  /* ADC1 Channel1 configuration***********************************************/
    // ADC1 Channel1 configuration
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2 pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   // ADC_DeInit();
    
  /* ADC common init **********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure); 
  
  /* ADC1 init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //no trigger
    //ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; //Trigger select, rising edge
    //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; //Trigger select
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
  /* ADC1 regular channel1 configuration **************************************/
    
    /* ADC1 regular channel1 configuration **************************************/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_480Cycles); //ADC1 regular channel config
    
    /* ADC2 init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //no trigger
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; //Trigger select
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC2, &ADC_InitStructure);
    
  /* ADC2 regular channel2 configuration **************************************/
    
    /* ADC2 regular channel2 configuration **************************************/
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 1, ADC_SampleTime_480Cycles); //ADC2 regular channel config
    
    /* ADC3 init ****************************************************************/
    //ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    //ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    //ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    //ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //no trigger
    //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; //Trigger select
    //ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    //ADC_InitStructure.ADC_NbrOfConversion = 1;
    //ADC_InitStructure.ADC_NbrOfConversion = 2; // будем когда-нибудь пробовать подключать 2 и больше каналов к одному ацп
   // ADC_Init(ADC3, &ADC_InitStructure);
    
  /* ADC3 regular channel3 configuration **************************************/
    
    /* ADC3 regular channel3 configuration **************************************/
    //ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 1, ADC_SampleTime_480Cycles); //ADC3 regular channel config
    //ADC_RegularChannelConfig(ADC3, ADC_Channel_4, 1, ADC_SampleTime_480Cycles); //ADC3 regular channel config
    
    //ADC_EOCOnEachRegularChannelCmd(ADC3, ENABLE);
    
  /* DMA2 Stream0 channel0 init ***********************************************/
  /*
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&adc1Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = ADC1BUFFERLEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;y
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE); 
  */
  /* DMA request enable (Single-ADC mode) */
  //  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  /* ADC3 DMA enable */
  //  ADC_DMACmd(ADC1, ENABLE);
    
    //NVIC_InitTypeDef NVIC_InitStructure;
 
    /* ADC1 interrupt config  */
    //NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
    
    
    //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); //Enable ADC1 End Of Conversion interrupt
    
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    /* Enable ADC2 */
    ADC_Cmd(ADC2, ENABLE);
    /* Enable ADC3 */
    //ADC_Cmd(ADC3, ENABLE);
 
   }// end constructor
};

extern "C" void ADC_IRQHandler(void) //ADC interrupt handler
{
  
  if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) //End of conversion ADC1
  { 
    
    common.adc1Buffer[0] = ADC_GetConversionValue(ADC1);
    common.adc1BufferHead = 1;
    
    ADC_SoftwareStartConv(ADC1);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    common.adc1Buffer[0] += ADC_GetConversionValue(ADC1);
    ADC_SoftwareStartConv(ADC1);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    common.adc1Buffer[0] += ADC_GetConversionValue(ADC1);
    common.adc1Buffer[0] /= 3;
    
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
  
    
    
  }
  

}

