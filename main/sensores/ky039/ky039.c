#include "ky039.h"
#include <float.h>
#include "esp_timer.h"
#include "adc_module.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_timer.h>


float readings[SAMP_SIZE], first, second, third;
float readingsSum, currentReading, sensorValue, startTime, lastReading, printValue, bpm;
long int currentTime, readingsIndex, bpmListIndex, lastBeat;
int riseCount, n;
bool rising;
int HBChannel;

void setup(int channel) 
{
    adc_init(ADC_UNIT_1);
    adc_config_pin(channel);
    HBChannel = channel;

	for(int i = 0; i < SAMP_SIZE; i++)
		readings[i] = 0;
	readingsSum = 0;
	readingsIndex = 0;
	bpmListIndex = 0;
}


float valorAnterior = 0.0;
float valorMaximo = 0.0;
int quantidadeBatidas = 0;
float fatorFiltro = 0.75;
int minimoEntreBatidas = 300;

long entreBatidas;
long tempoBPM;

void monitorBPM()
{
    // Inicialização das variáveis globais
    entreBatidas = esp_timer_get_time() / 1000; // Convertendo para milissegundos
    tempoBPM = esp_timer_get_time() / 1000;     // Convertendo para milissegundos

    while (true)
    {
        int valorLido = analogRead(HBChannel);

        int limiarSensorTocado = 1900;

        
          float valorFiltrado = fatorFiltro * valorAnterior + (1 - fatorFiltro) * valorLido;

          float valorDiferenca = valorFiltrado - valorAnterior;

          valorAnterior = valorFiltrado;

          if ((valorDiferenca >= valorMaximo) && (esp_timer_get_time() / 1000 > entreBatidas + minimoEntreBatidas))
          {
              valorMaximo = valorDiferenca;
              entreBatidas = esp_timer_get_time() / 1000;
              quantidadeBatidas++;
          }

          valorMaximo = valorMaximo * 0.97;

          if (esp_timer_get_time() / 1000 >= tempoBPM + 15000)
          {
              int bpm = quantidadeBatidas * 4;
              if (valorLido > limiarSensorTocado){
                    printf("batidas: %d\n", bpm);
                }  
              tempoBPM = esp_timer_get_time() / 1000;
              quantidadeBatidas = 0;
          }

          vTaskDelay(50 / portTICK_PERIOD_MS);
        
    }
}