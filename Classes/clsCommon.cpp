// this class is a container for a global variables, which are used for 
// intertask communications
class clsCommon
{
private:

public:
   
    //const int SAMPLINGFREQUENCY = 500;
   
    // debug ***********
    //const int SAMPLINGFREQUENCY = 250;
    
    int numberOfSamples;
   // cyclic buffer for ecg samples which feed a 50 Hz filter
   uint16_t filterBuffer[ADC1BUFFERLEN];
   // global variable "uint16_t filterBufferHead" is an index which points to the 
   // current position for writing a new sample in the buffer 
   uint16_t filterBufferHead;
   
   // cyclic buffer for ecg samples
   // here we collect filtered samples (a 50 Hz filter drops them here)
   uint16_t adc1Buffer[ADC1BUFFERLEN];
   // global variable "uint16_t adc1BufferHead" is an index which points to the 
   // current position for writing a new sample in the buffer 
   uint16_t adc1BufferHead;
   
   uint16_t outputDelay; //output delay in seconds, for task delay = outputDelay*delay1000
   
   // structure for accelerometer samples
   struct acceleration
   {
      int x;
      int y;
      int z;
   };
   // cyclic buffer for accelerometer samples
   acceleration accBuffer[ACCBUFFERLEN];
   // global variable "uint16_t adc1BufferHead" is an index which points to the 
   // current position for writing a new sample in the buffer 
   uint16_t accBufferHead;
   
   static const int INBUFFERLENGTH = 64; //recieve buffer length
   char uartRecieveBuffer[INBUFFERLENGTH];
   int uartRecieveBufferHead;
   
   uint8_t batteryCharge; //battery charge in percents
   
   bool commandMode,sendStatus,sendSamples,sendMarkers,sendAccel; //flags
   
   int needForCharge;   // signals that charge level is below 80%
   
   int heartRate;
   
   int length;
   float frequency;
   uint16_t current;
   
   int pulseDownCounter;
   int pulseOn;
   int pulseTim2Period;
   int samplesCounter;
   
   uint32_t ads8320Data;
   uint32_t ad7691Data;
   
   portTickType ads8320DelayBase;
   portTickType ads8320Delay;
   
   int accumConnected;
   
   int timer2Tick;
   
   int last_dU;
   int last_I;
   int i_setting;
   uint16_t resistors_gpios;
   
   int number_of_pulses;
   
   // public constructor
   clsCommon()
   {
      commandMode = false;
      
      sendStatus = true;
      
#ifdef SAMPLES
      sendSamples = true;
#else
      sendSamples = false;
#endif
     
#ifdef MARKERS
     sendMarkers = true;
#else
     sendMarkers = false;
#endif
     
#ifdef ACCEL
     sendAccel = true;
#else
     sendAccel = false;
#endif
      
      adc1BufferHead = 0;
      adc1Buffer[0] = 0;
      accBufferHead = 0;
      filterBufferHead = 0;
      uartRecieveBufferHead = 0;
      batteryCharge = 83;
      
      outputDelay=1;
      
      for(int i=0;i<ACCBUFFERLEN;i++) //init acelerometer common buffer
      {
        accBuffer[i].x = 0;
        accBuffer[i].y = 0;
        accBuffer[i].z = 0;
      }
      
      memset(&uartRecieveBuffer[0],'_',INBUFFERLENGTH - 1); //fill empty recieve buffer
      uartRecieveBuffer[INBUFFERLENGTH - 1] = 0;
      
     needForCharge = 0;
     
     heartRate = 444;
     
     length = 0;
     frequency = 0.0;
     current = 0;
     numberOfSamples = 25;
     
     pulseDownCounter = 0;
     pulseOn = 0;
     
     ads8320Data = 0;
     ad7691Data = 0;
   
     ads8320DelayBase = (portTickType)((configTICK_RATE_HZ/1000));
     ads8320Delay = 2;
     
     accumConnected = 0;
     
     samplesCounter = 0;
     
     timer2Tick = 0;
     
     last_dU = 0;
     last_I = 0;
     i_setting = 32;
     // transform gpios
    uint8_t I_raw = (uint8_t)(i_setting);
    uint8_t I_final = 0;
    // 0 - 5, 1 - 4, 2 - 6, 3 - 7, 4 - 0, 5 - 1, 6 - 2, 7 - 3

    I_final = I_raw >> 4;    // 4 - 0, 5 - 1, 6 - 2, 7 - 3
    I_final = I_final | ((I_raw & 0x01)<<5);      // 0 - 5
    I_final = I_final | (((I_raw>>1) & 0x01)<<4); // 1 - 4
    I_final = I_final | (((I_raw>>2) & 0x01)<<6); // 2 - 6
    I_final = I_final | (((I_raw>>3) & 0x01)<<7); // 3 - 7

    resistors_gpios = (uint16_t)I_final;
     
    number_of_pulses = 10;  // kolichestvo impulsov za vse vremya (zdes' 10 Hz * 1 sec = 10)
      
   }

};
