#ifndef WIFI_AP_H
#define WIFI_AP_H

#include <stdbool.h>

// Inicializa o Pico W como Access Point
bool wifi_ap_init(void);

// Deve ser chamada continuamente no loop principal
void wifi_ap_update(void);

// Retorna quantos dispositivos estão conectados
int wifi_ap_get_client_count(void);

// Finaliza Wi-Fi, DHCP e DNS
void wifi_ap_deinit(void);

#endif