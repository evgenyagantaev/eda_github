//This task provides sdio write operation
void sdCardTask(void *pvParameters)
{
  
  while (1)
  { 
    xSemaphoreTake(sdioOutputSemaphore, portMAX_DELAY);
    if(!common.commandMode) sdio.writeToFile();
    taskYIELD(); 
  }
  
}