#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "wifi_ap.h"
#include "webserver.h"
#include "motor.h"


int main(void)
{
    stdio_init_all();

    sleep_ms(2000);

    printf("\n");
    printf("========================================\n");
    printf("    INICIANDO CARRINHO MATEMATICO\n");
    printf("========================================\n");


    // ==================================================
    // WI-FI
    // ==================================================

    if (!wifi_ap_init())
    {
        printf("ERRO FATAL: falha ao iniciar Wi-Fi.\n");

        while (true)
        {
            sleep_ms(1000);
        }
    }


    // ==================================================
    // MOTORES
    // ==================================================

    if (!motor_init())
    {
        printf("ERRO FATAL: falha ao iniciar motores.\n");

        while (true)
        {
            sleep_ms(1000);
        }
    }



    // ==================================================
    // SERVIDOR WEB
    // ==================================================

    if (!webserver_init())
    {
        printf("ERRO FATAL: falha ao iniciar servidor web.\n");

        motor_deinit();
        wifi_ap_deinit();

        while (true)
        {
            sleep_ms(1000);
        }
    }


    printf("\n");
    printf("========================================\n");
    printf("           SISTEMA PRONTO\n");
    printf("========================================\n");

    printf("Wi-Fi: CORRIDA_MATEMATICA\n");
    printf("Endereco: http://192.168.4.1\n");

    printf("========================================\n\n");


    // ==================================================
    // LOOP PRINCIPAL
    // ==================================================

    while (true)
    {

#if PICO_CYW43_ARCH_POLL

        cyw43_arch_poll();

#endif

        wifi_ap_update();

        // MUITO IMPORTANTE
        motor_update();

        sleep_ms(10);
    }


    return 0;
}