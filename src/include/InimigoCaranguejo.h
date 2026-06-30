/**
 * @file InimigoCaranguejo.h
 * @brief Declarações do Inimigo (Crabmeat).
 */
#pragma once
#include "Tipos.h"

InimigoCaranguejo *criarInimigoCaranguejo( Rectangle ret, Color cor );
void destruirInimigoCaranguejo( InimigoCaranguejo *inimigo );
void atualizarInimigoCaranguejo( InimigoCaranguejo *inimigo, GameWorld *gw, float delta );
void desenharInimigoCaranguejo( InimigoCaranguejo *inimigo );
QuadroAnimacao *getQuadroAnimacaoAtualInimigoCaranguejo( InimigoCaranguejo *inimigo );