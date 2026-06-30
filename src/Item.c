/**
 * @file Item.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Item.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Item.h"
#include "ItemAnel.h"
#include "ItemEscudo.h"
#include "ItemEstrela.h"
#include "ItemMola.h"
#include "ItemEspinho.h"
#include "Tipos.h"

/**
 * @brief Cria um novo Item.
 */
Item *criarItem( TipoItem tipo ) {

    Item *novoItem = (Item*) malloc( sizeof( Item ) );
    novoItem->objeto = NULL;
    novoItem->tipo = tipo;

    return novoItem;

}

/**
 * @brief Destroi um item.
 */
void destruirItem( Item *item ) {
    if ( item != NULL ) {
        switch ( item->tipo ) {
            case TIPO_ITEM_ANEL:
                destruirItemAnel( (ItemAnel*) item->objeto );
                break;
            case TIPO_ITEM_ESCUDO:
                destruirItemEscudo( (ItemEscudo*) item->objeto );
                break;
            case TIPO_ITEM_ESTRELA:
                destruirItemEstrela( (ItemEstrela*) item->objeto );
                break;
            case TIPO_ITEM_MOLA:
                destruirItemMola( (ItemMola*) item->objeto );
                break;
            case TIPO_ITEM_ESPINHO:
                destruirItemEspinho( (ItemEspinho*) item->objeto );
                break;
            default:
                break;
        }
        free( item );
    }
}

/**
 * @brief Atualiza um item.
 */
void atualizarItem( Item *item, float delta ) {

    switch ( item->tipo ) {
        case TIPO_ITEM_ANEL:
            atualizarItemAnel( (ItemAnel*) item->objeto, delta );
            break;
        case TIPO_ITEM_ESCUDO:
        case TIPO_ITEM_ESTRELA:
            (void) delta;
            break;
        case TIPO_ITEM_MOLA:
            atualizarItemMola( (ItemMola*) item->objeto, delta );
            break;
        case TIPO_ITEM_ESPINHO:
            (void) delta;
            break;
        default:
            return;
    }

}

/**
 * @brief Desenha um item.
 */
void desenharItem( Item *item ) {

    switch ( item->tipo ) {
        case TIPO_ITEM_ANEL:
            desenharItemAnel( (ItemAnel*) item->objeto );
            break;
        case TIPO_ITEM_ESCUDO:
            desenharItemEscudo( (ItemEscudo*) item->objeto );
            break;
        case TIPO_ITEM_ESTRELA:
            desenharItemEstrela( (ItemEstrela*) item->objeto );
            break;
        case TIPO_ITEM_MOLA:
            desenharItemMola( (ItemMola*) item->objeto );
            break;
        case TIPO_ITEM_ESPINHO:
            desenharItemEspinho( (ItemEspinho*) item->objeto );
            break;
        default:
            return;
    }

}
