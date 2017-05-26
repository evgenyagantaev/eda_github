// This main file for 2v version of generator
#define DEBUG                   // includes debug activities in the compiler job




#include "main.h"

double abs(double value);

//--------------------------- macro definitions---------------------------------

//--------------------------end macro definitions-------------------------------

// global flags ---------------------------------------------------
int measure_and_save_flag = 0;
int get_voltage_flag = 0;
int start_plot_output_flag = 0;
int start_slowly_plot_output_flag = 0;
int short_plot_output_flag = 0;
int voltage_and_temperature_flag = 0;
int measure_voltage_temperature_flag = 0;
int voltage_temperature_current_flag = 0;


//----------------------------global constants----------------------------------
const double CURRENT_COEFFICIENT = 12500.0/4096.0/2.0;

char VERSION[] = "005-012 (EDA 4 12V 26.05.2017)";

//--------------------------end global constants--------------------------------

//----------------------------global variables----------------------------------
// class-container of common global variables
#include "clsCommon.cpp"
// global object, which contains all variables needed for inter-module
// communications
clsCommon common;

long timer3_counter = 0;

#define BUFFER_LENGTH 20000
#define CURRENT_BUF_LENGTH 4000

int counter = 0;
uint32_t data_buffer[BUFFER_LENGTH];
uint16_t current_buffer[CURRENT_BUF_LENGTH];

int data_buffer_index = 0;
int current_buffer_index = 0;
int frequency_buffer_index = 0;

char current_input[4];
char frequency_input[7];

// buffer for input symbols
#define INPUTBUFFERLENGTH 64
char input_buffer[INPUTBUFFERLENGTH];


// text buffer for output via uart
//char sampleTxtBuffer[16];



// global flag; when set, signals that output through uart with dma in process
volatile int uartOutputInProcess;
uint16_t adc1Buffer[ADC1BUFFERLEN];
volatile int adc1BufferHead = 0;
// cyclic buffer for ecg samples which feed a 50 Hz filter
uint16_t filterBuffer[ADC1BUFFERLEN];
// global variable "uint16_t filterBufferHead" is an index which points to the 
// current position for writing a new sample in the buffer 
uint16_t filterBufferHead;

// cpu tick counters for profiling purposes
volatile portTickType tickCounter0, tickCounter1, tickCounter2, tickCounter3;
volatile portTickType interval0 = 0, interval1 = 0;
volatile portTickType profTick0, profTick1, profTick2, profTick3;
volatile portTickType profInterval0 = 0, profInterval1 = 0;

// cpu tick counters for timer interrupt control
volatile portTickType timerTickCounter0 = 0, timerTickCounter1 = 0;
volatile portTickType timerInterval = 0;

// cpu tick counters for timeouts
volatile portTickType timeoutInCounter, timeoutOutCounter;
volatile portTickType timeoutInterval = 0;
// timeout flag
int timeoutFlag;

// timer 3 update isr blocking flag
// set by timer 3 isr and clear by ad7792SpiReadDataTask
// if not clear, we get isr bypass
int timer3IsrBypass = 0;
// flag which signals that normal timer3 isr sequence was interrupted
int timer3SeqInterrupted = 1;

// extra sample
uint16_t extraSample0, extraSample1;
int extraSampleCounter = 0;

// vector of coefficients for polynomial approximation
//double a[4];
double a[2];
// vector of constatnt terms of a linear system
//double v[4];
double v[2];
// vector of samples which we want to approximate
//uint16_t y[4] = {20000, 20000, 20200, 20000};
uint16_t y[4];
// vector of points of approximation curve
double yy[4];

uint8_t adResponce;

// adc conversion result
volatile uint16_t Conv;

// adc fault flag
int adcFault = 0;
int adcAbnormal = 0;

// flag which signals that we have to suspend qrs detection procedure
// set when we just detected an r-pick
int qrsSuspend = 0;
// marker which marks a moment when we have found a new r-pick
int qrsSuspendMarker = 0;

//test mode flag
int testFlag=0; 

// flag which signals to timer2 interrupt procedure that measurements on
int measure_on = 0;
int measurement_counter = 0;

char message[64];
char voltage_message[21];
char temperature_message[21];
char temperature2_message[21];
char test_voltage_message[21];
char test_temperature_message[21];
char test_temperature2_message[21];

#define FREQ_LENGTH 11
double frequencies[] = {0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 200.0};
double f_setting = 10.0;
double working_frequency = 10.0; // frequency in Hertc

int samples_in_one_pulse = 100;
int duration = 1;  // duration in seconds

long bluetooth_silence = 0;

//--------------------------end global variables--------------------------------

//--------------------------function prototypes--------------------------------

void measure_voltage_temperature();
void readAds8320(void);
void readAd7691(void);
void uart1_receive_interrupt_service(char r);
void measure_and_save();

//--------------------------end function prototypes--------------------------------


//---------------------------classes includes-----------------------------------



#include "clsGPIO.cpp"
clsGPIO gpio;
#include "LEDclass.cpp"
clsLED led;
#include "clsThermometer.cpp"
clsThermometer thermometer;

#include "UARTclass.cpp"
clsUART uart;

#include "clsTimer2.cpp"
clsTimer2 timer2;


#include "clsTimer3.cpp"
clsTimer3 timer3;

#include "clsSpi1.cpp"
clsSpi1 spi1;

#include "ADC1class.cpp"
clsADC1 adc1;



//#include "DAC1class.cpp"
//clsDAC1 dac1;

//#include "clsDacPower.cpp"
//clsDacPower dacPower;

#include "clsWatchdog.cpp"
clsWatchdog watchdog;


//-------------------------interrupt services------------------------------------

void uart1_receive_interrupt_service(char r)
{
    static int input_buffer_index = 0;
    
    if(r == '\n')   // end of input string received
    {
        bluetooth_silence = 0;
        
        int wrong_command = 0;
        
        // finalize input string
        input_buffer[input_buffer_index] = 0;
        // parse input string
        
        // 1. check GETU, GETUT, GETUTI
        if(strstr(input_buffer, "GETUTI") == input_buffer)
        {
            voltage_temperature_current_flag = 1;
        }
        else if(strstr(input_buffer, "GETUT") == input_buffer)
        {
            voltage_and_temperature_flag = 1;
        }
        else if(strstr(input_buffer, "GETU") == input_buffer)
        {
            get_voltage_flag = 1;
        }
        else if(strstr(input_buffer, "SETFREQUENCY") == input_buffer)
        {
            char ch;
            if((sscanf(input_buffer, "SETFREQUENCY %lf%c", &(f_setting), &ch) == 2) && (ch == '\r'))
            {
                // correct frequency settings
                if(f_setting > 200)
                    f_setting = 200;
                if(f_setting < 0.1)
                    f_setting = 0.1;
                
                // print echo
                uart.transmitMessage("frequency = ");
                uart.transmitMessage(input_buffer);
                uart.transmitMessage("\r\n");
                
                // correct frequency (snap to frequency grid)
                double min_difference = abs(f_setting - frequencies[0]);
                int min_index = 0;
                
                for(int i = 1; i < FREQ_LENGTH; i++)
                {
                    if(min_difference > abs(f_setting - frequencies[i]))
                    {
                        min_difference = abs(f_setting - frequencies[i]);
                        min_index = i;
                    }
                }
                
                working_frequency = frequencies[min_index];
                // adjust duration
                if(min_index <= 3)
                    duration = 10;
                else if(min_index >= 6)
                    duration = 1;
                else
                    duration = 5;
                
                // calculate number of pulses
                common.number_of_pulses = (int)(duration*working_frequency);
                samples_in_one_pulse = (int)( 1000 / working_frequency );
            }
            else // something wrong
            {
                wrong_command = 1;
            }
        }
        else if(strstr(input_buffer, "SETCURRENT") == input_buffer)
        {
            char ch;
            if((sscanf(input_buffer, "SETCURRENT %d%c", &(common.i_setting), &ch) == 2) && (ch == '\r'))
            {
                // correct current settings
                if(common.i_setting > 255)
                    common.i_setting = 255;
                if(common.i_setting < 1)
                    common.i_setting = 1;
                
                // print echo
                uart.transmitMessage("current = ");
                uart.transmitMessage(input_buffer);
                uart.transmitMessage("\r\n");
                
                // transform gpios
                uint8_t I_raw = (uint8_t)(common.i_setting);
                uint8_t I_final = 0;
                // 0 - 5, 1 - 4, 2 - 6, 3 - 7, 4 - 0, 5 - 1, 6 - 2, 7 - 3
               
                /*
                I_final = I_raw >> 4;    // 4 - 0, 5 - 1, 6 - 2, 7 - 3
                I_final = I_final | ((I_raw & 0x01)<<5);      // 0 - 5
                I_final = I_final | (((I_raw>>1) & 0x01)<<4); // 1 - 4
                I_final = I_final | (((I_raw>>2) & 0x01)<<6); // 2 - 6
                I_final = I_final | (((I_raw>>3) & 0x01)<<7); // 3 - 7
                */
                I_final = I_raw;
                
                common.resistors_gpios = (uint16_t)I_final;
            }
            else // something wrong
            {
                wrong_command = 1;
            }
        }
        else if(strstr(input_buffer, "STARTTEST") == input_buffer)
        {
            measure_and_save_flag = 1;
            ////////////////////////////
            //working_frequency = 10.0;
            //common.i_setting = 32;
            //duration = 1;
            //common.number_of_pulses = (int)(duration*working_frequency);
            //samples_in_one_pulse = (int)( 1000 / working_frequency );
        }
        else if(strstr(input_buffer, "GETTESTDATASLOW") == input_buffer)
        {
            start_slowly_plot_output_flag = 1;
        }
        else if(strstr(input_buffer, "GETTESTDATAFAST") == input_buffer)
        {
            start_plot_output_flag = 1;
        }
        else if(strstr(input_buffer, "GETVERSION") == input_buffer)
        {
            sprintf(message, "version = %s\r\n", VERSION);
            uart.transmitMessage(message);
        }
        else // nothing of known
        {
            wrong_command = 1;
        }
        
        if(wrong_command)
        {
            //char wrong_command_mesage[INPUTBUFFERLENGTH + 32];
            //sprintf(wrong_command_mesage, "WRONG COMMAND %s\r\n", input_buffer);
            //uart.transmitMessage(wrong_command_mesage);
        }
        
        // initialize input string
        input_buffer_index = 0;
    }
    else // no end of string reached
    {
        input_buffer[input_buffer_index] = r;
        input_buffer_index++;
        
        if(input_buffer_index >= INPUTBUFFERLENGTH)  // something wrong!!! 
            input_buffer_index = 0; // cycle buffer
    }

    
}

void measure_current(int descriptor)
{
    static uint16_t initial_current = 0xffff;
    uint16_t adc2Data;
    
    ADC_SoftwareStartConv(ADC2);
    while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
    adc2Data = ADC_GetConversionValue(ADC2);
    
    if(descriptor == 0) // proizvodim nachalnoe izmerenie toka
    {
        initial_current = adc2Data;
    }
    else // proizvodim izmerenie v konce impulsa
    {
        //current_buffer[current_buffer_index] = adc1Data;
        //current_buffer[current_buffer_index] = initial_current;
        //*
        if(initial_current <= adc2Data)
            current_buffer[current_buffer_index] = (uint16_t)((adc2Data - initial_current)*CURRENT_COEFFICIENT);
        else
            current_buffer[current_buffer_index] = 0;
        //*/
        current_buffer_index++;
    }
}

void measure_and_save() // STARTTEST
{
    // test: 10 sec 10 Hz 2000 KHz
    // total impulses 100
    // 200 measurements in every impulse
    
    int i;
    char message[64];
    
    data_buffer_index = 0;
    current_buffer_index = 0;
    // stop low speed timer
    timer3.stopTimer();
    
    measure_voltage_temperature();
    strcpy(test_voltage_message, voltage_message);
    strcpy(test_temperature_message, temperature_message);
    strcpy(test_temperature2_message, temperature2_message);
    
    uart.transmitMessage("START TEST\r\n");
    //uart.transmitMessage(voltage_message);
    //uart.transmitMessage("\r\n");
    //uart.transmitMessage(temperature_message);
    //uart.transmitMessage("\r\n");
    sprintf(message, "current = %d\r\n", common.i_setting);
    uart.transmitMessage(message);
    sprintf(message, "frequency = %f\r\n", working_frequency);
    uart.transmitMessage(message);
    // start high speed timer
    timer2.startTimer();
    
    
    // loop on impulses
    for(i=0; i<common.number_of_pulses; i++)
    {
        // measure current
        measure_current(0);
        //high semyperiod =====================
       
        // start impulse
        gpio.high();
        // start measurements
        measurement_counter = 0;
        measure_on = 1;
        // wait for end of measurements
        while(measure_on);
        // stop high speed timer
        //timer2.stopTimer();
        // measure current
        measure_current(1);
        // stop impulse
        gpio.low();
        //low semyperiod =====================
        // start high speed timer
        //timer2.startTimer();
        // start measurements
        measurement_counter = 0;
        measure_on = 1;
        // wait for end of measurements
        while(measure_on);
        
    }
    
    // stop high speed timer
    timer2.stopTimer();
    
    
    measure_and_save_flag = 0;
    uart.transmitMessage("FINISH TEST\r\n");
    
    // start low speed timer
    timer3.startTimer();
}

void readAds8320(void)
{
   uint32_t ads8320Data = 0;
   uint8_t byteBuffer;
   
   GPIOA->BSRRH=GPIO_Pin_8;  //CS pin low (start spi communication)
   
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   SPI1->DR=0x55;
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
   byteBuffer=SPI1->DR;
   // apply 00000011 mask (0x03); 5 clocks and first zero dout
   byteBuffer &= 0x03;
   ads8320Data |= (uint32_t)byteBuffer;
   ads8320Data <<= 8;
   
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   SPI1->DR=0x55;
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
   byteBuffer=SPI1->DR;
   ads8320Data |= (uint32_t)byteBuffer;
   ads8320Data <<= 8;
   
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   SPI1->DR=0x55;
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
   byteBuffer=SPI1->DR;
   ads8320Data |= (uint32_t)byteBuffer;
   ads8320Data >>= 2;  // 2 "extra" clocks
   
   common.ads8320Data = ads8320Data;
   
   GPIOA->BSRRL=GPIO_Pin_8;  //CS pin high (stop spi communication)
}

void readAd7691(void)
{
   int32_t ad7691Data = 0;
   uint8_t byteBuffer = 0;
   
   GPIOA->BSRRL=GPIO_Pin_8;  //CNV pin high (start analog to digit conversion)
   GPIOA->BSRRH=GPIO_Pin_8;  //CNV pin low (initiate busy indicator)
   
   while((GPIOA->IDR & GPIO_Pin_6) != (uint32_t)Bit_RESET); // wait for an end of conversion
   
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   SPI1->DR=0x55;
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
   byteBuffer=SPI1->DR;
   // apply mask 0x7f
   //byteBuffer &= (uint8_t)0x7f;
   ad7691Data |= (uint32_t)byteBuffer;
   ad7691Data <<= 8;
   
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   SPI1->DR=0x55;
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
   byteBuffer=SPI1->DR;
   ad7691Data |= (uint32_t)byteBuffer;
   ad7691Data <<= 8;
   
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   SPI1->DR=0x55;
   while ((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET );
   while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
   byteBuffer=SPI1->DR;
   ad7691Data |= (uint32_t)byteBuffer;
   
   ad7691Data <<= 8;   // проверяем знак
   if(ad7691Data < 0)
       ad7691Data = 0;
   else // положительное число
   {
       ad7691Data >>= 8;
       ad7691Data >>= 6;  // 6 "extra" clocks
   }
   
   common.ad7691Data = ad7691Data;
   
}


void measure_voltage_temperature()
{
    int i;
    double voltage = 0;
    float temperature1;
    
#ifdef THEMPERATURE2
    float temperature2;
#endif
    
    measure_voltage_temperature_flag = 0;

    // toggle led
    led.toggleLed();
    
    // get data from external adc
    for(i=0;i<100;i++)
    {
        readAd7691();
        voltage += (double)common.ad7691Data;
    }
    voltage /= 100.0;
    //debug*****
    //voltage = (double)common.ad7691Data;
    //debug*****
    voltage = voltage*2041.0/262144.0*7.675;
    // voltage data output
    //debug
    //printf("%d\r\n", common.ads8320Data);
    sprintf(voltage_message, "U = %7.1f", voltage);
    //uart.transmitMessage(voltage_message);
    //uart.transmitMessage("\r\n");
    //printf("%s\r\n", voltage_message);
    
    //get temperature
    temperature1 = thermometer.readThemperature(1);
#ifdef THEMPERATURE2
    temperature2 = thermometer.readThemperature(2);
#endif
    // temperature data output
    //debug
    //printf("%d\r\n", common.ads8320Data);
    sprintf(temperature_message, "T = %7.1f", temperature1);
#ifdef THEMPERATURE2
    sprintf(temperature2_message, "O = %7.1f", temperature2);
#else
    sprintf(temperature2_message, "O = -----");
#endif
    //uart.transmitMessage(temperature_message);
    //uart.transmitMessage("\r\n");
    //printf("%s\r\n", temperature_message);
    //*/
    
       
}

void plot_output(int output_slowly)
{
    int j, k;
    
    //char message[64];
    char voltage_message[21];
    char current_message[128];
    
    // stop low speed timer
    timer3.stopTimer();
    
    uart.transmitMessage("START DATA\r\n");
    sprintf(message, "version = %s\r\n", VERSION);
    uart.transmitMessage(message);
    sprintf(message, "current = %d\r\n", common.i_setting);
    uart.transmitMessage(message);
    sprintf(message, "frequency = %f\r\n", working_frequency);
    uart.transmitMessage(message);
    sprintf(message, "number of samples = %d\r\n", samples_in_one_pulse);
    uart.transmitMessage(message);
    uart.transmitMessage(test_voltage_message);
    uart.transmitMessage("\r\n");
    uart.transmitMessage(test_temperature_message);
    uart.transmitMessage("\r\n");
    uart.transmitMessage(test_temperature2_message);
    uart.transmitMessage("\r\n");
    uart.transmitMessage("Il = -----\r\n");
    uart.transmitMessage("Ih = -----\r\n");
    
    uart.transmitMessage("START LOG\r\n");
    //*
    for(k=0; k<common.number_of_pulses*2; k++)
    {
        if(k%2 == 0)
        {
            sprintf(current_message, "I = %d **************************************** %04d\r\n", current_buffer[k/2], k);
            uart.transmitMessage(current_message);
        }
        else
        {
            sprintf(current_message, "I = %d **************************************** %04d\r\n", 0, k);
            uart.transmitMessage(current_message);
        }
        
        for(j=0; j<samples_in_one_pulse; j++)
        {
            sprintf(voltage_message, "%7.1f", (double)(data_buffer[k*samples_in_one_pulse + j])*2041.0/262144.0*7.675);
            uart.transmitMessage(voltage_message);
            uart.transmitMessage("\r\n");
            //printf("%s\r\n", voltage_message);
            if(output_slowly)
            {
                for(volatile long i=0; i<13100; i++);
                for(volatile long i=0; i<13100; i++);
                for(volatile long i=0; i<13100; i++);
                for(volatile long i=0; i<13100; i++);
                for(volatile long i=0; i<13100; i++);
            }
        }
        
        
    }
    //*/
    
    uart.transmitMessage("FINISH LOG\r\n");
    uart.transmitMessage("FINISH DATA\r\n");
    
    /*
    for(j=0; j<(CURRENT_BUF_LENGTH/divisor); j++)
    {
        sprintf(current_message, "%05d", current_buffer[j]);
        uart.transmitMessage(current_message);
        uart.transmitMessage("\r\n");
        for(volatile long i=0; i<13100; i++);
        for(volatile long i=0; i<13100; i++);
        for(volatile long i=0; i<13100; i++);
    }
    */
    
    start_plot_output_flag = 0;
    short_plot_output_flag = 0;
    start_slowly_plot_output_flag = 0;
    
    // start low speed timer
    timer3.startTimer();
}


void get_voltage()  // GETU
{
    get_voltage_flag = 0;
    
    measure_voltage_temperature();
    
    uart.transmitMessage(voltage_message);
    uart.transmitMessage("\r\n");
   
}


void get_voltage_and_temperature()  // GETUT
{
    voltage_and_temperature_flag = 0;
    
    measure_voltage_temperature();
    
    uart.transmitMessage(voltage_message);
    uart.transmitMessage("; ");
    
    uart.transmitMessage(temperature_message);
    uart.transmitMessage("\r\n");
}

void get_voltage_temperature_current()  // GETUTI
{
    voltage_temperature_current_flag = 0;
    
    measure_voltage_temperature();
    
    uart.transmitMessage("START GETUTI\r\n");
    
    uart.transmitMessage(voltage_message);
    uart.transmitMessage("\r\n");
    uart.transmitMessage(temperature_message);
    uart.transmitMessage("\r\n");
    uart.transmitMessage(temperature2_message);
    uart.transmitMessage("\r\n");
    uart.transmitMessage("Il = -----\r\n");
    uart.transmitMessage("Ih = -----\r\n");
    
    uart.transmitMessage("FINISH GETUTI\r\n");
}
    


//-----------------------end tasks definitions----------------------------------


// SOME NOTABENES

// timeout with increment of counter: 13100 -> 1 milisecond (120 MHz)
//for(volatile long i=0; i<13100; i++);
// timeout with increment of counter: 129 -> 1 microsecond (120 MHz)
//for(volatile long i=0; i<129; i++);

//timing control <<<<<<<<<<<<<<<<<<<<<
//GPIOD->BSRRL = GPIO_Pin_5;   //pd5 high
//GPIOD->BSRRH = GPIO_Pin_5;   //pd5 low
//timing control >>>>>>>>>>>>>>>>>>>>>






//-----------------------------------------------------------------------------
//*****************************************************************************
//                              MAIN
//*****************************************************************************
//-----------------------------------------------------------------------------


int main()
{
    
   
   timer3.startTimer();
   //timer2.startTimer();
   
   while(1)
   {
       if(measure_voltage_temperature_flag)
           measure_voltage_temperature();
       if(voltage_temperature_current_flag)
           get_voltage_temperature_current();
       if(voltage_and_temperature_flag)
           get_voltage_and_temperature();
       if(measure_and_save_flag)
           measure_and_save();
       if(get_voltage_flag)
           get_voltage();
       if(start_plot_output_flag)
           plot_output(0);
       if(start_slowly_plot_output_flag)
           plot_output(1);
       
   }
   
  
}


// auxiliary functions
double abs(double value)
{
    if(value >= 0.0)
        return value;
    else
        return -value;
}
