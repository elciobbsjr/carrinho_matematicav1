#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lwip/tcp.h"

#include "webserver.h"
#include "web_interface.h"
#include "game.h"
#include "motor.h"


// ======================================================
// CONFIGURAÇÕES
// ======================================================

#define HTTP_PORT 80

#define HTTP_REQUEST_BUFFER_SIZE 2048


// ======================================================
// SERVIDOR
// ======================================================

static struct tcp_pcb *server_pcb = NULL;


// ======================================================
// ESTADO INDIVIDUAL DE UMA CONEXÃO HTTP
// ======================================================

typedef struct
{
    char request[HTTP_REQUEST_BUFFER_SIZE];

    size_t length;

} http_client_t;


// ======================================================
// ENVIA RESPOSTA HTTP
// ======================================================

static err_t http_send_response(
    struct tcp_pcb *pcb,
    const char *status,
    const char *content_type,
    const char *body
)
{
    if (
        pcb == NULL ||
        status == NULL ||
        content_type == NULL ||
        body == NULL
    )
    {
        return ERR_ARG;
    }


    size_t body_length =
        strlen(body);


    char header[256];


    int header_length =
        snprintf(
            header,
            sizeof(header),

            "HTTP/1.1 %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "Cache-Control: no-store\r\n"
            "\r\n",

            status,
            content_type,
            (unsigned int)body_length
        );


    if (
        header_length <= 0 ||
        header_length >= (int)sizeof(header)
    )
    {
        return ERR_VAL;
    }


    err_t err =
        tcp_write(
            pcb,
            header,
            header_length,
            TCP_WRITE_FLAG_COPY
        );


    if (err != ERR_OK)
    {
        return err;
    }


    if (body_length > 0)
    {
        err =
            tcp_write(
                pcb,
                body,
                body_length,
                TCP_WRITE_FLAG_COPY
            );


        if (err != ERR_OK)
        {
            return err;
        }
    }


    return tcp_output(
        pcb
    );
}


// ======================================================
// RESPOSTA SEM CONTEÚDO
// ======================================================

static err_t http_send_no_content(
    struct tcp_pcb *pcb
)
{
    const char response[] =
        "HTTP/1.1 204 No Content\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";


    err_t err =
        tcp_write(
            pcb,
            response,
            strlen(response),
            TCP_WRITE_FLAG_COPY
        );


    if (err != ERR_OK)
    {
        return err;
    }


    return tcp_output(
        pcb
    );
}


// ======================================================
// ENVIA INTERFACE WEB
// ======================================================

static err_t send_interface(
    struct tcp_pcb *pcb
)
{
    return http_send_response(
        pcb,
        "200 OK",
        "text/html; charset=UTF-8",
        web_interface_get_html()
    );
}


// ======================================================
// ENVIA ESTADO DO JOGO
// ======================================================

static err_t send_game_state(
    struct tcp_pcb *pcb
)
{
    char json[256];


    // --------------------------------------------------
    // Corrida finalizada
    // --------------------------------------------------

    if (game_is_finished())
    {
        snprintf(
            json,
            sizeof(json),

            "{"
                "\"finalizada\":true,"
                "\"numero\":%d,"
                "\"total\":%d,"
                "\"progresso\":100,"
                "\"acertos\":%d"
            "}",

            game_get_total_questions(),
            game_get_total_questions(),
            game_get_score()
        );
    }

    // --------------------------------------------------
    // Corrida em andamento
    // --------------------------------------------------

    else
    {
        const game_question_t *questao =
            game_get_current_question();


        if (questao == NULL)
        {
            return http_send_response(
                pcb,
                "500 Internal Server Error",
                "application/json",
                "{\"erro\":\"questao_indisponivel\"}"
            );
        }


        snprintf(
            json,
            sizeof(json),

            "{"
                "\"finalizada\":false,"
                "\"numero\":%d,"
                "\"total\":%d,"
                "\"progresso\":%d,"
                "\"acertos\":%d,"
                "\"pergunta\":\"%s\""
            "}",

            game_get_question_number(),
            game_get_total_questions(),
            game_get_progress(),
            game_get_score(),
            questao->pergunta
        );
    }


    return http_send_response(
        pcb,
        "200 OK",
        "application/json",
        json
    );
}


// ======================================================
// PROCESSA RESPOSTA MATEMÁTICA
// ======================================================

static err_t process_answer(
    struct tcp_pcb *pcb,
    const char *body
)
{
    // --------------------------------------------------
    // Corpo inexistente
    // --------------------------------------------------

    if (body == NULL)
    {
        return http_send_response(
            pcb,
            "400 Bad Request",
            "application/json",
            "{\"erro\":\"corpo_invalido\"}"
        );
    }


    // --------------------------------------------------
    // Procura campo resposta
    // --------------------------------------------------

    const char *field =
        strstr(
            body,
            "resposta="
        );


    if (field == NULL)
    {
        return http_send_response(
            pcb,
            "400 Bad Request",
            "application/json",
            "{\"erro\":\"resposta_ausente\"}"
        );
    }


    field +=
        strlen(
            "resposta="
        );


    // --------------------------------------------------
    // Converte resposta para inteiro
    // --------------------------------------------------

    char *endptr = NULL;


    long valor =
        strtol(
            field,
            &endptr,
            10
        );


    if (endptr == field)
    {
        return http_send_response(
            pcb,
            "400 Bad Request",
            "application/json",
            "{\"erro\":\"resposta_invalida\"}"
        );
    }


    // --------------------------------------------------
    // Terminal
    // --------------------------------------------------

    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        "        RESPOSTA MATEMATICA\n"
    );

    printf(
        "========================================\n"
    );


    printf(
        "Resposta recebida: %ld\n",
        valor
    );


    // --------------------------------------------------
    // Verifica resposta
    // --------------------------------------------------

    bool correto =
        game_check_answer(
            (int)valor
        );


    // ==================================================
    // RESPOSTA CORRETA
    // ==================================================

    if (correto)
    {
        printf(
            "[MOTOR] Resposta correta. "
            "Acionando carrinho.\n"
        );


        // ----------------------------------------------
        // FAZ O CARRINHO ANDAR
        // ----------------------------------------------

        motor_avancar();


        printf(
            "Resultado: CORRETO\n"
        );


        printf(
            "Progresso: %d/%d\n",
            game_get_score(),
            game_get_total_questions()
        );


        if (game_is_finished())
        {
            printf(
                "CORRIDA FINALIZADA!\n"
            );
        }
    }

    // ==================================================
    // RESPOSTA INCORRETA
    // ==================================================

    else
    {
        printf(
            "Resultado: INCORRETO\n"
        );
    }


    printf(
        "========================================\n"
    );


    // --------------------------------------------------
    // Monta JSON de resposta
    // --------------------------------------------------

    char json[160];


    snprintf(
        json,
        sizeof(json),

        "{"
            "\"correto\":%s,"
            "\"finalizada\":%s,"
            "\"progresso\":%d,"
            "\"acertos\":%d"
        "}",

        correto
            ? "true"
            : "false",

        game_is_finished()
            ? "true"
            : "false",

        game_get_progress(),

        game_get_score()
    );


    // --------------------------------------------------
    // Responde navegador
    // --------------------------------------------------

    return http_send_response(
        pcb,
        "200 OK",
        "application/json",
        json
    );
}


// ======================================================
// OBTÉM CONTENT-LENGTH
// ======================================================

static int get_content_length(
    const char *request
)
{
    const char *header =
        strstr(
            request,
            "Content-Length:"
        );


    if (header == NULL)
    {
        return 0;
    }


    header +=
        strlen(
            "Content-Length:"
        );


    // Ignora espaços
    while (
        *header == ' ' ||
        *header == '\t'
    )
    {
        header++;
    }


    return atoi(
        header
    );
}


// ======================================================
// VERIFICA SE A REQUISIÇÃO ESTÁ COMPLETA
// ======================================================

static bool request_is_complete(
    http_client_t *client
)
{
    char *header_end =
        strstr(
            client->request,
            "\r\n\r\n"
        );


    if (header_end == NULL)
    {
        return false;
    }


    size_t header_size =
        (size_t)(
            header_end -
            client->request
        ) + 4;


    int content_length =
        get_content_length(
            client->request
        );


    return
        client->length >=
        (
            header_size +
            (size_t)content_length
        );
}


// ======================================================
// PROCESSA ROTAS HTTP
// ======================================================

static void process_request(
    struct tcp_pcb *pcb,
    http_client_t *client
)
{
    char *request =
        client->request;


    // ==================================================
    // GET /
    // ==================================================

    if (
        strncmp(
            request,
            "GET / HTTP/",
            11
        ) == 0
    )
    {
        printf(
            "[HTTP] GET /\n"
        );


        send_interface(
            pcb
        );


        return;
    }


    // ==================================================
    // GET /estado
    // ==================================================

    if (
        strncmp(
            request,
            "GET /estado ",
            12
        ) == 0
    )
    {
        printf(
            "[HTTP] GET /estado\n"
        );


        send_game_state(
            pcb
        );


        return;
    }


    // ==================================================
    // GET /favicon.ico
    // ==================================================

    if (
        strncmp(
            request,
            "GET /favicon.ico ",
            17
        ) == 0
    )
    {
        http_send_no_content(
            pcb
        );


        return;
    }


    // ==================================================
    // POST /responder
    // ==================================================

    if (
        strncmp(
            request,
            "POST /responder ",
            16
        ) == 0
    )
    {
        char *body =
            strstr(
                request,
                "\r\n\r\n"
            );


        if (body != NULL)
        {
            body += 4;
        }


        process_answer(
            pcb,
            body
        );


        return;
    }


    // ==================================================
    // QUALQUER OUTRA ROTA
    // ==================================================

    http_send_response(
        pcb,
        "404 Not Found",
        "text/plain; charset=UTF-8",
        "404"
    );
}


// ======================================================
// CALLBACK DE ERRO DA CONEXÃO
// ======================================================

static void client_error_callback(
    void *arg,
    err_t err
)
{
    http_client_t *client =
        (http_client_t *)arg;


    if (client != NULL)
    {
        free(
            client
        );
    }


    // Evita poluir o terminal com requisições externas
    // redirecionadas pelo DNS.
    (void)err;
}


// ======================================================
// CALLBACK DE RECEBIMENTO
// ======================================================

static err_t receive_callback(
    void *arg,
    struct tcp_pcb *pcb,
    struct pbuf *p,
    err_t err
)
{
    http_client_t *client =
        (http_client_t *)arg;


    // --------------------------------------------------
    // Cliente encerrou conexão
    // --------------------------------------------------

    if (p == NULL)
    {
        if (client != NULL)
        {
            free(
                client
            );
        }


        tcp_arg(
            pcb,
            NULL
        );


        tcp_close(
            pcb
        );


        return ERR_OK;
    }


    // --------------------------------------------------
    // Erro
    // --------------------------------------------------

    if (err != ERR_OK)
    {
        pbuf_free(
            p
        );


        return err;
    }


    // --------------------------------------------------
    // Contexto inexistente
    // --------------------------------------------------

    if (client == NULL)
    {
        pbuf_free(
            p
        );


        tcp_abort(
            pcb
        );


        return ERR_ABRT;
    }


    // ==================================================
    // VERIFICA ESPAÇO NO BUFFER
    // ==================================================

    size_t disponivel =
        HTTP_REQUEST_BUFFER_SIZE -
        client->length -
        1;


    if (p->tot_len > disponivel)
    {
        printf(
            "[HTTP] Requisicao grande demais.\n"
        );


        pbuf_free(
            p
        );


        free(
            client
        );


        tcp_arg(
            pcb,
            NULL
        );


        tcp_abort(
            pcb
        );


        return ERR_ABRT;
    }


    // ==================================================
    // COPIA PACOTE PARA BUFFER
    // ==================================================

    pbuf_copy_partial(
        p,

        client->request +
        client->length,

        p->tot_len,

        0
    );


    client->length +=
        p->tot_len;


    client->request[
        client->length
    ] = '\0';


    // --------------------------------------------------
    // Informa ao lwIP que recebeu
    // --------------------------------------------------

    tcp_recved(
        pcb,
        p->tot_len
    );


    pbuf_free(
        p
    );


    // ==================================================
    // AGUARDA A REQUISIÇÃO COMPLETA
    // ==================================================

    if (
        !request_is_complete(
            client
        )
    )
    {
        return ERR_OK;
    }


    // ==================================================
    // PROCESSA REQUISIÇÃO
    // ==================================================

    process_request(
        pcb,
        client
    );


    // ==================================================
    // LIBERA CONTEXTO
    // ==================================================

    free(
        client
    );


    tcp_arg(
        pcb,
        NULL
    );


    tcp_recv(
        pcb,
        NULL
    );


    tcp_err(
        pcb,
        NULL
    );


    // ==================================================
    // FECHA CONEXÃO
    // ==================================================

    err_t close_err =
        tcp_close(
            pcb
        );


    if (close_err != ERR_OK)
    {
        tcp_abort(
            pcb
        );


        return ERR_ABRT;
    }


    return ERR_OK;
}


// ======================================================
// CALLBACK DE NOVA CONEXÃO
// ======================================================

static err_t accept_callback(
    void *arg,
    struct tcp_pcb *new_pcb,
    err_t err
)
{
    (void)arg;


    if (
        err != ERR_OK ||
        new_pcb == NULL
    )
    {
        return ERR_VAL;
    }


    // --------------------------------------------------
    // Cria contexto para o cliente
    // --------------------------------------------------

    http_client_t *client =
        calloc(
            1,
            sizeof(http_client_t)
        );


    if (client == NULL)
    {
        tcp_abort(
            new_pcb
        );


        return ERR_ABRT;
    }


    // --------------------------------------------------
    // Configura callbacks
    // --------------------------------------------------

    tcp_arg(
        new_pcb,
        client
    );


    tcp_recv(
        new_pcb,
        receive_callback
    );


    tcp_err(
        new_pcb,
        client_error_callback
    );


    return ERR_OK;
}


// ======================================================
// INICIALIZA SERVIDOR WEB
// ======================================================

bool webserver_init(void)
{
    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        "          SERVIDOR WEB HTTP\n"
    );

    printf(
        "========================================\n"
    );


    // --------------------------------------------------
    // Inicializa jogo
    // --------------------------------------------------

    game_init();


    // --------------------------------------------------
    // Cria PCB TCP
    // --------------------------------------------------

    server_pcb =
        tcp_new();


    if (server_pcb == NULL)
    {
        printf(
            "ERRO: falha ao criar servidor TCP.\n"
        );


        return false;
    }


    // --------------------------------------------------
    // Porta 80
    // --------------------------------------------------

    err_t err =
        tcp_bind(
            server_pcb,
            IP_ADDR_ANY,
            HTTP_PORT
        );


    if (err != ERR_OK)
    {
        printf(
            "ERRO: porta 80 indisponivel.\n"
        );


        tcp_close(
            server_pcb
        );


        server_pcb =
            NULL;


        return false;
    }


    // --------------------------------------------------
    // Modo LISTEN
    // --------------------------------------------------

    struct tcp_pcb *listen_pcb =
        tcp_listen(
            server_pcb
        );


    if (listen_pcb == NULL)
    {
        printf(
            "ERRO: falha no modo LISTEN.\n"
        );


        tcp_close(
            server_pcb
        );


        server_pcb =
            NULL;


        return false;
    }


    server_pcb =
        listen_pcb;


    // --------------------------------------------------
    // Aceita novas conexões
    // --------------------------------------------------

    tcp_accept(
        server_pcb,
        accept_callback
    );


    printf(
        "Servidor HTTP iniciado.\n"
    );


    printf(
        "Acesse: http://192.168.4.1\n"
    );


    printf(
        "========================================\n\n"
    );


    return true;
}


// ======================================================
// FINALIZA SERVIDOR WEB
// ======================================================

void webserver_deinit(void)
{
    if (server_pcb != NULL)
    {
        printf(
            "[HTTP] Encerrando servidor.\n"
        );


        tcp_close(
            server_pcb
        );


        server_pcb =
            NULL;
    }
}