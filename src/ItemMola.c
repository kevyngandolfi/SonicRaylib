#include <stdlib.h>

#include "raylib/raylib.h"

#include "ItemMola.h"
#include "ResourceManager.h"

ItemMola *criarItemMola( Rectangle ret ) {

    ItemMola *mola = (ItemMola*) malloc( sizeof( ItemMola ) );

    mola->ret = ret;
    mola->cor = RED;
    mola->ativa = true;
    mola->tempoAnimacao = 0.0f;

    return mola;

}

void destruirItemMola( ItemMola *itemMola ) {

    if ( itemMola != NULL ) {
        free( itemMola );
    }

}

void atualizarItemMola( ItemMola *itemMola, float delta ) {

    if ( itemMola == NULL ) return;

    if ( itemMola->tempoAnimacao > 0.0f ) {
        itemMola->tempoAnimacao -= delta;

        if ( itemMola->tempoAnimacao < 0.0f ) {
            itemMola->tempoAnimacao = 0.0f;
        }
    }

}

void acionarItemMola( ItemMola *itemMola ) {

    if ( itemMola != NULL ) {
        itemMola->tempoAnimacao = 0.16f;
    }

}

void desenharItemMola( ItemMola *itemMola ) {

    if ( itemMola == NULL || !itemMola->ativa ) return;

    float compressao = itemMola->tempoAnimacao > 0.0f ? 8.0f : 0.0f;

    Rectangle destino = {
        itemMola->ret.x,
        itemMola->ret.y + compressao,
        itemMola->ret.width,
        itemMola->ret.height - compressao
    };

    if ( rm.texturaMola.id > 0 ) {

        DrawTexturePro(
            rm.texturaMola,
            (Rectangle){ 0.0f, 0.0f, (float)rm.texturaMola.width, (float)rm.texturaMola.height },
            destino,
            (Vector2){ 0 },
            0.0f,
            WHITE
        );

    } else {

        DrawRectangleRounded( destino, 0.18f, 4, RED );
        DrawRectangleRoundedLinesEx( destino, 0.18f, 4, 2.0f, WHITE );

    }

}
