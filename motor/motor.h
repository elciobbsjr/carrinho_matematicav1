#ifndef MOTOR_H
#define MOTOR_H

#include <stdbool.h>

// Inicializa os dois servos 360°
bool motor_init(void);

// Inicia um avanço
void motor_avancar(void);

// Deve ser chamada continuamente no loop principal
void motor_update(void);

// Para imediatamente
void motor_parar(void);

// Finaliza o módulo
void motor_deinit(void);

#endif