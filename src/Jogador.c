/**
 * @file Jogador.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Jogador.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoMotobug.h"
#include "Item.h"
#include "ItemAnel.h"
#include "ItemEscudo.h"
#include "ItemEstrela.h"
#include "ItemMola.h"
#include "ItemEspinho.h"
#include "Macros.h"
#include "Jogador.h"
#include "ResourceManager.h"
#include "Tipos.h"
#include "InimigoBuzzBomber.h"
#include "InimigoCaranguejo.h"

static void desenharQuadroAnimacaoJogador( Jogador *j, QuadroAnimacao *qa, Color tonalidade );
static void desenharParticulasVelocidadeJogador( Jogador *j );
static QuadroAnimacao *getQuadroAnimacaoAtualJogador( Jogador *j );
static Animacao *getAnimacaoAtualJogador( Jogador *j );

static void resolverColisaoJogadorObstaculosMapaX( Jogador *j, Mapa *mapa );
static void resolverColisaoJogadorObstaculosMapaY( Jogador *j, Mapa *mapa );

static void resolverColisaoJogadorItensMapa( Jogador *j, Mapa *mapa );
static void resolverColisaoJogadorInimigosMapa( Jogador *j, Mapa *mapa );

static const bool MOSTRAR_RETANGULOS = false;

/**
 * @brief Cria uma instância alocada dinamicamente da struct Jogador.
 */
Jogador *criarJogador( float x, float y, float w, float h ) {

    Jogador *novoJogador = (Jogador*) malloc( sizeof( Jogador ) );

    novoJogador->ret.x = x;
    novoJogador->ret.y = y;
    novoJogador->ret.width = w;
    novoJogador->ret.height = h;
    novoJogador->vel = (Vector2) { 0 };

    novoJogador->cor = BLUE;

    novoJogador->velAndando = 200;
    novoJogador->velAndandoRapido = 400;
    novoJogador->velCorrendo = 800;
    novoJogador->velPulo = -550;
    novoJogador->velMaxQueda = 600;

    novoJogador->aceleracao = 200;
    novoJogador->desaceleracao = 400;
    novoJogador->frenagem = 1800;

    novoJogador->quantidadePulos = 0;
    novoJogador->quantidadeMaxPulos = 2;

    novoJogador->quantidadeAneis = 0;
    novoJogador->quantidadeVidas = 3;

    novoJogador->temEscudo = false;
    novoJogador->temEstrela = false;
    novoJogador->cronometroEstrela = 0.0f;

    novoJogador->invulneravel = false;
    novoJogador->tempoInvulnerabilidade = 3.0f;
    novoJogador->contadorTempoInvulnerabilidade = 0.0f;

    novoJogador->piscaPisca = false;
    novoJogador->tempoPiscaPisca = 0.05f;
    novoJogador->contadorTempoPiscaPisca = 0.0f;

    novoJogador->freando = false;
    novoJogador->morrendo = false;

    novoJogador->estado = ESTADO_JOGADOR_PARADO;
    novoJogador->olhandoParaDireita = true;

    int quantidadeAnimacoes = 0;

    novoJogador->animacaoParado.quantidadeQuadros = 1;
    novoJogador->animacaoParado.quadroAtual = 0;
    novoJogador->animacaoParado.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoParado.pararNoUltimoQuadro = false;
    novoJogador->animacaoParado.executarUmaVez = false;
    novoJogador->animacaoParado.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoParado, novoJogador->animacaoParado.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoParado.quadros,
        novoJogador->animacaoParado.quantidadeQuadros,
        1000,
        24, 251,
        48, 48,
        4,
        false,
        (Rectangle) {
            32, 20, 42, 76
        }
    );

    novoJogador->animacaoAndando.quantidadeQuadros = 8;
    novoJogador->animacaoAndando.quadroAtual = 0;
    novoJogador->animacaoAndando.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoAndando.pararNoUltimoQuadro = false;
    novoJogador->animacaoAndando.executarUmaVez = false;
    novoJogador->animacaoAndando.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoAndando, novoJogador->animacaoAndando.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoAndando.quadros,
        novoJogador->animacaoAndando.quantidadeQuadros,
        80,
        664, 324,
        48, 48,
        4,
        false,
        (Rectangle) {
            32, 20, 42, 76
        }
    );

    novoJogador->animacaoAndandoRapido.quantidadeQuadros = 8;
    novoJogador->animacaoAndandoRapido.quadroAtual = 0;
    novoJogador->animacaoAndandoRapido.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoAndandoRapido.pararNoUltimoQuadro = false;
    novoJogador->animacaoAndandoRapido.executarUmaVez = false;
    novoJogador->animacaoAndandoRapido.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoAndandoRapido, novoJogador->animacaoAndandoRapido.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoAndandoRapido.quadros,
        novoJogador->animacaoAndandoRapido.quantidadeQuadros,
        40,
        664, 324,
        48, 48,
        4,
        false,
        (Rectangle) {
            32, 20, 42, 76
        }
    );

    novoJogador->animacaoCorrendo.quantidadeQuadros = 4;
    novoJogador->animacaoCorrendo.quadroAtual = 0;
    novoJogador->animacaoCorrendo.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoCorrendo.pararNoUltimoQuadro = false;
    novoJogador->animacaoCorrendo.executarUmaVez = false;
    novoJogador->animacaoCorrendo.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoCorrendo, novoJogador->animacaoCorrendo.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoCorrendo.quadros,
        novoJogador->animacaoCorrendo.quantidadeQuadros,
        20,
        24, 397,
        48, 48,
        4,
        false,
        (Rectangle) {
            32, 20, 42, 76
        }
    );

    novoJogador->animacaoPulando.quantidadeQuadros = 4;
    novoJogador->animacaoPulando.quadroAtual = 0;
    novoJogador->animacaoPulando.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulando.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulando.executarUmaVez = false;
    novoJogador->animacaoPulando.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoPulando, novoJogador->animacaoPulando.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoPulando.quadros,
        novoJogador->animacaoPulando.quantidadeQuadros,
        40,
        248, 397,
        48, 48,
        4,
        false,
        (Rectangle) {
            32, 46, 42, 50
        }
    );

    novoJogador->animacaoPulandoRapido.quantidadeQuadros = 4;
    novoJogador->animacaoPulandoRapido.quadroAtual = 0;
    novoJogador->animacaoPulandoRapido.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulandoRapido.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulandoRapido.executarUmaVez = false;
    novoJogador->animacaoPulandoRapido.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoPulandoRapido, novoJogador->animacaoPulandoRapido.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoJogador->animacaoPulandoRapido.quadros,
        novoJogador->animacaoPulandoRapido.quantidadeQuadros,
        25,
        248, 397,
        48, 48,
        4,
        false,
        (Rectangle) {
            32, 46, 42, 50
        }
    );

    novoJogador->animacaoPulandoCorrendo.quantidadeQuadros = 4;
    novoJogador->animacaoPulandoCorrendo.quadroAtual = 0;
    novoJogador->animacaoPulandoCorrendo.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulandoCorrendo.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulandoCorrendo.executarUmaVez = false;
    novoJogador->animacaoPulandoCorrendo.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoPulandoCorrendo, novoJogador->animacaoPulandoCorrendo.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoJogador->animacaoPulandoCorrendo.quadros,
        novoJogador->animacaoPulandoCorrendo.quantidadeQuadros,
        15,
        248, 397,
        48, 48,
        4,
        false,
        (Rectangle) {
            32, 46, 42, 50
        }
    );

    novoJogador->animacoes[ESTADO_JOGADOR_PARADO] = &novoJogador->animacaoParado; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_ANDANDO] = &novoJogador->animacaoAndando; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_ANDANDO_RAPIDO] = &novoJogador->animacaoAndandoRapido; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_CORRENDO] = &novoJogador->animacaoCorrendo; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO] = &novoJogador->animacaoPulando; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO_RAPIDO] = &novoJogador->animacaoPulandoRapido; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO_CORRENDO] = &novoJogador->animacaoPulandoCorrendo; quantidadeAnimacoes++;
    novoJogador->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoJogador;

}

/**
 * @brief Destrói um objeto Jogador e libera seus recursos.
 */
void destruirJogador( Jogador *j ) {

    if ( j != NULL ) {

        for ( int i = 0; i < j->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( j->animacoes[i] );
        }

        free( j );

    }

}

/**
 * @brief Lê a entrada do usuário e atualiza as velocidades do jogador.
 */
void entradaJogador( Jogador *j, float delta ) {

    if ( j->morrendo ) return;

    EstadoJogador estadoAnterior = j->estado;

    bool direitaDown  = IsKeyDown( KEY_RIGHT ) || IsKeyDown( KEY_D ) || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT ) );
    bool esquerdaDown = IsKeyDown( KEY_LEFT ) || IsKeyDown( KEY_A ) || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_LEFT_FACE_LEFT ) );
    bool puloPressed  = IsKeyPressed( KEY_SPACE ) || IsKeyPressed( KEY_W ) || IsKeyPressed( KEY_UP ) || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN ) );

    if ( direitaDown ) {

        if ( j->vel.x < 0 ) {

            j->vel.x += j->frenagem * delta;

            if ( !j->freando && j->estado == ESTADO_JOGADOR_CORRENDO ) {
                PlaySound( rm.somFrenagem );
                j->freando = true;
            }

            if ( j->vel.x > 0 ) {
                j->vel.x = 0;
                j->freando = false;
            }

        } else {

            j->vel.x += j->aceleracao * delta;

            if ( j->vel.x > j->velCorrendo ) {
                j->vel.x = j->velCorrendo;
            }

        }

        j->olhandoParaDireita = true;

    } else if ( esquerdaDown ) {

        if ( j->vel.x > 0 ) {

            j->vel.x -= j->frenagem * delta;

            if ( !j->freando && j->estado == ESTADO_JOGADOR_CORRENDO ) {
                PlaySound( rm.somFrenagem );
                j->freando = true;
            }

            if ( j->vel.x < 0 ) {
                j->vel.x = 0;
                j->freando = false;
            }

        } else {

            j->vel.x -= j->aceleracao * delta;

            if ( j->vel.x < -j->velCorrendo ) {
                j->vel.x = -j->velCorrendo;
            }

        }

        j->olhandoParaDireita = false;

    } else {

        if ( j->vel.x > 0 ) {

            j->vel.x -= j->desaceleracao * delta;

            if ( j->vel.x < 0 ) {
                j->vel.x = 0;
            }

        } else if ( j->vel.x < 0 ) {

            j->vel.x += j->desaceleracao * delta;

            if ( j->vel.x > 0 ) {
                j->vel.x = 0;
            }

        }

    }

    float absVelX = fabsf( j->vel.x );

    if ( j->quantidadePulos > 0 ) {

        if ( absVelX <= j->velAndando ) {
            j->estado = ESTADO_JOGADOR_PULANDO;
        } else if ( absVelX <= j->velAndandoRapido ) {
            j->estado = ESTADO_JOGADOR_PULANDO_RAPIDO;
        } else {
            j->estado = ESTADO_JOGADOR_PULANDO_CORRENDO;
        }

    } else if ( absVelX < 1.0f ) {

        j->estado = ESTADO_JOGADOR_PARADO;

    } else if ( absVelX <= j->velAndando ) {

        j->estado = ESTADO_JOGADOR_ANDANDO;

    } else if ( absVelX <= j->velAndandoRapido ) {

        j->estado = ESTADO_JOGADOR_ANDANDO_RAPIDO;

    } else {

        j->estado = ESTADO_JOGADOR_CORRENDO;

    }

    if ( puloPressed && j->quantidadePulos < j->quantidadeMaxPulos ) {
        j->vel.y = j->velPulo;
        j->quantidadePulos++;
        PlaySound( rm.somPulo );
    }

    if ( estadoAnterior == ESTADO_JOGADOR_ANDANDO && j->estado == ESTADO_JOGADOR_ANDANDO_RAPIDO ) {
        sincronizarAnimacao( &j->animacaoAndandoRapido, &j->animacaoAndando );
    } else if ( estadoAnterior == ESTADO_JOGADOR_ANDANDO_RAPIDO && j->estado == ESTADO_JOGADOR_ANDANDO ) {
        sincronizarAnimacao( &j->animacaoAndando, &j->animacaoAndandoRapido );
    }

}

/**
 * @brief Aplica física e resolve colisões do jogador com o mundo.
 */
void atualizarJogador( Jogador *j, GameWorld *gw, float delta ) {

    /*
     * Estrela tem prioridade sobre a invulnerabilidade comum.
     * Antes estava misturando 10s de estrela com 3s de invulnerabilidade,
     * então a estrela podia acabar cedo demais.
     */
    if ( j->temEstrela ) {

        j->cronometroEstrela -= delta;
        j->invulneravel = true;

        j->contadorTempoPiscaPisca += delta;

        if ( j->contadorTempoPiscaPisca >= j->tempoPiscaPisca ) {
            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = !j->piscaPisca;
        }

        if ( j->cronometroEstrela <= 0.0f ) {

            j->temEstrela = false;
            j->cronometroEstrela = 0.0f;

            j->invulneravel = false;
            j->contadorTempoInvulnerabilidade = 0.0f;

            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = false;

        }

    } else if ( j->invulneravel ) {

        j->contadorTempoPiscaPisca += delta;

        if ( j->contadorTempoPiscaPisca >= j->tempoPiscaPisca ) {
            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = !j->piscaPisca;
        }

        j->contadorTempoInvulnerabilidade += delta;

        if ( j->contadorTempoInvulnerabilidade >= j->tempoInvulnerabilidade ) {

            j->contadorTempoInvulnerabilidade = 0.0f;
            j->invulneravel = false;

            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = false;

        }

    }

    Animacao *animacaoAtual = getAnimacaoAtualJogador( j );
    atualizarAnimacao( animacaoAtual, delta );

    j->ret.x += j->vel.x * delta;
    if ( !j->morrendo ) resolverColisaoJogadorObstaculosMapaX( j, gw->mapa );

    j->vel.y += gw->gravidade * delta;
    if ( j->vel.y > j->velMaxQueda ) j->vel.y = j->velMaxQueda;

    j->ret.y += j->vel.y * delta;
    if ( !j->morrendo ) resolverColisaoJogadorObstaculosMapaY( j, gw->mapa );

    if ( !j->morrendo ) resolverColisaoJogadorItensMapa( j, gw->mapa );
    if ( !j->morrendo ) resolverColisaoJogadorInimigosMapa( j, gw->mapa );

    if ( j->morrendo && j->ret.y > 1500.0f ) { 
        
        j->ret.x = 100.0f; 
        j->ret.y = 100.0f; 
        
        j->vel.x = 0;
        j->vel.y = 0;
        j->morrendo = false;
        j->estado = ESTADO_JOGADOR_PARADO;
        
        j->temEscudo = false;
        j->temEstrela = false;
        j->cronometroEstrela = 0.0f;

        j->invulneravel = true;
        j->contadorTempoInvulnerabilidade = 0.0f;
        j->contadorTempoPiscaPisca = 0.0f;
        j->piscaPisca = false;

        if ( j->quantidadeVidas < 0 ) {
            j->quantidadeVidas = 3; 
            j->pontuacao = 0;       
        }

    }

}

/**
 * @brief Desenha o jogador.
 */
void desenharJogador( Jogador *j ) {

    desenharParticulasVelocidadeJogador( j );

    if ( !j->piscaPisca ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        Color tonalidade = WHITE;

        if ( j->temEstrela ) {
            tonalidade = ( (int)( GetTime() * 12.0 ) % 2 == 0 ) ? YELLOW : WHITE;
        }

        desenharQuadroAnimacaoJogador( j, qa, tonalidade );

    }

    /*
     * Feedback visual simples do escudo.
     * Não depende de sprite novo.
     */
    if ( j->temEscudo && !j->temEstrela && !j->morrendo ) {

        Vector2 centroEscudo = {
            j->ret.x + j->ret.width / 2.0f,
            j->ret.y + j->ret.height / 2.0f
        };

        float raio = ( j->ret.width > j->ret.height ? j->ret.width : j->ret.height ) * 0.52f;

        DrawCircleV( centroEscudo, raio, Fade( SKYBLUE, 0.18f ) );
        DrawCircleLines( (int) centroEscudo.x, (int) centroEscudo.y, raio, SKYBLUE );
        DrawCircleLines( (int) centroEscudo.x, (int) centroEscudo.y, raio - 4.0f, BLUE );

    }

    if ( MOSTRAR_RETANGULOS ) {
        DrawRectangleRec( j->ret, Fade( j->cor, 0.5f ) );
        DrawRectangleLines( j->ret.x, j->ret.y, j->ret.width, j->ret.height, BLACK );
    }

}

static void desenharParticulasVelocidadeJogador( Jogador *j ) {

    if ( j->morrendo ) return;

    float velocidade = fabsf( j->vel.x );

    if ( velocidade < j->velAndandoRapido + 80.0f ) return;

    float intensidade = ( velocidade - j->velAndandoRapido ) / ( j->velCorrendo - j->velAndandoRapido );

    if ( intensidade < 0.0f ) intensidade = 0.0f;
    if ( intensidade > 1.0f ) intensidade = 1.0f;

    float direcao = j->vel.x >= 0.0f ? -1.0f : 1.0f;
    float baseX = j->ret.x + j->ret.width / 2.0f + direcao * 30.0f;
    float baseY = j->ret.y + j->ret.height * 0.66f;
    float tempo = (float)GetTime();
    int quantidade = 2 + (int)( intensidade * 3.0f );

    for ( int i = 0; i < quantidade; i++ ) {

        float atraso = (float)i * 14.0f;
        float oscilacao = sinf( tempo * 9.0f + (float)i * 1.7f ) * 4.0f;
        float x = baseX + direcao * atraso;
        float y = baseY + oscilacao - (float)( i % 2 ) * 4.0f;
        float tamanho = 8.0f + intensidade * 10.0f - i * 1.4f;
        float alpha = ( 0.22f - i * 0.035f ) * intensidade;

        if ( alpha <= 0.0f ) continue;

        DrawLineEx(
            (Vector2){ x, y },
            (Vector2){ x + direcao * tamanho, y + oscilacao * 0.16f },
            1.0f + intensidade,
            Fade( SKYBLUE, alpha )
        );

        DrawCircleV(
            (Vector2){ x + direcao * ( tamanho + 4.0f ), y },
            1.5f + intensidade,
            Fade( WHITE, alpha * 0.55f )
        );

    }

}

static void desenharQuadroAnimacaoJogador( Jogador *j, QuadroAnimacao *qa, Color tonalidade ) {

    if ( qa != NULL ) {

        DrawTexturePro(
            rm.texturaJogador,
            (Rectangle) {
                qa->fonte.x,
                qa->fonte.y,
                j->olhandoParaDireita ? qa->fonte.width : -qa->fonte.width,
                qa->fonte.height
            },
            j->ret,
            (Vector2) { 0 },
            0.0f,
            tonalidade
        );

        if ( MOSTRAR_RETANGULOS ) {

            float xDesenho = j->olhandoParaDireita
                ? j->ret.x + qa->retColisao.x
                : j->ret.x + j->ret.width - qa->retColisao.x - qa->retColisao.width;

            float yDesenho = j->ret.y + qa->retColisao.y;

            DrawRectangle(
                xDesenho,
                yDesenho,
                qa->retColisao.width,
                qa->retColisao.height,
                Fade( GREEN, 0.5f )
            );

        }

    }

}

static QuadroAnimacao *getQuadroAnimacaoAtualJogador( Jogador *j ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualJogador( j ) );
}

static Animacao *getAnimacaoAtualJogador( Jogador *j ) {
    return j->animacoes[j->estado];
}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo X.
 */
static void resolverColisaoJogadorObstaculosMapaX( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;

        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( CheckCollisionRecs( retColCalculado, o->ret ) ) {

            if ( retColCalculado.x + retColCalculado.width / 2 < o->ret.x + o->ret.width / 2 ) {
                j->ret.x = o->ret.x - qa->retColisao.width - deslocamentoX;
            } else {
                j->ret.x = o->ret.x + o->ret.width - deslocamentoX;
            }

            j->vel.x = 0;

        }

        el = el->proximo;

    }

}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo Y.
 */
static void resolverColisaoJogadorObstaculosMapaY( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;

        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( CheckCollisionRecs( retColCalculado, o->ret ) ) {

            if ( retColCalculado.y + retColCalculado.height / 2 < o->ret.y + o->ret.height / 2 ) {
                j->ret.y = o->ret.y - qa->retColisao.height - deslocamentoY;
                j->quantidadePulos = 0;
            } else {
                j->ret.y = o->ret.y + o->ret.height - deslocamentoY;
            }

            j->vel.y = 0;

        }

        el = el->proximo;

    }

}

static void resolverColisaoJogadorItensMapa( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->itens;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;

        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Item *item = (Item*) el->objeto;

        if ( item->tipo == TIPO_ITEM_ANEL ) {

            ItemAnel *itemAnel = (ItemAnel*) item->objeto;

            if ( !itemAnel->ativo || itemAnel->estado == ESTADO_ITEM_ANEL_COLETADO ) {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemAnel( itemAnel );
            
            Rectangle retColItemCalculado = {
                itemAnel->ret.x + qaItem->retColisao.x,
                itemAnel->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width,
                qaItem->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColItemCalculado ) ) {
                itemAnel->estado = ESTADO_ITEM_ANEL_COLETADO;
                itemAnel->ativo = false;
                j->quantidadeAneis++;
                PlaySound( rm.somAnel );
            }

        } else if ( item->tipo == TIPO_ITEM_ESCUDO ) {

            ItemEscudo *itemEscudo = (ItemEscudo*) item->objeto;

            if ( !itemEscudo->ativo || itemEscudo->estado == ESTADO_ITEM_ESCUDO_COLETADO ) {
                el = el->proximo;
                continue;
            }

            if ( CheckCollisionRecs( retColCalculado, itemEscudo->ret ) ) {

                itemEscudo->estado = ESTADO_ITEM_ESCUDO_COLETADO;
                itemEscudo->ativo = false;

                j->temEscudo = true;
                j->temEstrela = false;
                j->cronometroEstrela = 0.0f;

                PlaySound( rm.somAnel );

            }

        } else if ( item->tipo == TIPO_ITEM_ESTRELA ) {

            ItemEstrela *itemEstrela = (ItemEstrela*) item->objeto;

            if ( !itemEstrela->ativo || itemEstrela->estado == ESTADO_ITEM_ESTRELA_COLETADO ) {
                el = el->proximo;
                continue;
            }

            if ( CheckCollisionRecs( retColCalculado, itemEstrela->ret ) ) {

                itemEstrela->estado = ESTADO_ITEM_ESTRELA_COLETADO;
                itemEstrela->ativo = false;

                j->temEscudo = false;
                j->temEstrela = true;
                j->cronometroEstrela = 10.0f;

                j->invulneravel = true;
                j->contadorTempoInvulnerabilidade = 0.0f;
                j->contadorTempoPiscaPisca = 0.0f;
                j->piscaPisca = false;

                PlaySound( rm.somAnel );

            }

        } else if ( item->tipo == TIPO_ITEM_MOLA ) {

            ItemMola *itemMola = (ItemMola*) item->objeto;

            if ( itemMola == NULL || !itemMola->ativa ) {
                el = el->proximo;
                continue;
            }

            if ( CheckCollisionRecs( retColCalculado, itemMola->ret ) &&
                 j->vel.y >= 0.0f &&
                 retColCalculado.y + retColCalculado.height <= itemMola->ret.y + itemMola->ret.height * 0.75f ) {

                j->ret.y = itemMola->ret.y - qa->retColisao.height - deslocamentoY;
                j->vel.y = j->velPulo * 1.35f;
                j->quantidadePulos = 1;

                acionarItemMola( itemMola );
                PlaySound( rm.somMola );

            }

        } else if ( item->tipo == TIPO_ITEM_ESPINHO ) {

            ItemEspinho *itemEspinho = (ItemEspinho*) item->objeto;

            if ( itemEspinho == NULL || !itemEspinho->ativo ) {
                el = el->proximo;
                continue;
            }

            Rectangle retEspinho = {
                itemEspinho->ret.x + 4.0f,
                itemEspinho->ret.y + 8.0f,
                itemEspinho->ret.width - 8.0f,
                itemEspinho->ret.height - 8.0f
            };

            if ( CheckCollisionRecs( retColCalculado, retEspinho ) && !j->invulneravel && !j->temEstrela ) {

                if ( j->temEscudo ) {
                    j->temEscudo = false;
                    PlaySound( rm.somHitComAnel );
                } else if ( j->quantidadeAneis > 0 ) {
                    j->quantidadeAneis = 0;
                    PlaySound( rm.somHitComAnel );
                } else {
                    j->quantidadeVidas--;
                    PlaySound( rm.somMorte );
                    j->morrendo = true;
                    j->vel.y = -600;
                    j->vel.x = 0;
                }

                if ( !j->morrendo ) {
                    j->vel.y = j->velPulo * 0.55f;
                    j->vel.x = j->olhandoParaDireita ? -260.0f : 260.0f;
                }

                j->invulneravel = true;
                j->contadorTempoInvulnerabilidade = 0.0f;
                j->contadorTempoPiscaPisca = 0.0f;
                j->piscaPisca = false;

            }

        }

        el = el->proximo;

    }

}

static void resolverColisaoJogadorInimigosMapa( Jogador *j, Mapa *mapa ) {
 
    ElementoMapa *el = mapa->inimigos;
 
    while ( el != NULL ) {
 
        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );
 
        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;

        float deslocamentoY = qa->retColisao.y;
 
        Rectangle retColJogador = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };
 
        Inimigo *inimigo = (Inimigo*) el->objeto;
 
        #define INIMIGO_ATIVO_E_VIVO( ativo, estado, estadoMorrendo ) \
            if ( !(ativo) || (estado) == (estadoMorrendo) ) { el = el->proximo; continue; }
 
        #define CALCULAR_RET_INIMIGO( ret_, qa_, dir_ ) \
            (Rectangle){ \
                (ret_).x + ((dir_) ? (ret_).width - (qa_)->retColisao.x - (qa_)->retColisao.width : (qa_)->retColisao.x), \
                (ret_).y + (qa_)->retColisao.y, \
                (qa_)->retColisao.width, \
                (qa_)->retColisao.height \
            }
 
        #define APLICAR_DANO_INIMIGO() \
            if ( (j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO) || j->temEstrela ) { \
                if ( !j->temEstrela ) j->vel.y = j->velPulo; \
                PlaySound( rm.somHitInimigo ); \
                j->pontuacao += 100; \
            } else if ( !j->invulneravel ) { \
                if ( j->temEscudo ) { \
                    j->temEscudo = false; \
                    PlaySound( rm.somHitComAnel ); \
                } else if ( j->quantidadeAneis > 0 ) { \
                    j->quantidadeAneis = 0; \
                    PlaySound( rm.somHitComAnel ); \
                } else { \
                    j->quantidadeVidas--; \
                    PlaySound( rm.somMorte ); \
                    j->morrendo = true; \
                    j->vel.y = -600; \
                    j->vel.x = 0; \
                } \
                j->invulneravel = true; \
                j->contadorTempoInvulnerabilidade = 0.0f; \
                j->contadorTempoPiscaPisca = 0.0f; \
                j->piscaPisca = false; \
            }
 
        if ( inimigo->tipo == TIPO_INIMIGO_MOTOBUG ) {
 
            InimigoMotobug *motobug = (InimigoMotobug*) inimigo->objeto;
            INIMIGO_ATIVO_E_VIVO( motobug->ativo, motobug->estado, ESTADO_INIMIGO_MOTOBUG_MORRENDO )
 
            QuadroAnimacao *qaI = getQuadroAnimacaoAtualInimigoMotobug( motobug );
            Rectangle retI = CALCULAR_RET_INIMIGO( motobug->ret, qaI, motobug->olhandoParaDireita );
 
            if ( CheckCollisionRecs( retColJogador, retI ) ) {

                APLICAR_DANO_INIMIGO()

                if ( (j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO) || j->temEstrela ) {
                    motobug->estado = ESTADO_INIMIGO_MOTOBUG_MORRENDO;
                }

                return;

            }
 
        } else if ( inimigo->tipo == TIPO_INIMIGO_BUZZBOMBER || inimigo->tipo == TIPO_INIMIGO_MOSQUITO ) {
 
            InimigoBuzzBomber *bb = (InimigoBuzzBomber*) inimigo->objeto;
            INIMIGO_ATIVO_E_VIVO( bb->ativo, bb->estado, ESTADO_INIMIGO_BUZZBOMBER_MORRENDO )
 
            QuadroAnimacao *qaI = getQuadroAnimacaoAtualInimigoBuzzBomber( bb );
            Rectangle retI = CALCULAR_RET_INIMIGO( bb->ret, qaI, bb->olhandoParaDireita );
 
            if ( CheckCollisionRecs( retColJogador, retI ) ) {

                APLICAR_DANO_INIMIGO()

                if ( (j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO) || j->temEstrela ) {
                    bb->estado = ESTADO_INIMIGO_BUZZBOMBER_MORRENDO;
                }

                return;

            }
 
        } else if ( inimigo->tipo == TIPO_INIMIGO_CARANGUEJO ) {
 
            InimigoCaranguejo *crab = (InimigoCaranguejo*) inimigo->objeto;
            INIMIGO_ATIVO_E_VIVO( crab->ativo, crab->estado, ESTADO_INIMIGO_CARANGUEJO_MORRENDO )
 
            QuadroAnimacao *qaI = getQuadroAnimacaoAtualInimigoCaranguejo( crab );
            Rectangle retI = CALCULAR_RET_INIMIGO( crab->ret, qaI, crab->olhandoParaDireita );
 
            if ( CheckCollisionRecs( retColJogador, retI ) ) {

                APLICAR_DANO_INIMIGO()

                if ( (j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO) || j->temEstrela ) {
                    crab->estado = ESTADO_INIMIGO_CARANGUEJO_MORRENDO;
                }

                return;

            }
 
        }
 
        #undef INIMIGO_ATIVO_E_VIVO
        #undef CALCULAR_RET_INIMIGO
        #undef APLICAR_DANO_INIMIGO
 
        el = el->proximo;

    }

}
