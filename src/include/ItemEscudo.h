#pragma once

#include "raylib/raylib.h"
#include "Tipos.h"

ItemEscudo *criarItemEscudo( Rectangle ret );
void destruirItemEscudo( ItemEscudo *itemEscudo );
void desenharItemEscudo( ItemEscudo *itemEscudo );