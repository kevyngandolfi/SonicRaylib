#pragma once

#include "raylib/raylib.h"
#include "Tipos.h"

ItemMola *criarItemMola( Rectangle ret );
void destruirItemMola( ItemMola *itemMola );
void atualizarItemMola( ItemMola *itemMola, float delta );
void acionarItemMola( ItemMola *itemMola );
void desenharItemMola( ItemMola *itemMola );
