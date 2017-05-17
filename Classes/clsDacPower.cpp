// This class provides ...

class clsDacPower
{
//******************************************************************************   
private://                   PRIVATE ZONE
//******************************************************************************   

   //************************* constants ***************************************
   //static const uint16_t 
   //*********************** end constants *************************************   


   //************************* variables ***************************************
   uint16_t dacValue;

   //*********************** end variables *************************************


   //********************* private functions ***********************************


   //******************** end private functions ********************************
   
      
//******************************************************************************   
public://                    PUBLIC ZONE
//******************************************************************************      

   //---------------------------------------------------------------------------
   // public constructor
   clsDacPower()
   {
      dacValue = 0;
   }//end clsDacPower
   //--------------------------end----------------------------------------------
   
   
   //************************ properties ***************************************
        
   //********************** end properties *************************************
   
   //********************* public functions ************************************
   
   // Function sets dac value
   void setDacValue(uint16_t value)
   {
      uint16_t localValue;
      if(value > 255)
         localValue = 255;
      else
         localValue = value;
      
      //dacValue = localValue << 2;
      dacValue = localValue;
   }//end setDacValue
   
   // Function sets current on
   void onCurrent(void)
   {
      GPIOC->BSRRL = dacValue;
   }//end onCurrent
   
   // Function sets current off
   void offCurrent(void)
   {
      GPIOC->BSRRH = dacValue;
   }//end onCurrent
   
   // Function starts pulse
   void startPulse(void)
   {
      timer3.stopTimer();
      
      if(common.length == 0)
         common.pulseDownCounter = 2;  // We want only one pulse
      else
         common.pulseDownCounter = (int)((float)common.length * common.frequency * 2.0);
      
      // calculate number of samples in semyperiod
      common.numberOfSamples = common.SAMPLINGFREQUENCY/common.frequency;
      
      if((common.frequency == 65) || (common.frequency == 75) || (common.frequency == 85))
         common.pulseTim2Period = 1000;
      else
      {
         if((common.frequency > 0) && (common.frequency <= 40))
            common.pulseTim2Period = 980;
         else if(((common.frequency > 40) && (common.frequency <= 45)) || (common.frequency == 55))
            common.pulseTim2Period = 950;
         else if((common.frequency > 45) && (common.frequency <= 60))
            common.pulseTim2Period = 890;
         else if((common.frequency > 60) && (common.frequency <= 90))
            common.pulseTim2Period = 875;
         else if((common.frequency > 90) && (common.frequency <= 95))
            common.pulseTim2Period = 840;
         else if((common.frequency > 95) && (common.frequency <= 100))
            common.pulseTim2Period = 800;
      }
   
      // debug ************
      common.pulseTim2Period = 1000;
         
      timer2.setPeriod(common.pulseTim2Period);
      
      // give semaphore to adc1 task
      // and there we will start timer2
      xSemaphoreGive(u_inMeasureTimerSemaphore);
      
   }//end startPulse
   
   void stopPulse(void)
   {
      common.pulseDownCounter = 0;
      common.pulseOn = 0;
      
      timer2.stopTimer();
      timer3.startTimer();
      
      offCurrent();
      
   }//end stopPulse
   
   //******************* end public functions **********************************
   
   
   //---------------------------------------------------------------------------
   // Function ...
   void someFunction(void)
   {
      
   }//end someFunction
   //--------------------------end----------------------------------------------
   
      
};