/**
 * @file Tipos.h
 * @author Prof. Dr. David Buzatto
 * @brief Definição dos tipos utilizados no jogo.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include <stdbool.h>

#include "raylib/raylib.h"

// ============================================================
// ENUMS
// ============================================================

typedef enum EstadoJogador {
    ESTADO_JOGADOR_PARADO,
    ESTADO_JOGADOR_ANDANDO,
    ESTADO_JOGADOR_ANDANDO_RAPIDO,
    ESTADO_JOGADOR_CORRENDO,
    ESTADO_JOGADOR_PULANDO,
    ESTADO_JOGADOR_PULANDO_RAPIDO,
    ESTADO_JOGADOR_PULANDO_CORRENDO,
} EstadoJogador;

typedef enum EstadoInimigoMotobug {
    ESTADO_INIMIGO_MOTOBUG_ANDANDO,
    ESTADO_INIMIGO_MOTOBUG_MORRENDO,
} EstadoInimigoMotobug;

typedef enum EstadoInimigoBuzzBomber {
    ESTADO_INIMIGO_BUZZBOMBER_VOANDO,
    ESTADO_INIMIGO_BUZZBOMBER_MORRENDO,
} EstadoInimigoBuzzBomber;

typedef enum EstadoInimigoCaranguejo {
    ESTADO_INIMIGO_CARANGUEJO_ANDANDO,
    ESTADO_INIMIGO_CARANGUEJO_MORRENDO,
} EstadoInimigoCaranguejo;

typedef enum TipoInimigo {
    TIPO_INIMIGO_MOTOBUG,
    TIPO_INIMIGO_BUZZBOMBER,
    TIPO_INIMIGO_CARANGUEJO,
    TIPO_INIMIGO_MOSQUITO,
} TipoInimigo;

typedef enum EstadoItemAnel {
    ESTADO_ITEM_ANEL_PARADO,
    ESTADO_ITEM_ANEL_COLETADO,
} EstadoItemAnel;

typedef enum EstadoItemEscudo {
    ESTADO_ITEM_ESCUDO_PARADO,
    ESTADO_ITEM_ESCUDO_COLETADO,
} EstadoItemEscudo;

typedef enum EstadoItemEstrela {
    ESTADO_ITEM_ESTRELA_PARADO,
    ESTADO_ITEM_ESTRELA_COLETADO,
} EstadoItemEstrela;

typedef enum TipoItem {
    TIPO_ITEM_ANEL,
    TIPO_ITEM_ESCUDO,
    TIPO_ITEM_ESTRELA,
    TIPO_ITEM_MOLA,
    TIPO_ITEM_ESPINHO,
} TipoItem;

typedef enum TipoElementoMapa {
    TIPO_ELEMENTO_MAPA_OBSTACULO,
    TIPO_ELEMENTO_MAPA_ITEM,
    TIPO_ELEMENTO_MAPA_INIMIGO,
} TipoElementoMapa;

typedef enum EstadoGameWorld {
    ESTADO_GAMEWORLD_MENU,
    ESTADO_GAMEWORLD_JOGANDO,
    ESTADO_GAMEWORLD_TRANSICAO_FASE,
    ESTADO_GAMEWORLD_CONTINUE,
} EstadoGameWorld;

// ============================================================
// STRUCTS BASE
// ============================================================

typedef struct QuadroAnimacao {
    Rectangle fonte;
    int duracao;
    Rectangle retColisao;
} QuadroAnimacao;

typedef struct Animacao {
    QuadroAnimacao *quadros;
    int quantidadeQuadros;
    int quadroAtual;
    int contadorTempoQuadro;
    bool pararNoUltimoQuadro;
    bool executarUmaVez;
    bool finalizada;
} Animacao;

// ============================================================
// JOGADOR
// ============================================================

typedef struct Jogador {
    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velAndandoRapido;
    float velCorrendo;
    float velPulo;
    float velMaxQueda;

    float aceleracao;
    float desaceleracao;
    float frenagem;

    int quantidadePulos;
    int quantidadeMaxPulos;

    int quantidadeAneis;
    int quantidadeVidas;
    int pontuacao;

    bool invulneravel;
    float tempoInvulnerabilidade;
    float contadorTempoInvulnerabilidade;

    bool piscaPisca;
    float tempoPiscaPisca;
    float contadorTempoPiscaPisca;

    bool freando;
    bool morrendo;

    EstadoJogador estado;
    bool olhandoParaDireita;

    Animacao *animacoes[20];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoAndando;
    Animacao animacaoAndandoRapido;
    Animacao animacaoCorrendo;
    Animacao animacaoPulando;
    Animacao animacaoPulandoRapido;
    Animacao animacaoPulandoCorrendo;

    bool temEscudo;
    bool temEstrela;
    float cronometroEstrela;
} Jogador;

// ============================================================
// INIMIGOS
// ============================================================

typedef struct InimigoMotobug {
    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velMaxQueda;

    EstadoInimigoMotobug estado;
    bool ativo;
    bool olhandoParaDireita;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoAndando;
    Animacao animacaoMorrendo;
} InimigoMotobug;

typedef struct InimigoBuzzBomber {
    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float alturaVoo;

    EstadoInimigoBuzzBomber estado;
    bool ativo;
    bool olhandoParaDireita;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoVoando;
    Animacao animacaoMorrendo;

    bool usaSpriteMosquito;

    bool tiroAtivo;
    Rectangle tiroRet;
    Vector2 tiroVel;
    float contadorTiro;
    float intervaloTiro;
    float tempoVidaTiro;
    float tempoVidaMaxTiro;
} InimigoBuzzBomber;

typedef struct InimigoCaranguejo {
    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velMaxQueda;

    EstadoInimigoCaranguejo estado;
    bool ativo;
    bool olhandoParaDireita;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoAndando;
    Animacao animacaoMorrendo;

    bool tiroAtivo;
    Rectangle tiroRet;
    Vector2 tiroVel;
    float contadorTiro;
    float intervaloTiro;
    float tempoVidaTiro;
    float tempoVidaMaxTiro;
} InimigoCaranguejo;

typedef struct Inimigo {
    void *objeto;
    TipoInimigo tipo;
} Inimigo;

// ============================================================
// ITENS
// ============================================================

typedef struct ItemAnel {
    Rectangle ret;
    Color cor;

    EstadoItemAnel estado;
    bool ativo;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoColetando;
} ItemAnel;

typedef struct ItemEscudo {
    Rectangle ret;
    Color cor;

    EstadoItemEscudo estado;
    bool ativo;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoColetando;
} ItemEscudo;

typedef struct ItemEstrela {
    Rectangle ret;
    Color cor;

    EstadoItemEstrela estado;
    bool ativo;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoColetando;
} ItemEstrela;

typedef struct ItemMola {
    Rectangle ret;
    Color cor;

    bool ativa;
    float tempoAnimacao;
} ItemMola;

typedef struct ItemEspinho {
    Rectangle ret;
    Color cor;

    bool ativo;
} ItemEspinho;

typedef struct Item {
    void *objeto;
    TipoItem tipo;
} Item;

// ============================================================
// MAPA
// ============================================================

typedef struct Obstaculo {
    Rectangle ret;
    Color cor;
    Rectangle fonte;
    Texture2D *textura;
} Obstaculo;

typedef struct ElementoMapa ElementoMapa;
struct ElementoMapa {
    void *objeto;
    TipoElementoMapa tipo;
    ElementoMapa *proximo;
};

typedef struct Mapa {
    ElementoMapa *obstaculos;
    int quantidadeObstaculos;

    ElementoMapa *itens;
    int quantidadeItens;

    ElementoMapa *inimigos;
    int quantidadeInimigos;

    float dimensaoPadraoElementos;
    int linhas;
    int colunas;
} Mapa;

// ============================================================
// GAME WORLD
// ============================================================

typedef struct GameWorld {
    Mapa *mapa;
    Jogador *jogador;

    Camera2D camera;

    float gravidade;
    float tempoFase;

    int faseAtual;
    int faseSelecionada;

    bool mostrandoMensagemFase;
    float tempoMensagemFase;

    EstadoGameWorld estado;

    float tempoTela;
    float tempoContinue;
    float duracaoTransicao;

    int proximaFase;
    int bonusAneis;
    int bonusTempo;
    int bonusTotal;
    int pontuacaoAntesTransicao;
} GameWorld;
