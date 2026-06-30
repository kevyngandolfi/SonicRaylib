/**
 * @file InimigoCaranguejo.c
 * @brief Implementação do Inimigo (Crabmeat) - terrestre, anda e atira.
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoCaranguejo.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacao( InimigoCaranguejo *inimigo, QuadroAnimacao *qa, Color tonalidade );
static void desenharQuadroMorrendo( InimigoCaranguejo *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade );
static Animacao *getAnimacaoAtual( InimigoCaranguejo *inimigo );

static void dispararTiroCaranguejo( InimigoCaranguejo *inimigo, GameWorld *gw );
static void atualizarTiroCaranguejo( InimigoCaranguejo *inimigo, GameWorld *gw, float delta );
static void desenharTiroCaranguejo( InimigoCaranguejo *inimigo );
static void aplicarDanoDoTiroNoJogador( Jogador *j );

static const bool MOSTRAR_RETANGULOS = false;

InimigoCaranguejo *criarInimigoCaranguejo( Rectangle ret, Color cor ) {

    InimigoCaranguejo *novo = (InimigoCaranguejo*) malloc( sizeof( InimigoCaranguejo ) );

    novo->ret = ret;
    novo->vel = (Vector2) { 0 };
    novo->cor = cor;
    novo->velAndando = 80;
    novo->velMaxQueda = 600;
    novo->estado = ESTADO_INIMIGO_CARANGUEJO_ANDANDO;
    novo->ativo = true;
    novo->olhandoParaDireita = false;

    /*
     * Configuração do tiro.
     * Um tiro por vez para não virar bagunça.
     */
    novo->tiroAtivo = false;
    novo->tiroRet = (Rectangle){ 0.0f, 0.0f, 18.0f, 14.0f };
    novo->tiroVel = (Vector2){ 0.0f, 0.0f };
    novo->contadorTiro = 0.0f;
    novo->intervaloTiro = 2.0f;
    novo->tempoVidaTiro = 0.0f;
    novo->tempoVidaMaxTiro = 2.2f;

    int quantidadeAnimacoes = 0;

    novo->animacaoAndando.quantidadeQuadros = 4;
    novo->animacaoAndando.quadroAtual = 0;
    novo->animacaoAndando.contadorTempoQuadro = 0.0f;
    novo->animacaoAndando.pararNoUltimoQuadro = false;
    novo->animacaoAndando.executarUmaVez = false;
    novo->animacaoAndando.finalizada = false;

    criarQuadrosAnimacao( &novo->animacaoAndando, novo->animacaoAndando.quantidadeQuadros );

    inicializarQuadrosAnimacao(
        novo->animacaoAndando.quadros,
        novo->animacaoAndando.quantidadeQuadros,
        200,
        2, 1,
        48, 45,
        8,
        false,
        (Rectangle) { 4, 20, 40, 38 }
    );

    novo->animacaoMorrendo.quantidadeQuadros = 4;
    novo->animacaoMorrendo.quadroAtual = 0;
    novo->animacaoMorrendo.contadorTempoQuadro = 0.0f;
    novo->animacaoMorrendo.pararNoUltimoQuadro = false;
    novo->animacaoMorrendo.executarUmaVez = true;
    novo->animacaoMorrendo.finalizada = false;

    criarQuadrosAnimacao( &novo->animacaoMorrendo, novo->animacaoMorrendo.quantidadeQuadros );

    inicializarQuadrosAnimacao(
        novo->animacaoMorrendo.quadros,
        novo->animacaoMorrendo.quantidadeQuadros,
        100,
        169, 1,
        32, 32,
        1,
        false,
        (Rectangle) { 0 }
    );

    novo->animacoes[ESTADO_INIMIGO_CARANGUEJO_ANDANDO]  = &novo->animacaoAndando;  quantidadeAnimacoes++;
    novo->animacoes[ESTADO_INIMIGO_CARANGUEJO_MORRENDO] = &novo->animacaoMorrendo; quantidadeAnimacoes++;
    novo->quantidadeAnimacoes = quantidadeAnimacoes;

    return novo;

}

void destruirInimigoCaranguejo( InimigoCaranguejo *inimigo ) {

    if ( inimigo != NULL ) {

        for ( int i = 0; i < inimigo->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( inimigo->animacoes[i] );
        }

        free( inimigo );

    }

}

void atualizarInimigoCaranguejo( InimigoCaranguejo *inimigo, GameWorld *gw, float delta ) {

    if ( !inimigo->ativo ) return;

    if ( inimigo->estado == ESTADO_INIMIGO_CARANGUEJO_ANDANDO ) {

        atualizarAnimacao( &inimigo->animacaoAndando, delta );

        Inimigo ini = { .objeto = inimigo, .tipo = TIPO_INIMIGO_CARANGUEJO };

        if ( inimigo->olhandoParaDireita ) {
            inimigo->vel.x = inimigo->velAndando;
        } else {
            inimigo->vel.x = -inimigo->velAndando;
        }

        inimigo->ret.x += inimigo->vel.x * delta;
        resolverColisaoInimigoObstaculosMapaX( &ini, gw->mapa );

        inimigo->vel.y += gw->gravidade * delta;

        if ( inimigo->vel.y > inimigo->velMaxQueda ) {
            inimigo->vel.y = inimigo->velMaxQueda;
        }

        inimigo->ret.y += inimigo->vel.y * delta;
        resolverColisaoInimigoObstaculosMapaY( &ini, gw->mapa );

        /*
         * Atira se o Sonic estiver próximo.
         */
        inimigo->contadorTiro += delta;

        float centroCrabX = inimigo->ret.x + inimigo->ret.width / 2.0f;
        float centroSonicX = gw->jogador->ret.x + gw->jogador->ret.width / 2.0f;
        float distanciaX = centroSonicX - centroCrabX;

        if ( !inimigo->tiroAtivo && inimigo->contadorTiro >= inimigo->intervaloTiro ) {

            if ( distanciaX > -520.0f && distanciaX < 520.0f ) {

                /*
                 * Antes de atirar, olha para o lado do Sonic.
                 */
                inimigo->olhandoParaDireita = distanciaX > 0.0f;
                dispararTiroCaranguejo( inimigo, gw );

            }

        }

    } else if ( inimigo->estado == ESTADO_INIMIGO_CARANGUEJO_MORRENDO ) {

        atualizarAnimacao( &inimigo->animacaoMorrendo, delta );

        if ( inimigo->animacaoMorrendo.finalizada ) {
            inimigo->ativo = false;
            inimigo->tiroAtivo = false;
        }

    }

    atualizarTiroCaranguejo( inimigo, gw, delta );

}

void desenharInimigoCaranguejo( InimigoCaranguejo *inimigo ) {

    if ( !inimigo->ativo ) return;

    if ( inimigo->estado == ESTADO_INIMIGO_CARANGUEJO_ANDANDO ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualInimigoCaranguejo( inimigo );
        desenharQuadroAnimacao( inimigo, qa, WHITE );

    } else {

        desenharQuadroMorrendo(
            inimigo,
            getQuadroAtualAnimacao( &inimigo->animacaoMorrendo ),
            2.0f,
            WHITE
        );

    }

    desenharTiroCaranguejo( inimigo );

    if ( MOSTRAR_RETANGULOS ) {
        DrawRectangleRec( inimigo->ret, Fade( inimigo->cor, 0.5f ) );
        if ( inimigo->tiroAtivo ) {
            DrawRectangleRec( inimigo->tiroRet, Fade( RED, 0.5f ) );
        }
    }

}

QuadroAnimacao *getQuadroAnimacaoAtualInimigoCaranguejo( InimigoCaranguejo *inimigo ) {
    return getQuadroAtualAnimacao( getAnimacaoAtual( inimigo ) );
}

static void desenharQuadroAnimacao( InimigoCaranguejo *inimigo, QuadroAnimacao *qa, Color tonalidade ) {

    if ( qa == NULL ) return;

    DrawTexturePro(
        rm.texturaCaranguejo,
        (Rectangle) {
            qa->fonte.x,
            qa->fonte.y,
            inimigo->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width,
            qa->fonte.height
        },
        inimigo->ret,
        (Vector2) { 0 },
        0.0f,
        tonalidade
    );

}

static void desenharQuadroMorrendo( InimigoCaranguejo *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade ) {

    if ( qa == NULL ) return;

    DrawTexturePro(
        rm.texturaBadniks,
        qa->fonte,
        (Rectangle) {
            inimigo->ret.x,
            inimigo->ret.y,
            qa->fonte.width * escala,
            qa->fonte.height * escala
        },
        (Vector2) { 0 },
        0.0f,
        tonalidade
    );

}

static Animacao *getAnimacaoAtual( InimigoCaranguejo *inimigo ) {
    return inimigo->animacoes[inimigo->estado];
}

/*
 * Disparo do Crabmeat.
 * Faz um tiro horizontal simples, parecido com bolinha de energia.
 */
static void dispararTiroCaranguejo( InimigoCaranguejo *inimigo, GameWorld *gw ) {

    (void) gw;

    inimigo->contadorTiro = 0.0f;
    inimigo->tempoVidaTiro = 0.0f;
    inimigo->tiroAtivo = true;

    float dir = inimigo->olhandoParaDireita ? 1.0f : -1.0f;

    float origemX;

    if ( inimigo->olhandoParaDireita ) {
        origemX = inimigo->ret.x + inimigo->ret.width - 9.0f;
    } else {
        origemX = inimigo->ret.x - 9.0f;
    }

    float origemY = inimigo->ret.y + inimigo->ret.height * 0.45f;

    inimigo->tiroRet = (Rectangle){
        origemX,
        origemY,
        18.0f,
        14.0f
    };

    inimigo->tiroVel.x = dir * 260.0f;
    inimigo->tiroVel.y = 0.0f;

}

static void atualizarTiroCaranguejo( InimigoCaranguejo *inimigo, GameWorld *gw, float delta ) {

    if ( !inimigo->tiroAtivo ) return;

    inimigo->tiroRet.x += inimigo->tiroVel.x * delta;
    inimigo->tiroRet.y += inimigo->tiroVel.y * delta;

    inimigo->tempoVidaTiro += delta;

    if ( inimigo->tempoVidaTiro >= inimigo->tempoVidaMaxTiro ) {
        inimigo->tiroAtivo = false;
        return;
    }

    if ( CheckCollisionRecs( inimigo->tiroRet, gw->jogador->ret ) ) {

        if ( gw->jogador->temEstrela ) {
            inimigo->tiroAtivo = false;
            return;
        }

        aplicarDanoDoTiroNoJogador( gw->jogador );
        inimigo->tiroAtivo = false;

    }

}

static void desenharTiroCaranguejo( InimigoCaranguejo *inimigo ) {

    if ( !inimigo->tiroAtivo ) return;

    Vector2 centro = {
        inimigo->tiroRet.x + inimigo->tiroRet.width / 2.0f,
        inimigo->tiroRet.y + inimigo->tiroRet.height / 2.0f
    };

    Vector2 rastro = {
        centro.x - inimigo->tiroVel.x * 0.030f,
        centro.y
    };

    DrawCircleV( rastro, 8.0f, Fade( ORANGE, 0.50f ) );
    DrawCircleV( centro, 7.0f, RED );
    DrawCircleV( centro, 3.0f, YELLOW );
    DrawCircleLines( (int) centro.x, (int) centro.y, 7.0f, ORANGE );

}

static void aplicarDanoDoTiroNoJogador( Jogador *j ) {

    if ( j->invulneravel || j->morrendo ) return;

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
        j->vel.y = -600.0f;
        j->vel.x = 0.0f;

    }

    j->invulneravel = true;
    j->contadorTempoInvulnerabilidade = 0.0f;
    j->contadorTempoPiscaPisca = 0.0f;
    j->piscaPisca = false;

}