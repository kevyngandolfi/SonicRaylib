/**
 * @file ResourceManager.h
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager struct and function declarations.
 * 
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "raylib/raylib.h"

typedef struct ResourceManager {

    Texture2D texturaJogador;
    Texture2D texturaBadniks;
    Texture2D texturaItens;
    Texture2D texturaMonitores;
    Texture2D texturaBuzzBomber;
    Texture2D texturaMosquito;
    Texture2D texturaCaranguejo;

    Texture2D texturaTerreno;
    Texture2D texturaMola;
    Texture2D texturaSpikes;
    Texture2D texturaPlaca;

    Texture2D texturaFundo;
    Texture2D texturaFundoFase02;
    Texture2D texturaFundoFase03;
    Texture2D texturaTelaMenu;
    Texture2D texturaTelaTransicao;
    Texture2D texturaTituloMenu;

    Texture2D texturaHUD;
    Texture2D texturaHUDVidas;

    Sound somAnel;
    Sound somFrenagem;
    Sound somHitComAnel;
    Sound somHitInimigo;
    Sound somMorte;
    Sound somPulo;
    Sound somMola;

    Music musicaFase01;
    Music musicaFase02;

} ResourceManager;

extern ResourceManager rm;

void loadResourcesResourceManager( void );
void unloadResourcesResourceManager( void );
