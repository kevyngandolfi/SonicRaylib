#pragma once

#include "raylib/raylib.h"
#include "Tipos.h"

ItemEspinho *criarItemEspinho( Rectangle ret );
void destruirItemEspinho( ItemEspinho *itemEspinho );
void desenharItemEspinho( ItemEspinho *itemEspinho );
