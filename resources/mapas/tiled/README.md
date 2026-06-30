# Editando os mapas no Tiled

Abra no Tiled um destes arquivos:

- `mapa01.tmx`
- `mapa02.tmx`
- `mapa03.tmx`

Cada quadrado representa uma letra do mapa `.txt` original. Depois de editar e salvar no Tiled, converta de volta para o jogo:

```bash
python3 tools/mapa_tiled.py import resources/mapas/tiled/mapa01.tmx resources/mapas/mapa01.txt
python3 tools/mapa_tiled.py import resources/mapas/tiled/mapa02.tmx resources/mapas/mapa02.txt
python3 tools/mapa_tiled.py import resources/mapas/tiled/mapa03.tmx resources/mapas/mapa03.txt
```

Para recriar os arquivos do Tiled a partir dos `.txt`:

```bash
python3 tools/mapa_tiled.py export resources/mapas/mapa01.txt
python3 tools/mapa_tiled.py export resources/mapas/mapa02.txt
python3 tools/mapa_tiled.py export resources/mapas/mapa03.txt
```

No tileset:

- Letras maiusculas sao blocos/terreno.
- Letras minusculas sao itens, molas, espinhos e extras.
- Numeros sao inimigos.
- Espaco vazio fica sem tile.
