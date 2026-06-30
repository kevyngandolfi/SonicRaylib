#include <stdlib.h>

#include "raylib/raylib.h"

#include "ItemEspinho.h"
#include "ResourceManager.h"

ItemEspinho *criarItemEspinho( Rectangle ret ) {

    ItemEspinho *espinho = (ItemEspinho*) malloc( sizeof( ItemEspinho ) );

    espinho->ret = ret;
    espinho->cor = WHITE;
    espinho->ativo = true;

    return espinho;

}

void destruirItemEspinho( ItemEspinho *itemEspinho ) {

    if ( itemEspinho != NULL ) {
        free( itemEspinho );
    }

}

void desenharItemEspinho( ItemEspinho *itemEspinho ) {

    if ( itemEspinho == NULL || !itemEspinho->ativo ) return;

    if ( rm.texturaSpikes.id > 0 ) {

        DrawTexturePro(
            rm.texturaSpikes,
            (Rectangle){ 0.0f, 0.0f, (float)rm.texturaSpikes.width, (float)rm.texturaSpikes.height },
            itemEspinho->ret,
            (Vector2){ 0 },
            0.0f,
            WHITE
        );

    } else {

        float baseY = itemEspinho->ret.y + itemEspinho->ret.height;
        float larguraPonta = itemEspinho->ret.width / 4.0f;

        for ( int i = 0; i < 4; i++ ) {
            DrawTriangle(
                (Vector2){ itemEspinho->ret.x + i * larguraPonta, baseY },
                (Vector2){ itemEspinho->ret.x + ( i + 0.5f ) * larguraPonta, itemEspinho->ret.y },
                (Vector2){ itemEspinho->ret.x + ( i + 1.0f ) * larguraPonta, baseY },
                WHITE
            );
        }

    }

}
