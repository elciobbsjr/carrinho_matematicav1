#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "game.h"


// ======================================================
// ORGANIZAÇÃO DO BANCO
// ======================================================
//
// 40 questões no total:
//
//  0 a  7 -> Adição
//  8 a 15 -> Subtração
// 16 a 23 -> Multiplicação
// 24 a 31 -> Divisão
// 32 a 39 -> Dobro / Metade
//
// Cada corrida seleciona:
// 1 questão de cada categoria.
//
// Total da corrida:
// 5 questões.
//
// ======================================================


// ======================================================
// BANCO COMPLETO DE QUESTÕES
// ======================================================

static const game_question_t banco[GAME_BANCO_QUESTOES] =
{
    // ==================================================
    // ADIÇÃO
    // Índices 0 a 7
    // ==================================================

    {"5 + 4 = ?", 9},
    {"7 + 5 = ?", 12},
    {"8 + 6 = ?", 14},
    {"9 + 4 = ?", 13},
    {"10 + 7 = ?", 17},
    {"12 + 5 = ?", 17},
    {"6 + 8 = ?", 14},
    {"11 + 9 = ?", 20},


    // ==================================================
    // SUBTRAÇÃO
    // Índices 8 a 15
    // ==================================================

    {"10 - 4 = ?", 6},
    {"12 - 5 = ?", 7},
    {"15 - 7 = ?", 8},
    {"18 - 9 = ?", 9},
    {"20 - 6 = ?", 14},
    {"17 - 5 = ?", 12},
    {"14 - 8 = ?", 6},
    {"19 - 7 = ?", 12},


    // ==================================================
    // MULTIPLICAÇÃO
    // Índices 16 a 23
    //
    // Tabuada até 5
    // ==================================================

    {"2 x 3 = ?", 6},
    {"2 x 5 = ?", 10},
    {"3 x 4 = ?", 12},
    {"3 x 5 = ?", 15},
    {"4 x 2 = ?", 8},
    {"4 x 4 = ?", 16},
    {"5 x 3 = ?", 15},
    {"5 x 5 = ?", 25},


    // ==================================================
    // DIVISÃO
    // Índices 24 a 31
    //
    // Divisões exatas e simples
    // ==================================================

    {"8 / 2 = ?", 4},
    {"10 / 2 = ?", 5},
    {"12 / 3 = ?", 4},
    {"15 / 5 = ?", 3},
    {"16 / 4 = ?", 4},
    {"18 / 3 = ?", 6},
    {"20 / 5 = ?", 4},
    {"25 / 5 = ?", 5},


    // ==================================================
    // DOBRO E METADE
    // Índices 32 a 39
    // ==================================================

    {"Dobro de 4 = ?", 8},
    {"Dobro de 5 = ?", 10},
    {"Dobro de 6 = ?", 12},
    {"Dobro de 8 = ?", 16},

    {"Metade de 10 = ?", 5},
    {"Metade de 12 = ?", 6},
    {"Metade de 16 = ?", 8},
    {"Metade de 20 = ?", 10}
};


// ======================================================
// QUESTÕES DA CORRIDA ATUAL
// ======================================================
//
// Aqui ficam os índices das 5 questões selecionadas.
//
// Exemplo:
//
// corrida[0] = 5
// corrida[1] = 30
// corrida[2] = 18
// corrida[3] = 37
// corrida[4] = 12
//
// ======================================================

static int corrida[GAME_TOTAL_QUESTOES];


// ======================================================
// ESTADO DO JOGO
// ======================================================

static int questao_atual = 0;

static int acertos = 0;

static uint32_t seed_atual = 0;


// ======================================================
// GERADOR PSEUDO-ALEATÓRIO
// ======================================================
//
// Utilizamos Xorshift32.
//
// A característica mais importante aqui é:
//
// MESMA SEED
//      ↓
// MESMA SEQUÊNCIA
//
// Portanto:
//
// Carrinho A -> seed 48271
// Carrinho B -> seed 48271
//
// Os dois terão exatamente a mesma corrida.
//
// ======================================================

static uint32_t rng_estado = 1;


// ======================================================
// Xorshift32
// ======================================================

static uint32_t game_random(void)
{
    uint32_t x = rng_estado;


    x ^= x << 13;

    x ^= x >> 17;

    x ^= x << 5;


    rng_estado = x;


    return x;
}


// ======================================================
// NÚMERO ALEATÓRIO DENTRO DE UM INTERVALO
// ======================================================
//
// Exemplo:
//
// random_intervalo(0, 7)
//
// retorna um número entre:
//
// 0 e 7
//
// ======================================================

static int random_intervalo(
    int minimo,
    int maximo
)
{
    uint32_t quantidade =
        (uint32_t)(
            maximo -
            minimo +
            1
        );


    return
        minimo +
        (int)(
            game_random() %
            quantidade
        );
}


// ======================================================
// EMBARALHA AS 5 QUESTÕES DA CORRIDA
// ======================================================
//
// Fisher-Yates.
//
// A seed também determina a ordem.
//
// ======================================================

static void embaralhar_corrida(void)
{
    for (
        int i = GAME_TOTAL_QUESTOES - 1;
        i > 0;
        i--
    )
    {
        int j =
            (int)(
                game_random() %
                (uint32_t)(i + 1)
            );


        int temp =
            corrida[i];


        corrida[i] =
            corrida[j];


        corrida[j] =
            temp;
    }
}


// ======================================================
// GERA A CORRIDA
// ======================================================
//
// Cada corrida terá:
//
// 1 adição
// 1 subtração
// 1 multiplicação
// 1 divisão
// 1 dobro/metade
//
// Depois as 5 são embaralhadas.
//
// ======================================================

static void gerar_corrida(
    uint32_t seed
)
{
    // --------------------------------------------------
    // Xorshift32 não pode iniciar com zero
    // --------------------------------------------------

    if (seed == 0)
    {
        seed = 1;
    }


    rng_estado = seed;


    // ==================================================
    // 1 - ADIÇÃO
    // ==================================================

    corrida[0] =
        random_intervalo(
            0,
            7
        );


    // ==================================================
    // 2 - SUBTRAÇÃO
    // ==================================================

    corrida[1] =
        random_intervalo(
            8,
            15
        );


    // ==================================================
    // 3 - MULTIPLICAÇÃO
    // ==================================================

    corrida[2] =
        random_intervalo(
            16,
            23
        );


    // ==================================================
    // 4 - DIVISÃO
    // ==================================================

    corrida[3] =
        random_intervalo(
            24,
            31
        );


    // ==================================================
    // 5 - DOBRO / METADE
    // ==================================================

    corrida[4] =
        random_intervalo(
            32,
            39
        );


    // ==================================================
    // EMBARALHA A ORDEM
    // ==================================================

    embaralhar_corrida();
}


// ======================================================
// INICIA NOVA CORRIDA
// ======================================================

void game_start(
    uint32_t seed
)
{
    // --------------------------------------------------
    // Seed zero não é permitida pelo Xorshift32
    // --------------------------------------------------

    if (seed == 0)
    {
        seed = 1;
    }


    seed_atual = seed;


    // --------------------------------------------------
    // Reinicia progresso
    // --------------------------------------------------

    questao_atual = 0;

    acertos = 0;


    // --------------------------------------------------
    // Gera as 5 questões
    // --------------------------------------------------

    gerar_corrida(
        seed
    );


    // ==================================================
    // DEBUG NO MONITOR SERIAL
    // ==================================================

    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        "          NOVA CORRIDA\n"
    );

    printf(
        "========================================\n"
    );


    printf(
        "Seed: %lu\n",
        (unsigned long)seed_atual
    );


    printf(
        "Total de questoes: %d\n",
        GAME_TOTAL_QUESTOES
    );


    printf(
        "Questoes selecionadas:\n"
    );


    for (
        int i = 0;
        i < GAME_TOTAL_QUESTOES;
        i++
    )
    {
        int indice =
            corrida[i];


        printf(
            "%d -> Banco[%02d] -> %s\n",
            i + 1,
            indice,
            banco[indice].pergunta
        );
    }


    printf(
        "========================================\n\n"
    );
}


// ======================================================
// INICIALIZAÇÃO
// ======================================================

void game_init(void)
{
    // ==================================================
    // SEED PADRÃO TEMPORÁRIA
    // ==================================================
    //
    // Enquanto ainda não existe a tela para digitar
    // o código da corrida, usamos uma seed fixa.
    //
    // Depois substituiremos isso por uma seed recebida
    // pela página web.
    //
    // ==================================================

    game_start(
        12345
    );
}


// ======================================================
// RETORNA A QUESTÃO ATUAL
// ======================================================

const game_question_t *game_get_current_question(void)
{
    // --------------------------------------------------
    // Corrida já terminou
    // --------------------------------------------------

    if (
        questao_atual >=
        GAME_TOTAL_QUESTOES
    )
    {
        return NULL;
    }


    // --------------------------------------------------
    // Descobre qual questão do banco foi selecionada
    // --------------------------------------------------

    int indice_banco =
        corrida[questao_atual];


    return
        &banco[indice_banco];
}


// ======================================================
// ÍNDICE INTERNO DA QUESTÃO
// ======================================================
//
// Retorna:
//
// 0
// 1
// 2
// 3
// 4
//
// ======================================================

int game_get_question_index(void)
{
    return
        questao_atual;
}


// ======================================================
// NÚMERO DA QUESTÃO
// ======================================================
//
// Retorna:
//
// 1
// 2
// 3
// 4
// 5
//
// ======================================================

int game_get_question_number(void)
{
    if (
        game_is_finished()
    )
    {
        return
            GAME_TOTAL_QUESTOES;
    }


    return
        questao_atual + 1;
}


// ======================================================
// TOTAL DE QUESTÕES
// ======================================================

int game_get_total_questions(void)
{
    return
        GAME_TOTAL_QUESTOES;
}


// ======================================================
// QUANTIDADE DE ACERTOS
// ======================================================

int game_get_score(void)
{
    return
        acertos;
}


// ======================================================
// PROGRESSO DA CORRIDA
// ======================================================
//
// 0 acertos ->   0%
// 1 acerto  ->  20%
// 2 acertos ->  40%
// 3 acertos ->  60%
// 4 acertos ->  80%
// 5 acertos -> 100%
//
// ======================================================

int game_get_progress(void)
{
    return
        (
            acertos *
            100
        )
        /
        GAME_TOTAL_QUESTOES;
}


// ======================================================
// VERIFICA RESPOSTA
// ======================================================

bool game_check_answer(
    int resposta
)
{
    const game_question_t *questao =
        game_get_current_question();


    // --------------------------------------------------
    // Jogo terminado
    // --------------------------------------------------

    if (
        questao == NULL
    )
    {
        return false;
    }


    // ==================================================
    // RESPOSTA CORRETA
    // ==================================================

    if (
        resposta ==
        questao->resposta
    )
    {
        acertos++;


        questao_atual++;


        return true;
    }


    // ==================================================
    // RESPOSTA INCORRETA
    // ==================================================

    return false;
}


// ======================================================
// VERIFICA SE A CORRIDA TERMINOU
// ======================================================

bool game_is_finished(void)
{
    return
        questao_atual >=
        GAME_TOTAL_QUESTOES;
}


// ======================================================
// RETORNA A SEED DA CORRIDA
// ======================================================

uint32_t game_get_seed(void)
{
    return
        seed_atual;
}


// ======================================================
// REINICIA A CORRIDA
// ======================================================
//
// Mantém a mesma seed.
//
// Portanto:
//
// - mesmas 5 questões
// - mesma ordem
//
// ======================================================

void game_reset(void)
{
    game_start(
        seed_atual
    );
}