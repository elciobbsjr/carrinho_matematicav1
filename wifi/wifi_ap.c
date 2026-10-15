#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/netif.h"
#include "lwip/ip4_addr.h"

#include "dhcpserver.h"
#include "dnsserver.h"

#include "wifi_ap.h"


// ======================================================
// CONFIGURAÇÕES DA REDE
// ======================================================

#define WIFI_SSID       "CORRIDA_MATEMATICA"
#define WIFI_PASSWORD   "12345678"

#define WIFI_IP_1       192
#define WIFI_IP_2       168
#define WIFI_IP_3       4
#define WIFI_IP_4       1

#define MAX_WIFI_CLIENTS 8


// ======================================================
// LED RGB
// ======================================================

// Vermelho: GPIO13 com resistor de 220 ohms
#define LED_RED_PIN 13

// Verde: GPIO11 com resistor de 220 ohms
#define LED_GREEN_PIN 11

// Azul: GPIO12 com resistor de 150 ohms
#define LED_BLUE_PIN 12


// ======================================================
// SERVIDORES
// ======================================================

static dhcp_server_t dhcp_server;
static dns_server_t dns_server;


// ======================================================
// CONTROLE DE CLIENTES
// ======================================================

static int client_count = 0;
static int previous_client_count = -1;


// ======================================================
// FUNÇÕES DO LED RGB
// ======================================================

static void rgb_init(void)
{
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);

    // Começa tudo desligado
    gpio_put(LED_RED_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_BLUE_PIN, 0);
}


static void rgb_set(
    bool red,
    bool green,
    bool blue
)
{
    gpio_put(LED_RED_PIN, red);
    gpio_put(LED_GREEN_PIN, green);
    gpio_put(LED_BLUE_PIN, blue);
}


// ======================================================
// MOSTRAR CLIENTES
// ======================================================

static void wifi_print_clients(
    int quantidade,
    uint8_t macs[]
)
{
    if (quantidade == 0) {

        printf(
            "Nenhum dispositivo conectado.\n"
        );

        return;
    }


    printf(
        "Dispositivos conectados: %d\n",
        quantidade
    );


    for (int i = 0; i < quantidade; i++) {

        uint8_t *mac = &macs[i * 6];

        printf(
            "Cliente %d - MAC: "
            "%02X:%02X:%02X:%02X:%02X:%02X\n",

            i + 1,

            mac[0],
            mac[1],
            mac[2],
            mac[3],
            mac[4],
            mac[5]
        );
    }
}


// ======================================================
// INICIALIZAÇÃO DO WIFI
// ======================================================

bool wifi_ap_init(void)
{
    printf("\n");
    printf("========================================\n");
    printf("          CORRIDA MATEMATICA\n");
    printf("========================================\n\n");


    // --------------------------------------------------
    // Inicializa LED RGB
    // --------------------------------------------------

    rgb_init();


    // Vermelho = inicializando
    rgb_set(
        true,
        false,
        false
    );


    printf(
        "Inicializando chip Wi-Fi...\n"
    );


    // --------------------------------------------------
    // Inicializa CYW43
    // --------------------------------------------------

    if (cyw43_arch_init()) {

        printf(
            "ERRO: nao foi possivel inicializar "
            "o CYW43.\n"
        );


        // Vermelho permanece aceso
        rgb_set(
            true,
            false,
            false
        );


        return false;
    }


    // --------------------------------------------------
    // Cria Access Point
    // --------------------------------------------------

    printf(
        "Criando Access Point...\n"
    );


    cyw43_arch_enable_ap_mode(
        WIFI_SSID,
        WIFI_PASSWORD,
        CYW43_AUTH_WPA2_AES_PSK
    );


    // --------------------------------------------------
    // Configuração de IP
    // --------------------------------------------------

    ip4_addr_t ip;
    ip4_addr_t mask;


    IP4_ADDR(
        &ip,
        WIFI_IP_1,
        WIFI_IP_2,
        WIFI_IP_3,
        WIFI_IP_4
    );


    IP4_ADDR(
        &mask,
        255,
        255,
        255,
        0
    );


    // Configura IP do Access Point
    netif_set_addr(
        &cyw43_state.netif[CYW43_ITF_AP],
        &ip,
        &mask,
        &ip
    );


    // --------------------------------------------------
    // Inicializa DHCP
    // --------------------------------------------------

    dhcp_server_init(
        &dhcp_server,
        &ip,
        &mask
    );


    // --------------------------------------------------
    // Inicializa DNS
    // --------------------------------------------------

    dns_server_init(
        &dns_server,
        &ip
    );


    // --------------------------------------------------
    // Access Point pronto
    // --------------------------------------------------

    // Azul = Wi-Fi funcionando e aguardando jogador
    rgb_set(
        false,
        false,
        true
    );


    // --------------------------------------------------
    // Informações no terminal
    // --------------------------------------------------

    printf("\n");
    printf("----------------------------------------\n");
    printf("Wi-Fi iniciado com sucesso!\n");
    printf("----------------------------------------\n");

    printf(
        "SSID : %s\n",
        WIFI_SSID
    );

    printf(
        "Senha: %s\n",
        WIFI_PASSWORD
    );

    printf(
        "IP   : %d.%d.%d.%d\n",
        WIFI_IP_1,
        WIFI_IP_2,
        WIFI_IP_3,
        WIFI_IP_4
    );

    printf("----------------------------------------\n");

    printf(
        "DHCP iniciado.\n"
    );

    printf(
        "DNS iniciado.\n"
    );

    printf(
        "LED: AZUL - aguardando jogador.\n"
    );

    printf(
        "\nAguardando conexoes...\n\n"
    );


    previous_client_count = -1;
    client_count = 0;


    return true;
}


// ======================================================
// ATUALIZAÇÃO DO WIFI
// ======================================================

void wifi_ap_update(void)
{
    uint8_t macs[MAX_WIFI_CLIENTS * 6];


    memset(
        macs,
        0,
        sizeof(macs)
    );


    // Define quantos clientes cabem no buffer
    int numero_stas = MAX_WIFI_CLIENTS;


    // --------------------------------------------------
    // Busca dispositivos conectados ao AP
    // --------------------------------------------------

    cyw43_wifi_ap_get_stas(
        &cyw43_state,
        &numero_stas,
        macs
    );


    client_count = numero_stas;


    // --------------------------------------------------
    // Controle do LED RGB
    // --------------------------------------------------

    if (client_count > 0) {

        // Verde = jogador conectado
        rgb_set(
            false,
            true,
            false
        );

    } else {

        // Azul = aguardando jogador
        rgb_set(
            false,
            false,
            true
        );
    }


    // --------------------------------------------------
    // Mostra mudanças apenas quando necessário
    // --------------------------------------------------

    if (client_count != previous_client_count) {

        printf("\n");
        printf("----------------------------------------\n");


        if (
            client_count > previous_client_count &&
            previous_client_count >= 0
        ) {

            printf(
                "Novo dispositivo conectado!\n"
            );

        } else if (
            previous_client_count >= 0 &&
            client_count < previous_client_count
        ) {

            printf(
                "Dispositivo desconectado.\n"
            );
        }


        wifi_print_clients(
            client_count,
            macs
        );


        if (client_count > 0) {

            printf(
                "LED: VERDE - jogador conectado.\n"
            );

        } else {

            printf(
                "LED: AZUL - aguardando jogador.\n"
            );
        }


        printf("----------------------------------------\n");


        previous_client_count = client_count;
    }
}


// ======================================================
// RETORNA QUANTIDADE DE CLIENTES
// ======================================================

int wifi_ap_get_client_count(void)
{
    return client_count;
}


// ======================================================
// FINALIZAÇÃO
// ======================================================

void wifi_ap_deinit(void)
{
    printf(
        "Encerrando Wi-Fi...\n"
    );


    // Vermelho durante encerramento
    rgb_set(
        true,
        false,
        false
    );


    // Desliga DNS
    dns_server_deinit(
        &dns_server
    );


    // Desliga DHCP
    dhcp_server_deinit(
        &dhcp_server
    );


    // Desliga CYW43
    cyw43_arch_deinit();


    // Desliga LED
    rgb_set(
        false,
        false,
        false
    );
}