#ifndef __WIFIAPI_H
#define __WIFIAPI_H

#include <WiFi.h>
#include <HTTPClient.h>

#include "Settings.ino"

class WifiAPI {

    private:
    
        char* accessToken = "";
        String url = "https://graph.facebook.com/v15.0/me/friends?access_token=";
        int followersAnteriores = 0;
        int contadorSleep = 0;

    public:

        void connectWifi() { // Se conecta al Wifi concretado en settings.ino
            WiFi.begin(SSIDWIFI, PASSWORDWIFI);
            while (WiFi.status() != WL_CONNECTED) {
                long startMillis = millis();
                while (millis() - startMillis < 1000) {
                    }
                Serial.println("Conectando...");
            }
            Serial.print("Conectado correctamente a ");
            Serial.println(SSIDWIFI);
        }
        
        int seeFollowers() {
            HTTPClient http;
            http.begin(url + accessToken); // Se inicia la url + el token 
            int httpResponseCode = http.GET(); // Llamamos a la URL 

            if (httpResponseCode > 0) { 
                String response = http.getString(); // Obtener la respuesta como un String
                int followers = 0;
                char* total_count = "\"total_count\":"; // Parte de la respuesta que necesitamos

                char* position = strstr(response.c_str(), total_count); // Buscar total_count dentro de la cadena
                
                position += strlen(total_count); // Mover el puntero al carácter después de total_count
                
                followers = atoi(position); // Convertir el carácter a entero

                Serial.print("Total de amigos: ");
                Serial.println(followers);

                http.end();

                // Control de sueño
                if (followersAnteriores == followers)
                    contadorSleep++;
                if (contadorSleep == SLEEPCOUNTER) {
                    contadorSleep = 0;
                    Serial.println("Entrando en sueño ligero...");
                    unsigned long startMillis = millis();
                    while (millis() - startMillis < 100) {
                    }
                    esp_light_sleep_start(); // Entra en modo de sueño ligero
                    Serial.println("Despertando del sueño");
                }
                followersAnteriores = followers;

                return followers;
            }
            http.end();
            return -1;
        }

};

#endif
