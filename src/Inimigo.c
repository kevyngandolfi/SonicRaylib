/**
 * @file Inimigo.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Inimigo.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Inimigo.h"
#include "InimigoMotobug.h"
#include "InimigoBuzzBomber.h"
#include "InimigoCaranguejo.h"
#include "Tipos.h"

Inimigo *criarInimigo( TipoInimigo tipo ) {
    Inimigo *novoInimigo = (Inimigo*) malloc( sizeof( Inimigo ) );
    novoInimigo->objeto = NULL;
    novoInimigo->tipo = tipo;
    return novoInimigo;
}

void destruirInimigo( Inimigo *inimigo ) {
    if ( inimigo != NULL ) {
        switch ( inimigo->tipo ) {
            case TIPO_INIMIGO_MOTOBUG:
                destruirInimigoMotobug( (InimigoMotobug*) inimigo->objeto );
                break;
            case TIPO_INIMIGO_BUZZBOMBER:
            case TIPO_INIMIGO_MOSQUITO:
                destruirInimigoBuzzBomber( (InimigoBuzzBomber*) inimigo->objeto );
                break;
            case TIPO_INIMIGO_CARANGUEJO:
                destruirInimigoCaranguejo( (InimigoCaranguejo*) inimigo->objeto );
                break;
            default:
                break;
        }
        free( inimigo );
    }
}

void atualizarInimigo( Inimigo *inimigo, GameWorld *gw, float delta ) {
    switch ( inimigo->tipo ) {
        case TIPO_INIMIGO_MOTOBUG:
            atualizarInimigoMotobug( (InimigoMotobug*) inimigo->objeto, gw, delta );
            break;
        case TIPO_INIMIGO_BUZZBOMBER:
        case TIPO_INIMIGO_MOSQUITO:
            atualizarInimigoBuzzBomber( (InimigoBuzzBomber*) inimigo->objeto, gw, delta );
            break;
        case TIPO_INIMIGO_CARANGUEJO:
            atualizarInimigoCaranguejo( (InimigoCaranguejo*) inimigo->objeto, gw, delta );
            break;
        default:
            return;
    }
}

void desenharInimigo( Inimigo *inimigo ) {
    switch ( inimigo->tipo ) {
        case TIPO_INIMIGO_MOTOBUG:
            desenharInimigoMotobug( (InimigoMotobug*) inimigo->objeto );
            break;
        case TIPO_INIMIGO_BUZZBOMBER:
        case TIPO_INIMIGO_MOSQUITO:
            desenharInimigoBuzzBomber( (InimigoBuzzBomber*) inimigo->objeto );
            break;
        case TIPO_INIMIGO_CARANGUEJO:
            desenharInimigoCaranguejo( (InimigoCaranguejo*) inimigo->objeto );
            break;
        default:
            return;
    }
}

void resolverColisaoInimigoObstaculosMapaX( Inimigo *inimigo, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        QuadroAnimacao *qa = NULL;
        bool *olhandoParaDireita = NULL;
        Rectangle *ret = NULL;

        if ( inimigo->tipo == TIPO_INIMIGO_MOTOBUG ) {
            InimigoMotobug *m = (InimigoMotobug*) inimigo->objeto;
            qa = getQuadroAnimacaoAtualInimigoMotobug( m );
            olhandoParaDireita = &m->olhandoParaDireita;
            ret = &m->ret;
        } else if ( inimigo->tipo == TIPO_INIMIGO_BUZZBOMBER || inimigo->tipo == TIPO_INIMIGO_MOSQUITO ) {
            InimigoBuzzBomber *b = (InimigoBuzzBomber*) inimigo->objeto;
            qa = getQuadroAnimacaoAtualInimigoBuzzBomber( b );
            olhandoParaDireita = &b->olhandoParaDireita;
            ret = &b->ret;
        } else if ( inimigo->tipo == TIPO_INIMIGO_CARANGUEJO ) {
            InimigoCaranguejo *c = (InimigoCaranguejo*) inimigo->objeto;
            qa = getQuadroAnimacaoAtualInimigoCaranguejo( c );
            olhandoParaDireita = &c->olhandoParaDireita;
            ret = &c->ret;
        } else {
            el = el->proximo;
            continue;
        }

        float deslocamentoX = *olhandoParaDireita
            ? ret->width - qa->retColisao.x - qa->retColisao.width
            : qa->retColisao.x;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retCol = {
            ret->x + deslocamentoX,
            ret->y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( CheckCollisionRecs( retCol, o->ret ) ) {
            if ( retCol.x + retCol.width / 2 < o->ret.x + o->ret.width / 2 ) {
                ret->x = o->ret.x - qa->retColisao.width - deslocamentoX;
            } else {
                ret->x = o->ret.x + o->ret.width - deslocamentoX;
            }
            *olhandoParaDireita = !*olhandoParaDireita;
        }

        el = el->proximo;
    }
}

void resolverColisaoInimigoObstaculosMapaY( Inimigo *inimigo, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        Obstaculo *o = (Obstaculo*) el->objeto;
        QuadroAnimacao *qa = NULL;
        bool *olhandoParaDireita = NULL;
        Rectangle *ret = NULL;
        Vector2 *vel = NULL;

        if ( inimigo->tipo == TIPO_INIMIGO_MOTOBUG ) {
            InimigoMotobug *m = (InimigoMotobug*) inimigo->objeto;
            qa = getQuadroAnimacaoAtualInimigoMotobug( m );
            olhandoParaDireita = &m->olhandoParaDireita;
            ret = &m->ret;
            vel = &m->vel;
        } else if ( inimigo->tipo == TIPO_INIMIGO_CARANGUEJO ) {
            InimigoCaranguejo *c = (InimigoCaranguejo*) inimigo->objeto;
            qa = getQuadroAnimacaoAtualInimigoCaranguejo( c );
            olhandoParaDireita = &c->olhandoParaDireita;
            ret = &c->ret;
            vel = &c->vel;
        } else {
            // BuzzBomber não precisa de colisão Y (voa)
            el = el->proximo;
            continue;
        }

        float deslocamentoX = *olhandoParaDireita
            ? ret->width - qa->retColisao.x - qa->retColisao.width
            : qa->retColisao.x;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retCol = {
            ret->x + deslocamentoX,
            ret->y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        if ( CheckCollisionRecs( retCol, o->ret ) ) {
            if ( retCol.y + retCol.height / 2 < o->ret.y + o->ret.height / 2 ) {
                ret->y = o->ret.y - qa->retColisao.height - deslocamentoY;
            } else {
                ret->y = o->ret.y + o->ret.height - deslocamentoY;
            }
            vel->y = 0;
        }

        el = el->proximo;
    }
}
