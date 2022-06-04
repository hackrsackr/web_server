#pragma once
#include <WiFi.h>
#include "server_config.hpp"

uint8_t attempts_before_restart = 20;

void establish_connection()
{
    // WiFi
    WiFi.disconnect(true);
    delay(1000);
    WiFi.begin(_SSID, _PASS);

    uint8_t failed_connections = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("connecting..");
        failed_connections++;
        if (failed_connections > attempts_before_restart)
        {
            Serial.println("restarting..");
            ESP.restart();
        }
    }
}