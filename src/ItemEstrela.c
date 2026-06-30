#include <stdlib.h>

#include "raylib/raylib.h"

#include "ItemEstrela.h"

ItemEstrela *criarItemEstrela( Rectangle ret ) {

    ItemEstrela *estrela = (ItemEstrela*) malloc( sizeof( ItemEstrela ) );

    estrela->ret = ret;
    estrela->ativo = true;
    estrela->estado = ESTADO_ITEM_ESTRELA_PARADO;

    return estrela;

}

void destruirItemEstrela( ItemEstrela *itemEstrela ) {

    if ( itemEstrela != NULL ) {
        free( itemEstrela );
    }

}

void desenharItemEstrela( ItemEstrela *itemEstrela ) {

    if ( itemEstrela == NULL ) return;

    if ( itemEstrela->ativo && itemEstrela->estado != ESTADO_ITEM_ESTRELA_COLETADO ) {

        Rectangle caixa = itemEstrela->ret;
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
        DrawRectangleRounded( tela, 0.12f, 4, (Color){ 28, 19, 8, 255 } );
        DrawRectangleRounded( (Rectangle){ caixa.x + 14.0f, caixa.y + caixa.height - 9.0f, caixa.width - 28.0f, 5.0f }, 0.22f, 4, BLACK );

        DrawPoly( centro, 5, 13.0f, -90.0f, GOLD );
        DrawPolyLines( centro, 5, 13.0f, -90.0f, WHITE );
        DrawCircleV( centro, 4.0f, WHITE );

    }

}
