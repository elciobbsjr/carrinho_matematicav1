#ifndef GAME_H
#define GAME_H


#include <stdbool.h>
#include <stdint.h>


// ======================================================
// CONFIGURAÇÕES DO JOGO
// ======================================================

// Quantidade de questões em cada corrida
#define GAME_TOTAL_QUESTOES 5

// Quantidade total de questões disponíveis no banco
#define GAME_BANCO_QUESTOES 40


// ======================================================
// ESTRUTURA DE UMA QUESTÃO
// ======================================================

typedef struct
{
    // Texto exibido na interface
    const char *pergunta;

    // Resposta correta
    int resposta;

} game_question_t;


// ======================================================
// INICIALIZAÇÃO DO JOGO
// ======================================================

// Inicializa o jogo usando uma seed padrão.
//
// Essa função é chamada na inicialização do sistema.
//
// Depois, quando implementarmos a tela para escolher
// a seed, game_start() será usado para iniciar a corrida.
void game_init(void);


// Inicia uma nova corrida utilizando uma seed específica.
//
// A mesma seed gera:
// - as mesmas 5 questões
// - na mesma ordem
//
// Portanto, dois carrinhos utilizando a mesma seed
// terão exatamente a mesma corrida.
void game_start(uint32_t seed);


// ======================================================
// QUESTÃO ATUAL
// ======================================================

// Retorna a questão atual.
//
// Retorna NULL caso a corrida já tenha terminado.
const game_question_t *game_get_current_question(void);


// Retorna o índice interno da questão.
//
// Valores:
// 0, 1, 2, 3 ou 4
int game_get_question_index(void);


// Retorna o número da questão exibido ao aluno.
//
// Valores:
// 1, 2, 3, 4 ou 5
int game_get_question_number(void);


// Retorna a quantidade total de questões da corrida.
//
// Atualmente:
// 5
int game_get_total_questions(void);


// ======================================================
// PROGRESSO
// ======================================================

// Retorna a quantidade de respostas corretas.
//
// Valores:
// 0 a 5
int game_get_score(void);


// Retorna o progresso da corrida em porcentagem.
//
// Exemplos:
//
// 0 acertos  -> 0%
// 1 acerto   -> 20%
// 2 acertos  -> 40%
// 3 acertos  -> 60%
// 4 acertos  -> 80%
// 5 acertos  -> 100%
int game_get_progress(void);


// ======================================================
// RESPOSTA
// ======================================================

// Verifica a resposta informada pelo aluno.
//
// Retorna:
//
// true  -> resposta correta
// false -> resposta incorreta
//
// Quando a resposta estiver correta,
// o jogo avança automaticamente para
// a próxima questão.
bool game_check_answer(int resposta);


// ======================================================
// ESTADO DA CORRIDA
// ======================================================

// Retorna true quando as 5 questões
// já tiverem sido respondidas corretamente.
bool game_is_finished(void);


// ======================================================
// SEED DA CORRIDA
// ======================================================

// Retorna a seed usada na corrida atual.
//
// Essa seed será importante para garantir
// que dois carrinhos recebam exatamente
// as mesmas questões.
uint32_t game_get_seed(void);


// ======================================================
// RESET
// ======================================================

// Reinicia a corrida utilizando
// a mesma seed atual.
//
// Isso faz com que a sequência de questões
// seja exatamente a mesma.
void game_reset(void);


#endif