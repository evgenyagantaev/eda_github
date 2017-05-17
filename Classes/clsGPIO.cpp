// This class provides functionality of gpio control

class clsGPIO
{
//******************************************************************************   
private://                   PRIVATE ZONE
//******************************************************************************   

   //************************* constants ***************************************
   //static const uint16_t 
   //*********************** end constants *************************************   


   //************************* variables ***************************************


   //*********************** end variables *************************************


   //********************* private functions ***********************************


   //******************** end private functions ********************************
   
      
//******************************************************************************   
public://                    PUBLIC ZONE
//******************************************************************************      

   //---------------------------------------------------------------------------
   // public constructor
   clsGPIO()
   {
      // Enable the GPIOA clock
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
      // Enable the GPIOB clock
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
      // Enable the GPIOC clock 
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

      GPIO_InitTypeDef GPIO_InitStructure; 

      // configure pins pc0 - pc7; pins which control impulses
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;             
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(GPIOC, &GPIO_InitStructure);
      
      // ad7691
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;             
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(GPIOA, &GPIO_InitStructure);
      // ad7691
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;             
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(GPIOA, &GPIO_InitStructure);
      
      // reset pin pa1
      all_pc_low();
      
      
      
      
   }//end clsGPIO
   //--------------------------end----------------------------------------------
   
   
   //************************ properties ***************************************
        
   //********************** end properties *************************************
   
   //********************* public functions ************************************
   
   // Function resets bluetooth
   void bluetooth_reset()
   {
       // Reset pin
       GPIOB->BSRRH=GPIO_Pin_5;  //pb5 low
       //pause 100 mSec
       for(volatile int j=0; j<100; j++)
           for(volatile long i=0; i<13100; i++);
       
       // set bluetooth pin high
       GPIOB->BSRRL=GPIO_Pin_5;  //pb5 high
   }
   
   // Function sets pin PC1
   void high_pa1(void)
   {
      GPIOA->BSRRL=GPIO_Pin_1;  //pa1 high
   }//end high
   
   // Function resets pin PC1
   void low_pa1(void)
   {
      GPIOA->BSRRH=GPIO_Pin_1;  //pa1 low
   }//end low
   
   // Function resets pins PC0 - PC7
   void all_pc_low(void)
   {
      GPIOC->BSRRH=GPIO_Pin_0;
      GPIOC->BSRRH=GPIO_Pin_1;
      GPIOC->BSRRH=GPIO_Pin_2;
      GPIOC->BSRRH=GPIO_Pin_3;
      GPIOC->BSRRH=GPIO_Pin_4;
      GPIOC->BSRRH=GPIO_Pin_5;
      GPIOC->BSRRH=GPIO_Pin_6;
      GPIOC->BSRRH=GPIO_Pin_7;
   }//end low
   
   void high()
   {
        //high_pa1();
        GPIOC->BSRRL=common.resistors_gpios;
           
   }
   
   void low()
   {
       //low_pa1();
       GPIOC->BSRRH=common.resistors_gpios;
   }
   
  
   
   //******************* end public functions **********************************
   
   
   //---------------------------------------------------------------------------
   // Function ...
   void someFunction(void)
   {
      
   }//end someFunction
   //--------------------------end----------------------------------------------
   
      
};