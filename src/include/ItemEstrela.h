#pragma once

#include "raylib/raylib.h"
#include "Tipos.h"

ItemEstrela *criarItemEstrela( Rectangle ret );
void destruirItemEstrela( ItemEstrela *itemEstrela );
void desenharItemEstrela( ItemEstrela *itemEstrela );