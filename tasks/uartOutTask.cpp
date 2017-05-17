// This task transmits via uart (using DMA) all messages from all tasks
// in one single stream
  
void uartOutTask(void *parameters)
{
   while(1)
   {
      if(!uartOutputInProcess)
      {
         uart.outStringViaDma();
      }
      taskYIELD();
   }
}