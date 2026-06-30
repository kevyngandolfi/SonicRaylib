/**
 * @file ResourceManager.c
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "ResourceManager.h"
#include "Utils.h"

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {

    rm.texturaJogador = carregarTexturaAlterandoCores( 
        "resources/imagens/sprites/sonic.png",
        (Color[]) { { 37, 102, 26, 255 }, { 13, 72, 7, 255 }, },
        (Color[]) { BLANK, BLANK, }, 2
    );

    rm.texturaBadniks = carregarTexturaAlterandoCores( 
        "resources/imagens/sprites/badniks.png",
        (Color[]) { { 13, 72, 7, 255 }, },
        (Color[]) { BLANK, }, 1
    );

    rm.texturaBuzzBomber = LoadTexture( "resources/imagens/sprites/buzzbomber.png" );
    rm.texturaMosquito = LoadTexture( "resources/imagens/sprites/recortes/mosquito_recorte.png" );
    rm.texturaCaranguejo = LoadTexture( "resources/imagens/sprites/crabmeat.png" );
    rm.texturaMonitores = carregarTexturaAlterandoCores(
        "resources/imagens/fundo/monitores_mini.png",
        (Color[]) { { 37, 102, 26, 255 } },
        (Color[]) { BLANK },
        1
    );

    rm.texturaItens = carregarTexturaAlterandoCores( 
        "resources/imagens/itens/itens.png",
        (Color[]) { { 16, 112, 132, 255 }, },
        (Color[]) { BLANK, }, 1
    );

    rm.texturaTerreno = LoadTexture( "resources/imagens/tiles/terreno.png" );
    rm.texturaMola = LoadTexture( "resources/imagens/itens/recortes/mola.png" );
    rm.texturaSpikes = LoadTexture( "resources/imagens/itens/recortes/spikes.png" );
    rm.texturaPlaca = LoadTexture( "resources/imagens/itens/recortes/placa.png" );

    rm.texturaFundo = LoadTexture( "resources/imagens/fundo/fundo.png" );

    /*
     * Fundo da fase 2.
     * Se esse arquivo não existir no teu projeto, troca para "resources/imagens/fundo/fundo.png".
     */
    rm.texturaFundoFase02 = LoadTexture( "resources/imagens/fundo/recortes/fundo_splash.png" );
    rm.texturaFundoFase03 = LoadTexture( "resources/imagens/fundo/recortes/montanhas_transicao.png" );
    rm.texturaTelaMenu = LoadTexture( "resources/imagens/fundo/recortes/ceu_menu.png" );
    rm.texturaTelaTransicao = LoadTexture( "resources/imagens/fundo/recortes/montanhas_transicao.png" );
    rm.texturaTituloMenu = LoadTexture( "resources/imagens/fundo/recortes/titulo_sonic.png" );

    rm.texturaHUD = LoadTexture( "resources/imagens/hud/hudMelhorado.png" );
    rm.texturaHUDVidas = LoadTexture( "resources/imagens/hud/hudVidas.png" );

    SetTextureFilter( rm.texturaJogador, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaBadniks, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaBuzzBomber, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaMosquito, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaCaranguejo, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaItens, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaMonitores, TEXTURE_FILTER_POINT ); 
    SetTextureFilter( rm.texturaTerreno, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaMola, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaSpikes, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaPlaca, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaFundo, TEXTURE_FILTER_POINT );

    if ( rm.texturaFundoFase02.id > 0 ) {
        SetTextureFilter( rm.texturaFundoFase02, TEXTURE_FILTER_POINT );
    }
    if ( rm.texturaFundoFase03.id > 0 ) {
        SetTextureFilter( rm.texturaFundoFase03, TEXTURE_FILTER_POINT );
    }
    SetTextureFilter( rm.texturaTelaMenu, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaTelaTransicao, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaTituloMenu, TEXTURE_FILTER_POINT );

    SetTextureFilter( rm.texturaHUD, TEXTURE_FILTER_POINT );
    SetTextureFilter( rm.texturaHUDVidas, TEXTURE_FILTER_POINT );

    rm.somAnel = LoadSound( "resources/sons/efeitos/anel.wav" );
    rm.somFrenagem = LoadSound( "resources/sons/efeitos/frenagem.wav" );
    rm.somHitComAnel = LoadSound( "resources/sons/efeitos/hit-com-anel.wav" );
    rm.somHitInimigo = LoadSound( "resources/sons/efeitos/hit-inimigo.wav" );
    rm.somMorte = LoadSound( "resources/sons/efeitos/morte.wav" );
    rm.somPulo = LoadSound( "resources/sons/efeitos/pulo.wav" );
    rm.somMola = LoadSound( "resources/sons/efeitos/mola.wav" );

    rm.musicaFase01 = LoadMusicStream( "resources/sons/musicas/green-hill-zone.mp3" );
    rm.musicaFase02 = LoadMusicStream( "resources/sons/musicas/green-hill-zone.mp3" );

}

void unloadResourcesResourceManager( void ) {

    UnloadTexture( rm.texturaJogador );
    UnloadTexture( rm.texturaBadniks );
    UnloadTexture( rm.texturaBuzzBomber );
    UnloadTexture( rm.texturaMosquito );
    UnloadTexture( rm.texturaCaranguejo );
    UnloadTexture( rm.texturaItens );
    UnloadTexture( rm.texturaMonitores ); 
    UnloadTexture( rm.texturaTerreno );
    UnloadTexture( rm.texturaMola );
    UnloadTexture( rm.texturaSpikes );
    UnloadTexture( rm.texturaPlaca );
    UnloadTexture( rm.texturaFundo );

    if ( rm.texturaFundoFase02.id > 0 ) {
        UnloadTexture( rm.texturaFundoFase02 );
    }
    if ( rm.texturaFundoFase03.id > 0 ) {
        UnloadTexture( rm.texturaFundoFase03 );
    }
    UnloadTexture( rm.texturaTelaMenu );
    UnloadTexture( rm.texturaTelaTransicao );
    UnloadTexture( rm.texturaTituloMenu );

    UnloadTexture( rm.texturaHUD );
    UnloadTexture( rm.texturaHUDVidas );

    UnloadSound( rm.somAnel );
    UnloadSound( rm.somFrenagem );
    UnloadSound( rm.somHitComAnel );
    UnloadSound( rm.somHitInimigo );
    UnloadSound( rm.somMorte );
    UnloadSound( rm.somPulo );
    UnloadSound( rm.somMola );
    
    UnloadMusicStream( rm.musicaFase01 );
    UnloadMusicStream( rm.musicaFase02 );

}
