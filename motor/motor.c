#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "pico/stdlib.h"

#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "motor.h"


// ======================================================
// PINAGEM DOS SERVOS
// ======================================================

#define SERVO_ESQUERDO_PIN 16
#define SERVO_DIREITO_PIN  17


// ======================================================
// CONFIGURAÇÃO PWM
// ======================================================

// Servo padrão:
// frequência aproximada de 50 Hz
//
// período = 20 ms = 20000 us
//
// Vamos configurar:
// 1 contador PWM = 1 microssegundo
//
// Portanto:
//
// 1500 = 1500 us
// 1700 = 1700 us
// 1300 = 1300 us

#define PWM_FREQUENCIA_BASE 1000000.0f

#define PWM_WRAP 19999


// ======================================================
// CALIBRAÇÃO DOS SERVOS
// ======================================================

// Servo contínuo normalmente para próximo de 1500 us.
//
// Dependendo do servo, pode ser necessário ajustar:
//
// 1470
// 1480
// 1490
// 1500
// 1510
// 1520
//
// Se o servo ficar girando quando deveria estar parado,
// ajuste este valor.

#define SERVO_ESQUERDO_STOP_US 1500
#define SERVO_DIREITO_STOP_US  1500


// ======================================================
// MOVIMENTO PARA FRENTE
// ======================================================
//
// Como as rodas ficam espelhadas:
//
// Servo esquerdo gira em um sentido.
// Servo direito gira no sentido contrário.
//
// Se o carrinho girar em vez de andar reto,
// inverta 1700 e 1300 de um dos lados.

#define SERVO_ESQUERDO_FRENTE_US 1900
#define SERVO_DIREITO_FRENTE_US  1100


// ======================================================
// TEMPO DE AVANÇO POR ACERTO
// ======================================================
//
// Cada resposta correta:
// carrinho anda por 800 ms.
//
// Depois podemos calibrar isso para:
// 10 cm
// 15 cm
// 20 cm
// etc.

#define MOTOR_TEMPO_AVANCO_MS 3000


// ======================================================
// ESTADO
// ======================================================

static bool motor_inicializado = false;

static bool motor_movendo = false;

static absolute_time_t tempo_parada;


// ======================================================
// PWM
// ======================================================

static uint motor_pwm_slice;


// ======================================================
// ESCREVE PULSO EM UM SERVO
// ======================================================

static void servo_write_us(
    uint gpio,
    uint16_t pulse_us
)
{
    pwm_set_gpio_level(
        gpio,
        pulse_us
    );
}


// ======================================================
// CONFIGURA PWM
// ======================================================

static bool configurar_pwm_servos(void)
{
    // --------------------------------------------------
    // Coloca GPIOs em função PWM
    // --------------------------------------------------

    gpio_set_function(
        SERVO_ESQUERDO_PIN,
        GPIO_FUNC_PWM
    );

    gpio_set_function(
        SERVO_DIREITO_PIN,
        GPIO_FUNC_PWM
    );


    // --------------------------------------------------
    // Descobre os slices
    // --------------------------------------------------

    uint slice_esquerdo =
        pwm_gpio_to_slice_num(
            SERVO_ESQUERDO_PIN
        );

    uint slice_direito =
        pwm_gpio_to_slice_num(
            SERVO_DIREITO_PIN
        );


    printf(
        "[MOTOR] Slice esquerdo: %u\n",
        slice_esquerdo
    );

    printf(
        "[MOTOR] Slice direito : %u\n",
        slice_direito
    );


    // GPIO16 e GPIO17 devem utilizar o mesmo slice.
    //
    // Isso é normal:
    //
    // GPIO16 = canal A
    // GPIO17 = canal B

    if (slice_esquerdo != slice_direito)
    {
        printf(
            "[MOTOR] AVISO: servos estao em slices diferentes.\n"
        );
    }


    motor_pwm_slice =
        slice_esquerdo;


    // --------------------------------------------------
    // Clock real do RP2040
    // --------------------------------------------------

    uint32_t clock_hz =
        clock_get_hz(
            clk_sys
        );


    float divisor =
        (float)clock_hz /
        PWM_FREQUENCIA_BASE;


    printf(
        "[MOTOR] Clock sistema: %lu Hz\n",
        (unsigned long)clock_hz
    );

    printf(
        "[MOTOR] Divisor PWM: %.2f\n",
        divisor
    );


    // --------------------------------------------------
    // Configuração do PWM
    // --------------------------------------------------

    pwm_config config =
        pwm_get_default_config();


    pwm_config_set_clkdiv(
        &config,
        divisor
    );


    pwm_config_set_wrap(
        &config,
        PWM_WRAP
    );


    // Inicializa parado.
    //
    // Ainda não habilita.
    pwm_init(
        motor_pwm_slice,
        &config,
        false
    );


    // --------------------------------------------------
    // Posição neutra dos dois servos
    // --------------------------------------------------

    servo_write_us(
        SERVO_ESQUERDO_PIN,
        SERVO_ESQUERDO_STOP_US
    );


    servo_write_us(
        SERVO_DIREITO_PIN,
        SERVO_DIREITO_STOP_US
    );


    // --------------------------------------------------
    // Agora habilita PWM
    // --------------------------------------------------

    pwm_set_enabled(
        motor_pwm_slice,
        true
    );


    return true;
}


// ======================================================
// INICIALIZA MOTORES
// ======================================================

bool motor_init(void)
{
    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        "          MOTORES / SERVOS\n"
    );

    printf(
        "========================================\n"
    );


    // Evita inicialização duplicada
    if (motor_inicializado)
    {
        printf(
            "[MOTOR] Modulo ja inicializado.\n"
        );

        return true;
    }


    printf(
        "Servo esquerdo: GPIO %d\n",
        SERVO_ESQUERDO_PIN
    );

    printf(
        "Servo direito : GPIO %d\n",
        SERVO_DIREITO_PIN
    );


    // --------------------------------------------------
    // Configuração PWM
    // --------------------------------------------------

    if (!configurar_pwm_servos())
    {
        printf(
            "[MOTOR] ERRO ao configurar PWM.\n"
        );

        return false;
    }


    motor_movendo = false;

    motor_inicializado = true;


    // --------------------------------------------------
    // Garante que começa parado
    // --------------------------------------------------

    servo_write_us(
        SERVO_ESQUERDO_PIN,
        SERVO_ESQUERDO_STOP_US
    );


    servo_write_us(
        SERVO_DIREITO_PIN,
        SERVO_DIREITO_STOP_US
    );


    printf(
        "Neutro esquerdo: %d us\n",
        SERVO_ESQUERDO_STOP_US
    );

    printf(
        "Neutro direito : %d us\n",
        SERVO_DIREITO_STOP_US
    );

    printf(
        "Frente esquerdo: %d us\n",
        SERVO_ESQUERDO_FRENTE_US
    );

    printf(
        "Frente direito : %d us\n",
        SERVO_DIREITO_FRENTE_US
    );

    printf(
        "Tempo por acerto: %d ms\n",
        MOTOR_TEMPO_AVANCO_MS
    );


    printf(
        "Motores inicializados.\n"
    );


    printf(
        "========================================\n\n"
    );


    return true;
}


// ======================================================
// PARA O CARRINHO
// ======================================================

void motor_parar(void)
{
    if (!motor_inicializado)
    {
        return;
    }


    servo_write_us(
        SERVO_ESQUERDO_PIN,
        SERVO_ESQUERDO_STOP_US
    );


    servo_write_us(
        SERVO_DIREITO_PIN,
        SERVO_DIREITO_STOP_US
    );


    motor_movendo = false;


    printf(
        "[MOTOR] Carrinho parado.\n"
    );
}


// ======================================================
// AVANÇA O CARRINHO
// ======================================================

void motor_avancar(void)
{
    if (!motor_inicializado)
    {
        printf(
            "[MOTOR] ERRO: modulo nao inicializado.\n"
        );

        return;
    }


    printf(
        "[MOTOR] Avancando...\n"
    );


    printf(
        "[MOTOR] Esquerdo = %d us | Direito = %d us\n",
        SERVO_ESQUERDO_FRENTE_US,
        SERVO_DIREITO_FRENTE_US
    );


    // --------------------------------------------------
    // Servo esquerdo
    // --------------------------------------------------

    servo_write_us(
        SERVO_ESQUERDO_PIN,
        SERVO_ESQUERDO_FRENTE_US
    );


    // --------------------------------------------------
    // Servo direito
    // --------------------------------------------------

    servo_write_us(
        SERVO_DIREITO_PIN,
        SERVO_DIREITO_FRENTE_US
    );


    motor_movendo = true;


    // --------------------------------------------------
    // Programa a parada
    // --------------------------------------------------

    tempo_parada =
        make_timeout_time_ms(
            MOTOR_TEMPO_AVANCO_MS
        );
}


// ======================================================
// ATUALIZAÇÃO
// ======================================================
//
// Chamar continuamente no main:
//
// motor_update();
//
// Não usamos sleep_ms(800) aqui,
// pois isso bloquearia temporariamente
// Wi-Fi e servidor web.
// ======================================================

void motor_update(void)
{
    if (!motor_inicializado)
    {
        return;
    }


    if (!motor_movendo)
    {
        return;
    }


    // --------------------------------------------------
    // Verifica se chegou no tempo programado
    // --------------------------------------------------

    if (
        absolute_time_diff_us(
            get_absolute_time(),
            tempo_parada
        ) <= 0
    )
    {
        motor_parar();
    }
}


// ======================================================
// FINALIZA
// ======================================================

void motor_deinit(void)
{
    if (!motor_inicializado)
    {
        return;
    }


    printf(
        "[MOTOR] Finalizando modulo...\n"
    );


    motor_parar();


    pwm_set_enabled(
        motor_pwm_slice,
        false
    );


    motor_inicializado = false;

    motor_movendo = false;


    printf(
        "[MOTOR] Modulo finalizado.\n"
    );
}