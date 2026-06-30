/**
 * @file Mapa.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Mapa.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Mapa.h"
#include "Inimigo.h"
#include "InimigoMotobug.h"
#include "InimigoBuzzBomber.h"
#include "InimigoCaranguejo.h"
#include "Item.h"
#include "ItemAnel.h"
#include "ItemEscudo.h"
#include "ItemEstrela.h"
#include "ItemMola.h"
#include "ItemEspinho.h"
#include "Obstaculo.h"
#include "Tipos.h"
#include "ResourceManager.h"

static void inserirObstaculo( Mapa *mapa, ElementoMapa *obstaculo );
static void inserirItem( Mapa *mapa, ElementoMapa *item );
static void inserirInimigo( Mapa *mapa, ElementoMapa *inimigo );
static float calcularYPowerUpNoChao(
    const char *dadosMapa,
    int linhaAtual,
    int colunaAtual,
    float tamanhoTile,
    float alturaItem,
    float yFallback
);

Mapa *carregarMapa( const char *caminhoArquivo ) {

    Mapa *novoMapa = (Mapa*) malloc( sizeof( Mapa ) );

    novoMapa->obstaculos = NULL;
    novoMapa->quantidadeObstaculos = 0;
    novoMapa->itens = NULL;
    novoMapa->quantidadeItens = 0;
    novoMapa->inimigos = NULL;
    novoMapa->quantidadeInimigos = 0;
    novoMapa->dimensaoPadraoElementos = 48;
    novoMapa->linhas = 0;
    novoMapa->colunas = 0;

    char *dadosMapa = LoadFileText( caminhoArquivo );

    if ( dadosMapa == NULL ) {
        TraceLog( LOG_ERROR, "Erro ao carregar mapa: %s", caminhoArquivo );
        free( novoMapa );
        return NULL;
    }

    char *caractereAtual = dadosMapa;
    int linhaAtual = 0;
    int colunaAtual = 0;

    while ( *caractereAtual != '\0' ) {

        char c = *caractereAtual;

        if ( c == '\n' ) {

            linhaAtual++;
            colunaAtual = 0;
            novoMapa->linhas = linhaAtual;

        } else {

            if ( c != ' ' ) {

                ElementoMapa *el = (ElementoMapa*) malloc( sizeof( ElementoMapa ) );
                el->proximo = NULL;
                el->objeto = NULL;

                if ( c >= 'A' && c <= 'Z' ) {

                    int deslocamento = c - 'A';

                    el->objeto = criarObstaculo(
                        (Rectangle){
                            .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                            .y = novoMapa->dimensaoPadraoElementos * linhaAtual,
                            .width = novoMapa->dimensaoPadraoElementos,
                            .height = novoMapa->dimensaoPadraoElementos
                        },
                        GRAY,
                        (Rectangle){
                            1 + ( novoMapa->dimensaoPadraoElementos + 1 ) * deslocamento,
                            1,
                            novoMapa->dimensaoPadraoElementos,
                            novoMapa->dimensaoPadraoElementos
                        },
                        &rm.texturaTerreno
                    );

                    el->tipo = TIPO_ELEMENTO_MAPA_OBSTACULO;
                    inserirObstaculo( novoMapa, el );

                } else if ( c >= 'a' && c <= 'z' ) {

                    Item *item = NULL;

                    switch ( c ) {

                        case 'a':

                            item = criarItem( TIPO_ITEM_ANEL );

                            item->objeto = criarItemAnel(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual + 8,
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual + 5,
                                    .width = 32,
                                    .height = 32
                                },
                                YELLOW
                            );

                            el->objeto = item;
                            el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                            break;

                        case 'e':

                            item = criarItem( TIPO_ITEM_ESCUDO );

                            item->objeto = criarItemEscudo(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual - 4,
                                    .y = calcularYPowerUpNoChao(
                                        dadosMapa,
                                        linhaAtual,
                                        colunaAtual,
                                        novoMapa->dimensaoPadraoElementos,
                                        56.0f,
                                        novoMapa->dimensaoPadraoElementos * linhaAtual + 8
                                    ),
                                    .width = 56,
                                    .height = 56
                                }
                            );

                            el->objeto = item;
                            el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                            break;

                        case 's':

                            item = criarItem( TIPO_ITEM_ESTRELA );

                            item->objeto = criarItemEstrela(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual - 4,
                                    .y = calcularYPowerUpNoChao(
                                        dadosMapa,
                                        linhaAtual,
                                        colunaAtual,
                                        novoMapa->dimensaoPadraoElementos,
                                        56.0f,
                                        novoMapa->dimensaoPadraoElementos * linhaAtual + 8
                                    ),
                                    .width = 56,
                                    .height = 56
                                }
                            );

                            el->objeto = item;
                            el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                            break;

                        case 'm':

                            item = criarItem( TIPO_ITEM_MOLA );

                            item->objeto = criarItemMola(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual - 2,
                                    .y = calcularYPowerUpNoChao(
                                        dadosMapa,
                                        linhaAtual,
                                        colunaAtual,
                                        novoMapa->dimensaoPadraoElementos,
                                        34.0f,
                                        novoMapa->dimensaoPadraoElementos * linhaAtual + 14
                                    ),
                                    .width = 52,
                                    .height = 34
                                }
                            );

                            el->objeto = item;
                            el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                            break;

                        case 'p':

                            item = criarItem( TIPO_ITEM_ESPINHO );

                            item->objeto = criarItemEspinho(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual - 4,
                                    .y = calcularYPowerUpNoChao(
                                        dadosMapa,
                                        linhaAtual,
                                        colunaAtual,
                                        novoMapa->dimensaoPadraoElementos,
                                        36.0f,
                                        novoMapa->dimensaoPadraoElementos * linhaAtual + 6
                                    ),
                                    .width = 56,
                                    .height = 42
                                }
                            );

                            el->objeto = item;
                            el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                            break;

                        default:
                            TraceLog( LOG_ERROR, "Tipo de item desconhecido: %c", c );
                            free( el );
                            abort();
                            break;

                    }

                    inserirItem( novoMapa, el );

                } else if ( c >= '0' && c <= '9' ) {

                    Inimigo *inimigo = NULL;

                    switch ( c ) {

                        case '0':

                            inimigo = criarInimigo( TIPO_INIMIGO_MOTOBUG );

                            inimigo->objeto = criarInimigoMotobug(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual - 12,
                                    .width = 80,
                                    .height = 60
                                },
                                YELLOW
                            );

                            el->objeto = inimigo;
                            el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                            break;

                        case '1':

                            inimigo = criarInimigo( TIPO_INIMIGO_BUZZBOMBER );

                            inimigo->objeto = criarInimigoBuzzBomber(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual - 48,
                                    .width = 98,
                                    .height = 50
                                },
                                BLUE
                            );

                            el->objeto = inimigo;
                            el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                            break;

                        case '2':

                            inimigo = criarInimigo( TIPO_INIMIGO_CARANGUEJO );

                            inimigo->objeto = criarInimigoCaranguejo(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual - 20,
                                    .width = 98,
                                    .height = 68
                                },
                                RED
                            );

                            el->objeto = inimigo;
                            el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                            break;

                        case '3':

                            inimigo = criarInimigo( TIPO_INIMIGO_MOSQUITO );

                            inimigo->objeto = criarInimigoMosquito(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual - 40,
                                    .width = 74,
                                    .height = 50
                                },
                                PURPLE
                            );

                            el->objeto = inimigo;
                            el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                            break;

                        default:
                            TraceLog( LOG_ERROR, "Tipo de inimigo desconhecido: %c", c );
                            free( el );
                            abort();
                            break;

                    }

                    inserirInimigo( novoMapa, el );

                } else {

                    TraceLog( LOG_ERROR, "Entidade inválida no mapa: %c", c );
                    free( el );
                    abort();

                }

            }

            colunaAtual++;

            if ( novoMapa->colunas < colunaAtual ) {
                novoMapa->colunas = colunaAtual;
            }

        }

        caractereAtual++;

    }

    novoMapa->linhas++;

    UnloadFileText( dadosMapa );

    return novoMapa;

}

static float calcularYPowerUpNoChao(
    const char *dadosMapa,
    int linhaAtual,
    int colunaAtual,
    float tamanhoTile,
    float alturaItem,
    float yFallback
) {

    int linha = 0;
    int coluna = 0;

    for ( const char *p = dadosMapa; *p != '\0'; p++ ) {

        if ( *p == '\n' ) {
            linha++;
            coluna = 0;
            continue;
        }

        if ( linha > linhaAtual && coluna == colunaAtual && *p >= 'A' && *p <= 'Z' ) {
            return linha * tamanhoTile - alturaItem;
        }

        coluna++;

    }

    return yFallback;

}

void destruirMapa( Mapa *m ) {

    if ( m != NULL ) {

        ElementoMapa *el = NULL;

        el = m->obstaculos;
        while ( el != NULL ) {

            destruirObstaculo( (Obstaculo*) el->objeto );

            ElementoMapa *t = el;
            el = el->proximo;
            free( t );

        }

        el = m->itens;
        while ( el != NULL ) {

            destruirItem( (Item*) el->objeto );

            ElementoMapa *t = el;
            el = el->proximo;
            free( t );

        }

        el = m->inimigos;
        while ( el != NULL ) {

            destruirInimigo( (Inimigo*) el->objeto );

            ElementoMapa *t = el;
            el = el->proximo;
            free( t );

        }

        free( m );

    }

}

void atualizarMapa( Mapa *m, GameWorld *gw, float delta ) {

    ElementoMapa *el = NULL;

    el = m->itens;
    while ( el != NULL ) {

        atualizarItem( (Item*) el->objeto, delta );

        el = el->proximo;

    }

    el = m->inimigos;
    while ( el != NULL ) {

        atualizarInimigo( (Inimigo*) el->objeto, gw, delta );

        el = el->proximo;

    }

}

void desenharMapa( Mapa *m ) {

    ElementoMapa *el = NULL;

    el = m->obstaculos;
    while ( el != NULL ) {

        desenharObstaculo( (Obstaculo*) el->objeto );

        el = el->proximo;

    }

    /*
     * IMPORTANTE:
     * Antes o Mapa.c desenhava escudo e estrela manualmente aqui,
     * usando texturaMonitores e ícones antigos.
     *
     * Agora ele chama desenharItem(item), então cada item se desenha
     * pelo próprio arquivo:
     *
     * - ItemAnel.c
     * - ItemEscudo.c
     * - ItemEstrela.c
     *
     * Isso remove o sprite antigo que estava ficando preso na TV.
     */
    el = m->itens;
    while ( el != NULL ) {

        desenharItem( (Item*) el->objeto );

        el = el->proximo;

    }

    el = m->inimigos;
    while ( el != NULL ) {

        desenharInimigo( (Inimigo*) el->objeto );

        el = el->proximo;

    }

}

int calcularLarguraMapa( Mapa *m ) {
    return (int) ( m->dimensaoPadraoElementos * m->colunas );
}

int calcularAlturaMapa( Mapa *m ) {
    return (int) ( m->dimensaoPadraoElementos * m->linhas );
}

static void inserirObstaculo( Mapa *mapa, ElementoMapa *obstaculo ) {

    if ( mapa->obstaculos == NULL ) {
        mapa->obstaculos = obstaculo;
    } else {
        obstaculo->proximo = mapa->obstaculos;
        mapa->obstaculos = obstaculo;
    }

    mapa->quantidadeObstaculos++;

}

static void inserirItem( Mapa *mapa, ElementoMapa *item ) {

    if ( mapa->itens == NULL ) {
        mapa->itens = item;
    } else {
        item->proximo = mapa->itens;
        mapa->itens = item;
    }

    mapa->quantidadeItens++;

}

static void inserirInimigo( Mapa *mapa, ElementoMapa *inimigo ) {

    if ( mapa->inimigos == NULL ) {
        mapa->inimigos = inimigo;
    } else {
        inimigo->proximo = mapa->inimigos;
        mapa->inimigos = inimigo;
    }

    mapa->quantidadeInimigos++;

}
