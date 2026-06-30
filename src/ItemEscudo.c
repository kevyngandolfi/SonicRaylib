#include <stdlib.h>

#include "raylib/raylib.h"

#include "ItemEscudo.h"

ItemEscudo *criarItemEscudo( Rectangle ret ) {

    ItemEscudo *escudo = (ItemEscudo*) malloc( sizeof( ItemEscudo ) );

    escudo->ret = ret;
    escudo->ativo = true;
    escudo->estado = ESTADO_ITEM_ESCUDO_PARADO;

    return escudo;

}

void destruirItemEscudo( ItemEscudo *itemEscudo ) {

    if ( itemEscudo != NULL ) {
        free( itemEscudo );
    }

}

void desenharItemEscudo( ItemEscudo *itemEscudo ) {

    if ( itemEscudo == NULL ) return;

    if ( itemEscudo->ativo && itemEscudo->estado != ESTADO_ITEM_ESCUDO_COLETADO ) {

        Rectangle caixa = itemEscudo->ret;
        Rectangle tela = {
            caixa.x + 8.0f,
            caixa.y + 7.0f,
            caixa.width - 16.0f,
            caixa.height - 18.0f
        };
        Vector2 centro = {
            tela.x + tela.width / 2.0f,
            tela.y + tela.height / 2.0f
        };

        DrawRectangleRounded( caixa, 0.16f, 5, (Color){ 34, 38, 42, 255 } );
        DrawRectangleRoundedLinesEx( caixa, 0.16f, 5, 2.0f, (Color){ 205, 212, 220, 255 } );
        DrawRectangleRounded( tela, 0.12f, 4, (Color){ 8, 16, 26, 255 } );
        DrawRectangleRounded( (Rectangle){ caixa.x + 14.0f, caixa.y + caixa.height - 9.0f, caixa.width - 28.0f, 5.0f }, 0.22f, 4, BLACK );

        DrawCircleV( centro, 12.0f, Fade( SKYBLUE, 0.82f ) );
        DrawCircleLines( (int)centro.x, (int)centro.y, 12.0f, WHITE );
        DrawCircleLines( (int)centro.x, (int)centro.y, 7.0f, BLUE );
        DrawCircleV( (Vector2){ centro.x - 5.0f, centro.y - 5.0f }, 2.0f, WHITE );

    }

}
