#include "main.h"

class clsLED //LED class @ PD.4&PD.5 pin
{
public:
   
   clsLED() //Construct
   {

       GPIO_InitTypeDef  GPIO_InitStructure;

       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //         
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
       GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
       GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //no pull resistor
       GPIO_Init(GPIOB, &GPIO_InitStructure);
   }
   
   void LED_On(void) //on/off led,
   {
    oneLedOn();
   }
   void LED_Off(void)
   {
    oneLedOff();
   }
   
   void oneLedOn(void)
   {
     GPIOB->BSRRL = GPIO_Pin_12;
   }
   void oneLedOff(void)
   {
     GPIOB->BSRRH = GPIO_Pin_12;
   }
   
   void toggleLed()
   {
       static int led_is_on = 0;
       if(led_is_on) // led is on
       {
           // turn led off
           GPIOB->BSRRH = GPIO_Pin_12;
           led_is_on = 0;
       }
       else // led is off
       {
           // turn led on
           GPIOB->BSRRL = GPIO_Pin_12;
           led_is_on = 1;
       }
   }
};