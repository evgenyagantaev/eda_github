// This class provides functionality of timer TIM4 /w ISR for acelerometer read task
class clsTimer4
{
public:
  clsTimer4()
  {
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 600;
    TIM_TimeBaseStructure.TIM_Period = 1000; //60Mhz/(600*1000)=100Hz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    //TIM_Cmd(TIM4, ENABLE);
    //NVIC_EnableIRQ(TIM4_IRQn);
  
  }
  void startTimer4(void)
   {
      TIM_Cmd(TIM4, ENABLE);
      NVIC_EnableIRQ(TIM4_IRQn);
   }
  
};

extern "C" void TIM4_IRQHandler(void) //timer 4 interrupt handler
{
    
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE; 
  
#ifdef DEBUG 
   //printf("timer4 it\n");
#endif 
   
   if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
  
   // give semaphore to the ISR handler task
   xSemaphoreGiveFromISR(accTimerSemaphore, &xHigherPriorityTaskWoken);
   
   // do the context switch, if necessery
   portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
   
}