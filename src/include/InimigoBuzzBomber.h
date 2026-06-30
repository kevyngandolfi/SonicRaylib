/**
 * @file InimigoBuzzBomber.h
 * @brief Declarações do Inimigo (BuzzBomber).
 */
#pragma once
#include "Tipos.h"

InimigoBuzzBomber *criarInimigoBuzzBomber( Rectangle ret, Color cor );
InimigoBuzzBomber *criarInimigoMosquito( Rectangle ret, Color cor );
void destruirInimigoBuzzBomber( InimigoBuzzBomber *inimigo );
void atualizarInimigoBuzzBomber( InimigoBuzzBomber *inimigo, GameWorld *gw, float delta );
void desenharInimigoBuzzBomber( InimigoBuzzBomber *inimigo );
QuadroAnimacao *getQuadroAnimacaoAtualInimigoBuzzBomber( InimigoBuzzBomber *inimigo );
