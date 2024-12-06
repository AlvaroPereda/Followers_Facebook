#include <esp_sleep.h>

#include "LedMatrix.ino"
#include "WifiAPI.ino"
#include "Settings.ino"


LedMatrix matrix;
WifiAPI api;

hw_timer_t *timer_API = NULL;
volatile bool checkFollowers = false; 

#define boot 0 // Se define el pin del boot

void IRAM_ATTR onTimer() {
  checkFollowers = true; 
}

void setup() {
  Serial.begin(115200); 
  Serial.println("Serial inicializado");
  for (int i = 0; i < 12; ++i) // Rellena con huecos en blanco
  {
    Serial.println();
  }

  //https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/sleep_modes.html 
  gpio_wakeup_enable((gpio_num_t)boot, GPIO_INTR_LOW_LEVEL); // Se pone el gpio_num_t para convertirlo en una salida gpio_num_t
  esp_sleep_enable_gpio_wakeup(); // Habilitar el wakeup desde GPIO

  matrix.initializeMatrix(); 
  matrix.resetMatrix();

  api.connectWifi(); 

  matrix.setNumberMatrix(api.seeFollowers()); // Comprobamos el numero de seguidores 

  timer_API = timerBegin(1000000); //Se crea el timer
  timerAttachInterrupt(timer_API, &onTimer); // Asociar la interrupción
  timerAlarm(timer_API, FINDNEWFOLLOWERS, true, 0); // Se define el tiempo de espera del timer
}

void loop() {  
  if (checkFollowers) { 
    checkFollowers = false;
    matrix.setNumberMatrix(api.seeFollowers()); // Comprobamos el numero de seguidores 
  }
  matrix.printNumber(); // Dibujamos en la matriz de leds
}