/**
 * @file InimigoBuzzBomber.c
 * @brief Implementação do Inimigo (BuzzBomber) - voa horizontalmente e atira.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoBuzzBomber.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacao( InimigoBuzzBomber *inimigo, QuadroAnimacao *qa, Color tonalidade );
static void desenharQuadroMorrendo( InimigoBuzzBomber *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade );
static Animacao *getAnimacaoAtual( InimigoBuzzBomber *inimigo );

static void dispararTiroBuzzBomber( InimigoBuzzBomber *inimigo, GameWorld *gw );
static void atualizarTiroBuzzBomber( InimigoBuzzBomber *inimigo, GameWorld *gw, float delta );
static void desenharTiroBuzzBomber( InimigoBuzzBomber *inimigo );
static void aplicarDanoDoTiroNoJogador( Jogador *j );

static const bool MOSTRAR_RETANGULOS = false;

InimigoBuzzBomber *criarInimigoBuzzBomber( Rectangle ret, Color cor ) {

    InimigoBuzzBomber *novo = (InimigoBuzzBomber*) malloc( sizeof( InimigoBuzzBomber ) );

    novo->ret = ret;
    novo->vel = (Vector2) { 0 };
    novo->cor = cor;
    novo->velAndando = 80;
    novo->alturaVoo = ret.y;
    novo->estado = ESTADO_INIMIGO_BUZZBOMBER_VOANDO;
    novo->ativo = true;
    novo->olhandoParaDireita = false;

    /*
     * Configuração do tiro.
     * Um tiro por vez para evitar bug e facilitar a entrega.
     */
    novo->tiroAtivo = false;
    novo->tiroRet = (Rectangle){ 0.0f, 0.0f, 16.0f, 12.0f };
    novo->tiroVel = (Vector2){ 0.0f, 0.0f };
    novo->contadorTiro = 0.0f;
    novo->intervaloTiro = 1.65f;
    novo->tempoVidaTiro = 0.0f;
    novo->tempoVidaMaxTiro = 3.0f;
    novo->usaSpriteMosquito = false;

    int quantidadeAnimacoes = 0;

    novo->animacaoVoando.quantidadeQuadros = 2;
    novo->animacaoVoando.quadroAtual = 0;
    novo->animacaoVoando.contadorTempoQuadro = 0.0f;
    novo->animacaoVoando.pararNoUltimoQuadro = false;
    novo->animacaoVoando.executarUmaVez = false;
    novo->animacaoVoando.finalizada = false;
    
    criarQuadrosAnimacao( &novo->animacaoVoando, novo->animacaoVoando.quantidadeQuadros );
    
    inicializarQuadrosAnimacao(
        novo->animacaoVoando.quadros,
        novo->animacaoVoando.quantidadeQuadros,
        200,
        0, 0,
        24, 18,
        0,
        false,
        (Rectangle) { 4, 4, 32, 24 } 
    );

    novo->animacaoVoando.quadros[0].fonte = (Rectangle){ 0, 5, 51, 28 };
    novo->animacaoVoando.quadros[1].fonte = (Rectangle){ 53, 0, 43, 32 };

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

    novo->animacoes[ESTADO_INIMIGO_BUZZBOMBER_VOANDO]   = &novo->animacaoVoando;   quantidadeAnimacoes++;
    novo->animacoes[ESTADO_INIMIGO_BUZZBOMBER_MORRENDO] = &novo->animacaoMorrendo; quantidadeAnimacoes++;
    novo->quantidadeAnimacoes = quantidadeAnimacoes;

    return novo;

}

InimigoBuzzBomber *criarInimigoMosquito( Rectangle ret, Color cor ) {

    InimigoBuzzBomber *novo = criarInimigoBuzzBomber( ret, cor );

    novo->usaSpriteMosquito = true;
    novo->animacaoVoando.quadros[0].fonte = (Rectangle){ 0, 0, 28, 20 };
    novo->animacaoVoando.quadros[1].fonte = (Rectangle){ 28, 0, 28, 20 };

    return novo;

}

void destruirInimigoBuzzBomber( InimigoBuzzBomber *inimigo ) {

    if ( inimigo != NULL ) {

        for ( int i = 0; i < inimigo->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( inimigo->animacoes[i] );
        }

        free( inimigo );

    }

}

void atualizarInimigoBuzzBomber( InimigoBuzzBomber *inimigo, GameWorld *gw, float delta ) {

    if ( !inimigo->ativo ) return;

    if ( inimigo->estado == ESTADO_INIMIGO_BUZZBOMBER_VOANDO ) {

        atualizarAnimacao( &inimigo->animacaoVoando, delta );

        if ( inimigo->olhandoParaDireita ) {
            inimigo->vel.x = inimigo->velAndando;
        } else {
            inimigo->vel.x = -inimigo->velAndando;
        }

        inimigo->ret.x += inimigo->vel.x * delta;
        inimigo->ret.y = inimigo->alturaVoo;

        Inimigo ini = { .objeto = inimigo, .tipo = TIPO_INIMIGO_BUZZBOMBER };
        resolverColisaoInimigoObstaculosMapaX( &ini, gw->mapa );

        /*
         * Atira se o Sonic estiver relativamente perto.
         */
        inimigo->contadorTiro += delta;

        float centroBuzzX = inimigo->ret.x + inimigo->ret.width / 2.0f;
        float centroSonicX = gw->jogador->ret.x + gw->jogador->ret.width / 2.0f;
        float distanciaX = fabsf( centroSonicX - centroBuzzX );

        if ( !inimigo->tiroAtivo && inimigo->contadorTiro >= inimigo->intervaloTiro && distanciaX < 650.0f ) {
            dispararTiroBuzzBomber( inimigo, gw );
        }

    } else if ( inimigo->estado == ESTADO_INIMIGO_BUZZBOMBER_MORRENDO ) {

        atualizarAnimacao( &inimigo->animacaoMorrendo, delta );

        if ( inimigo->animacaoMorrendo.finalizada ) {
            inimigo->ativo = false;
            inimigo->tiroAtivo = false;
        }

    }

    atualizarTiroBuzzBomber( inimigo, gw, delta );

}

void desenharInimigoBuzzBomber( InimigoBuzzBomber *inimigo ) {

    if ( !inimigo->ativo ) return;

    if ( inimigo->estado == ESTADO_INIMIGO_BUZZBOMBER_VOANDO ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualInimigoBuzzBomber( inimigo );
        desenharQuadroAnimacao( inimigo, qa, WHITE );

    } else {

        desenharQuadroMorrendo(
            inimigo,
            getQuadroAtualAnimacao( &inimigo->animacaoMorrendo ),
            2.0f,
            WHITE
        );

    }

    desenharTiroBuzzBomber( inimigo );

    if ( MOSTRAR_RETANGULOS ) {
        DrawRectangleRec( inimigo->ret, Fade( inimigo->cor, 0.5f ) );
        if ( inimigo->tiroAtivo ) {
            DrawRectangleRec( inimigo->tiroRet, Fade( YELLOW, 0.5f ) );
        }
    }

}

QuadroAnimacao *getQuadroAnimacaoAtualInimigoBuzzBomber( InimigoBuzzBomber *inimigo ) {
    return getQuadroAtualAnimacao( getAnimacaoAtual( inimigo ) );
}

static void desenharQuadroAnimacao( InimigoBuzzBomber *inimigo, QuadroAnimacao *qa, Color tonalidade ) {

    if ( qa == NULL ) return;

    const float targetWidth = inimigo->usaSpriteMosquito ? 42.0f : 80.0f;
    const float targetHeight = inimigo->usaSpriteMosquito ? 30.0f : 60.0f;

    float posX = inimigo->ret.x - ( targetWidth - inimigo->ret.width ) / 2.0f;
    float posY = inimigo->ret.y - ( targetHeight - inimigo->ret.height ) / 2.0f;

    Rectangle destino = { posX, posY, targetWidth, targetHeight };

    DrawTexturePro(
        inimigo->usaSpriteMosquito ? rm.texturaMosquito : rm.texturaBuzzBomber,
        (Rectangle) {
            qa->fonte.x,
            qa->fonte.y,
            inimigo->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width,
            qa->fonte.height
        },
        destino,
        (Vector2) { 0 },
        0.0f,
        tonalidade
    );

}

static void desenharQuadroMorrendo( InimigoBuzzBomber *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade ) {

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

static Animacao *getAnimacaoAtual( InimigoBuzzBomber *inimigo ) {
    return inimigo->animacoes[inimigo->estado];
}

static void dispararTiroBuzzBomber( InimigoBuzzBomber *inimigo, GameWorld *gw ) {

    inimigo->contadorTiro = 0.0f;
    inimigo->tempoVidaTiro = 0.0f;
    inimigo->tiroAtivo = true;

    float origemX = inimigo->ret.x + inimigo->ret.width / 2.0f - 8.0f;
    float origemY = inimigo->ret.y + inimigo->ret.height * 0.62f;

    inimigo->tiroRet = (Rectangle){
        origemX,
        origemY,
        16.0f,
        12.0f
    };

    float alvoX = gw->jogador->ret.x + gw->jogador->ret.width / 2.0f;
    float alvoY = gw->jogador->ret.y + gw->jogador->ret.height / 2.0f;

    float centroTiroX = inimigo->tiroRet.x + inimigo->tiroRet.width / 2.0f;
    float centroTiroY = inimigo->tiroRet.y + inimigo->tiroRet.height / 2.0f;

    float dx = alvoX - centroTiroX;
    float dy = alvoY - centroTiroY;

    float dist = sqrtf( dx * dx + dy * dy );

    if ( dist < 1.0f ) {
        dist = 1.0f;
    }

    float velocidadeTiro = 260.0f;

    inimigo->tiroVel.x = ( dx / dist ) * velocidadeTiro;
    inimigo->tiroVel.y = ( dy / dist ) * velocidadeTiro;

    /*
     * Vira o Buzz para o lado do tiro.
     */
    inimigo->olhandoParaDireita = inimigo->tiroVel.x > 0.0f;

}

static void atualizarTiroBuzzBomber( InimigoBuzzBomber *inimigo, GameWorld *gw, float delta ) {

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

static void desenharTiroBuzzBomber( InimigoBuzzBomber *inimigo ) {

    if ( !inimigo->tiroAtivo ) return;

    Vector2 centro = {
        inimigo->tiroRet.x + inimigo->tiroRet.width / 2.0f,
        inimigo->tiroRet.y + inimigo->tiroRet.height / 2.0f
    };

    Vector2 rastro = {
        centro.x - inimigo->tiroVel.x * 0.040f,
        centro.y - inimigo->tiroVel.y * 0.040f
    };

    DrawCircleV( rastro, 8.0f, Fade( ORANGE, 0.50f ) );
    DrawCircleV( centro, 7.0f, YELLOW );
    DrawCircleV( centro, 3.0f, WHITE );
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
