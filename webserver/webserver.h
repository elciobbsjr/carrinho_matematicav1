#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdbool.h>

// Inicializa o servidor HTTP local
bool webserver_init(void);

// Finaliza o servidor HTTP
void webserver_deinit(void);

#endif