/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações de funções do GameWorld.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

GameWorld *createGameWorld( void );

void destroyGameWorld( GameWorld *gw );

void updateGameWorld( GameWorld *gw, float delta );

void drawGameWorld( GameWorld *gw );