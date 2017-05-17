// This class provides functionality for ecg analysis

class clsEcgAnalizer
{
private:

   //************************* constants ***************************************
   static const uint16_t SAMPLINGFREQUENCY = 242;
   //static const double SAMPLINGINTERVAL = 4.13; //(miliseconds)
   static const uint16_t ONEMINUTE = 60000; //(miliseconds)
   // threshold values for r-pick detection
   static const uint16_t AMPLITUDELEASTTHRESHOLD = 35;				
   static const uint16_t AMPLITUDEMOSTTHRESHOLD = 135;
   static const uint16_t RLEASTLENGTH = 2;
   static const uint16_t RMOSTLENGTH = 7;
   // length of local window (piece of ecg sample buffer)
   static const uint16_t QRSWINDOWLENGTH = 15;
   // constants definitions for array of markers
   static const uint16_t EMPTYMARKER = 0;
   static const uint16_t WHITEMARKER = 1;
   static const uint16_t REDMARKER = 2;
   static const uint16_t BLUEMARKER = 3;
   // heart rate buffers length
   static const uint16_t REGULARANDQUARANTINEBUFFERLENGTH = 5;
   // maximum appropriate deviation of heart rate from mean value of buffer
   // (percents)
   static const uint16_t MAXHEARTRATEDEVIATION = 10;
   static const uint16_t QRSSUSPENDPERIOD = 60;
   //*********************** end constants *************************************   
   
   
   //************************* variables ***************************************
   double SAMPLINGINTERVAL; //(miliseconds)
   // heart rate buffer
    int regularHeartRateBuffer[REGULARANDQUARANTINEBUFFERLENGTH];
    // mean of heart rate buffer
    int regularAverage;
    // quarantine heart rate buffer
    int quarantineHeartRateBuffer[REGULARANDQUARANTINEBUFFERLENGTH];
    // mean quarantine heart rate buffer
    int quarantineAverage;
    //local qrs window (here we will detect an r-pick presence)
    uint16_t qrsWindow[QRSWINDOWLENGTH];
    // local window of isoline
    uint16_t isolineWindow[QRSWINDOWLENGTH];
    // array of markers; just for debug purposes; here we mark start of r-pick,
    // finish of r-pick, local maximum e.c.t.
    uint16_t marks[QRSWINDOWLENGTH];
    
    // bazovyi uroven' izolinii, pri kotorom proizoshlo prevyshenie poroga
    // pri detektirovanii R-zubtca
    // my zamorazhivaem etot uroven', kak tol'ko detektirovali prevyshenie
    uint16_t baseIsoline;
    // index of r-pick beginning
    int rStart;
    // index of r-pick ending
    int rFinish;
    // index of r-pick local maximum
    int rMaxIndex;
    // index of the last processed sample in cyclic samples buffer
    uint16_t cyclicBufferIndex;
    
    // marker of the newest detected r-pick
    int rMarker;
    // marker of the old detected r-pick
    int oldRMarker;
    // rr-interval (in samples; interval beetween )
    
    // flag that indicates, is local buffer ready for analysis or not
    int localBufferReady;
    // local buffer counter; counts number of samples in local buffer
    int bufferCounter;
    
    int heartRate;
    // shift (in samples) between markers (sample-to-sample interval is 4.13 ms)
    int markerShift;
    
    // flag
    int newRegularHeartRateReady;
    
    // asystoly shift counter
    int asystolyShiftCounter;
    
    //*********************** end variables ************************************
    
    
    //********************* private functions **********************************
    
   //---------------------------------------------------------------------------
   // Method calculates average of array
   // gets array pointer, array length, returns mean value
   int calculateAverage(int *array, int arrayLength)
   {
      int average = 0;
      for(int i=0; i<arrayLength; i++)
         average += array[i];

      return average/arrayLength;
   }// end calculateAverage
   //--------------------------end----------------------------------------------
    
   //---------------------------------------------------------------------------
   // Method calculates rr-interval and heart rate and manipulates with 
   // heart rate buffers
   void heartRateCalculate(void)
   {
      
      // calculate interval between markers (in samples)
      if(oldRMarker == -1000) // first rMarker
         markerShift = 245;
      else if(rMarker >= oldRMarker)
         markerShift = rMarker - oldRMarker;
      else
         markerShift = ADC1BUFFERLEN - oldRMarker + rMarker;

      // calculate heartRate
      heartRate = (uint16_t)(ONEMINUTE/markerShift/SAMPLINGINTERVAL);

      // look up what about to insert this heart rate in regular buffer
      if((abs(heartRate - regularAverage)*100/regularAverage) < MAXHEARTRATEDEVIATION)
      {
         //shift regular and quarantine buffers
         for(int i=0; i<(REGULARANDQUARANTINEBUFFERLENGTH - 1); i++)
         {
            regularHeartRateBuffer[i] = regularHeartRateBuffer[i+1];
            quarantineHeartRateBuffer[i] = quarantineHeartRateBuffer[i+1];
         }
         // add value in regular and quarantine buffers
         //debug
         if(heartRate == 0)
         {
            printf("debug stop = 0\r\n");
            //debug stop
            while(1);
         }
         regularHeartRateBuffer[REGULARANDQUARANTINEBUFFERLENGTH - 1] = heartRate;
         quarantineHeartRateBuffer[REGULARANDQUARANTINEBUFFERLENGTH - 1] = heartRate;
         
         // set flag for rr interval collection procedure
         newRegularHeartRateReady = 1;

         // calculate average of regular buffer
         regularAverage = calculateAverage(regularHeartRateBuffer, REGULARANDQUARANTINEBUFFERLENGTH);
         if(common.heartRate == 444) // there was not real pulse yet
         {
            if((regularAverage < 130) && (regularAverage > 40)) // if normal start pulse
               common.heartRate = regularAverage;
         }
         else
            common.heartRate = regularAverage;
         
         // calculate average of quarantine buffer
         quarantineAverage = calculateAverage(quarantineHeartRateBuffer, REGULARANDQUARANTINEBUFFERLENGTH);
      }//end if((abs(heartRate - regularAverage)*100/regularAverage) < MAXHEARTRATEDEVIATION)
      else // here is an excess of permissible deviation from average
      {
         // so we add value in quarantine buffer *******************************

         // shift quarantine buffer
         for(int i=0; i<(REGULARANDQUARANTINEBUFFERLENGTH - 1); i++)
         {
            quarantineHeartRateBuffer[i] = quarantineHeartRateBuffer[i+1];
         }
         // add value
         quarantineHeartRateBuffer[REGULARANDQUARANTINEBUFFERLENGTH - 1] = heartRate;
         // calculate average of quarantine buffer
         quarantineAverage = calculateAverage(quarantineHeartRateBuffer, REGULARANDQUARANTINEBUFFERLENGTH);

         // check if we have to open quarantine ********************************

         // check the deviation of quarantine buffer content from a quarantine mean
         int deviationOk = 1;
         int i = 0;
         while (deviationOk && (i < REGULARANDQUARANTINEBUFFERLENGTH))
         {
            if (abs((quarantineHeartRateBuffer[i] - quarantineAverage) * 100 / quarantineAverage) > MAXHEARTRATEDEVIATION)
                   deviationOk = 0;
            else
                   i++;
         } 

         // if deviation ok, open quarantine (copy quarantine buffer in 
         // regular buffer)
         if (deviationOk)
         {
            for (int i = 0; i < REGULARANDQUARANTINEBUFFERLENGTH; i++)
             regularHeartRateBuffer[i] = quarantineHeartRateBuffer[i];
            regularAverage = quarantineAverage;
         }
      }// end else


   }// end heartRateCalculate
   //--------------------------end----------------------------------------------

   //****************** end private functions **********************************
   
public:

   //---------------------------------------------------------------------------
   // public constructor
   clsEcgAnalizer()
   {
      SAMPLINGINTERVAL = 4.13;
      oldRMarker = rMarker = -1000;
      qrsSuspend = 0;
      
      // initialize local window buffers
      for(int i=0; i<QRSWINDOWLENGTH; i++)
      {
         qrsWindow[i] = isolineWindow[i] = 0;
         marks[i] = EMPTYMARKER;
      }
      
      // initialize heart rate buffers
      for(int i=0; i<REGULARANDQUARANTINEBUFFERLENGTH; i++)
      {
         regularHeartRateBuffer[i] = 333;
         quarantineHeartRateBuffer[i] = 333;
      }
      // initialize mean values of heart rate buffers
      regularAverage = 333;
      quarantineAverage = 333;
      
      //initialize flags
      localBufferReady = 0;
      
      //initialize counters
      bufferCounter = 0;
      
      // reset flag
      newRegularHeartRateReady = 0;
      
      asystolyShiftCounter = 0;
      
   }//end clsEcgAnalizer
   //--------------------------end----------------------------------------------
   
   
   //************************ properties ***************************************
   
   uint16_t oldSample(){return qrsWindow[0];}
   uint16_t oldIsoline(){return isolineWindow[0];}
   uint16_t marker(){return marks[0];}
   
   void asystolyShiftCounterInc(void){ asystolyShiftCounter++; }
   
   // returns current heart rate (average of regular buffer)
   int getHeartRate()
   {
      //debug
      /*
      if(regularAverage > 2000)
      {
         for(int i=0; i<REGULARANDQUARANTINEBUFFERLENGTH; i++)
         {
            printf("%d\r\n", regularHeartRateBuffer[i]);
         }
         
         //debug stop
         while(1);
      }
      */
      //debug *****
      
      return regularAverage;
   }
   
   uint16_t getSamplingFrequency()
   {return SAMPLINGFREQUENCY;}
   
   int getNewRegularHeartRateReady(void)
   {   return newRegularHeartRateReady; }
   
   void resetNewRegularHeartRateReady(void)
   {   newRegularHeartRateReady = 0; }
   
   int getLastRR(void)
   {
      int heartRate = (int)regularHeartRateBuffer[REGULARANDQUARANTINEBUFFERLENGTH - 1];
      int RR = ONEMINUTE/heartRate;
      
      return RR;
   }
   //********************** end properties *************************************
   
   //********************* public functions ************************************
   
   //---------------------------------------------------------------------------
   // Method adds sample in qrs window and isoline value in isoline window
   void addSample(uint16_t sample, uint16_t isoline, uint16_t index)
   {
      // shift buffers
      for(int i=0;i<QRSWINDOWLENGTH-1;i++)
      {
         qrsWindow[i] = qrsWindow[i+1];
         isolineWindow[i] = isolineWindow[i+1];
         //debug
         marks[i] = marks[i+1];
      }
      // add values
      qrsWindow[QRSWINDOWLENGTH-1] = sample;
      isolineWindow[QRSWINDOWLENGTH-1] = isoline;
      cyclicBufferIndex = index;
      //debug
      marks[QRSWINDOWLENGTH-1] = EMPTYMARKER;
      
      // check if local buffer (full) ready for analysis
      if(!localBufferReady)
      {
         // buffer is not ready, so we have not collected enough samples in it,
         // so we increment samples counter
         bufferCounter++;
         // if buffer is full, set flag of buffer readiness
         if(bufferCounter >= QRSWINDOWLENGTH)
            localBufferReady = 1;
      }
   }//end addSample
   //--------------------------end----------------------------------------------
   
   
   
   
   
   
   
   //---------------------------------------------------------------------------
   // Method detects presence of r-pick in the local qrs-window
   void qrsDetect(void)
   {
      if(qrsSuspend) // procedure of qrs detection is suspended (just have found
      // a new r-pick)
      {
         if((common.samplesTimer - qrsSuspendMarker) > QRSSUSPENDPERIOD)
         {
            //suspend period gone yet; so we reset the suspend flag
            qrsSuspend = 0;
            GPIOB->BSRRH=GPIO_Pin_9;  //pb9 low
         }
      }
      else // qrs detection is not suspended
      {
         // first: we detect if there is an excess of threshold level in the window
         int excess = 0;
         int i = 0;
         while(!excess && (i<(QRSWINDOWLENGTH-7)))
         {
            if((qrsWindow[i] - isolineWindow[i]) > AMPLITUDELEASTTHRESHOLD)
            {
               excess = 1;
               baseIsoline = isolineWindow[i];
               rStart = i;
            }
            else
               i++;
         }
         
         // if no excess of threshold level in the local window, we have to drop
         // buffer and collect it again
         if(!excess)
         {
            localBufferReady = 0;
            bufferCounter = 0;
            
            // if no qrs more than 4 seconds, then we detect asistoly
            if(asystolyShiftCounter >= 1000)
            {
               if(common.heartRate != 444) // there was real pulse yet
                  common.heartRate = 0;
            }
         }
         
         // second: we detect if there is a drop under isoline level
         int drop = 0;
         if(excess)
         {
            while(!drop && (i<QRSWINDOWLENGTH))
            {
               if(qrsWindow[i] < baseIsoline)
               {
                  drop = 1;
                  rFinish = i;
               }
               else
                  i++;
            }
         }// end if(excess)
         
         // third: we detect if the r-pick length is ok
         int lengthOk = 0;
         if(drop)
         {
            int length = rFinish - rStart;
            if((length <= RMOSTLENGTH) && (length >= RLEASTLENGTH))
               lengthOk = 1;
         }
         
         // fourth: we check amplitude
         int amplitudeOk = 0;
         if(lengthOk)
         {
            int amplitude = qrsWindow[rStart];
            rMaxIndex = rStart;
            for(int i=(rStart+1); i<rFinish; i++)
            {
               if(qrsWindow[i] > amplitude)
               {
                  amplitude = qrsWindow[i];
                  rMaxIndex = i;
               }
            }
            amplitude = amplitude - baseIsoline;
            if(amplitude <= AMPLITUDEMOSTTHRESHOLD)
               amplitudeOk = 1;
         }// end if(lengthOk)
         
         // now if amplitude OK, so we probably detected good r-pick
         if(amplitudeOk)
         {
   #ifdef DEBUG
            // set markers in array of markers
            //marks[rStart] = WHITEMARKER;
            //marks[rFinish] = BLUEMARKER;
            marks[rMaxIndex] = REDMARKER;
   #endif
            
            // set marker of freshly detected r-pick
            oldRMarker = rMarker;
            rMarker = cyclicBufferIndex - (QRSWINDOWLENGTH - 1 - rMaxIndex);
            if(rMarker < 0)
               rMarker += ADC1BUFFERLEN; 
            
            // reset of asystoly counter
            asystolyShiftCounter = 0;
            
            // drop buffer
            localBufferReady = 0;
            bufferCounter = 0;
            
            // calculate rr-interval and heart rate
            heartRateCalculate();
            // now suspend a qrs detection procedure for a while
            qrsSuspendMarker = common.samplesTimer;
            qrsSuspend = 1;
            GPIOB->BSRRL=GPIO_Pin_9;  //pb9 high
            
         }//end if(amplitudeOk)
         
      }//end if(qrsSuspend)
      
   }//end qrsDetect
   //--------------------------end----------------------------------------------

   //****************** end public functions ***********************************
   
   
   //---------------------------------------------------------------------------
   // Method 
   
   //--------------------------end----------------------------------------------
   
      
};