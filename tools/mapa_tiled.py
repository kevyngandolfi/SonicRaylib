#!/usr/bin/env python3
"""
Converte os mapas .txt do jogo para .tmx editavel no Tiled e de volta.

Uso:
  python3 tools/mapa_tiled.py export resources/mapas/mapa02.txt
  python3 tools/mapa_tiled.py import resources/mapas/tiled/mapa02.tmx resources/mapas/mapa02.txt

No Tiled:
  - Abra o arquivo .tmx gerado em resources/mapas/tiled/
  - Edite usando o tileset de letras
  - Salve
  - Rode o comando import para voltar ao .txt do jogo
"""

from __future__ import annotations

import argparse
import csv
import math
import struct
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


TILE_SIZE = 48
TILESET_COLUMNS = 8

CHAR_ORDER = (
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
)

CHAR_TO_GID = {char: index + 1 for index, char in enumerate(CHAR_ORDER)}
GID_TO_CHAR = {gid: char for char, gid in CHAR_TO_GID.items()}

FONT_5X7 = {
    "0": ["01110", "10001", "10011", "10101", "11001", "10001", "01110"],
    "1": ["00100", "01100", "00100", "00100", "00100", "00100", "01110"],
    "2": ["01110", "10001", "00001", "00010", "00100", "01000", "11111"],
    "3": ["11110", "00001", "00001", "01110", "00001", "00001", "11110"],
    "4": ["00010", "00110", "01010", "10010", "11111", "00010", "00010"],
    "5": ["11111", "10000", "10000", "11110", "00001", "00001", "11110"],
    "6": ["01110", "10000", "10000", "11110", "10001", "10001", "01110"],
    "7": ["11111", "00001", "00010", "00100", "01000", "01000", "01000"],
    "8": ["01110", "10001", "10001", "01110", "10001", "10001", "01110"],
    "9": ["01110", "10001", "10001", "01111", "00001", "00001", "01110"],
    "A": ["01110", "10001", "10001", "11111", "10001", "10001", "10001"],
    "B": ["11110", "10001", "10001", "11110", "10001", "10001", "11110"],
    "C": ["01111", "10000", "10000", "10000", "10000", "10000", "01111"],
    "D": ["11110", "10001", "10001", "10001", "10001", "10001", "11110"],
    "E": ["11111", "10000", "10000", "11110", "10000", "10000", "11111"],
    "F": ["11111", "10000", "10000", "11110", "10000", "10000", "10000"],
    "G": ["01111", "10000", "10000", "10011", "10001", "10001", "01111"],
    "H": ["10001", "10001", "10001", "11111", "10001", "10001", "10001"],
    "I": ["11111", "00100", "00100", "00100", "00100", "00100", "11111"],
    "J": ["00111", "00010", "00010", "00010", "00010", "10010", "01100"],
    "K": ["10001", "10010", "10100", "11000", "10100", "10010", "10001"],
    "L": ["10000", "10000", "10000", "10000", "10000", "10000", "11111"],
    "M": ["10001", "11011", "10101", "10101", "10001", "10001", "10001"],
    "N": ["10001", "11001", "10101", "10011", "10001", "10001", "10001"],
    "O": ["01110", "10001", "10001", "10001", "10001", "10001", "01110"],
    "P": ["11110", "10001", "10001", "11110", "10000", "10000", "10000"],
    "Q": ["01110", "10001", "10001", "10001", "10101", "10010", "01101"],
    "R": ["11110", "10001", "10001", "11110", "10100", "10010", "10001"],
    "S": ["01111", "10000", "10000", "01110", "00001", "00001", "11110"],
    "T": ["11111", "00100", "00100", "00100", "00100", "00100", "00100"],
    "U": ["10001", "10001", "10001", "10001", "10001", "10001", "01110"],
    "V": ["10001", "10001", "10001", "10001", "10001", "01010", "00100"],
    "W": ["10001", "10001", "10001", "10101", "10101", "10101", "01010"],
    "X": ["10001", "10001", "01010", "00100", "01010", "10001", "10001"],
    "Y": ["10001", "10001", "01010", "00100", "00100", "00100", "00100"],
    "Z": ["11111", "00001", "00010", "00100", "01000", "10000", "11111"],
}


def bg_for_char(char: str) -> tuple[int, int, int]:
    if char.isupper():
        if char == "M":
            return (118, 103, 150)
        return (172, 91, 22)
    if char.isdigit():
        return (120, 38, 38)
    return (28, 96, 92)


def fg_for_char(char: str) -> tuple[int, int, int]:
    if char.islower():
        return (255, 232, 48)
    return (255, 255, 255)


def set_px(pixels: list[list[tuple[int, int, int]]], x: int, y: int, color: tuple[int, int, int]) -> None:
    if 0 <= y < len(pixels) and 0 <= x < len(pixels[0]):
        pixels[y][x] = color


def fill_rect(
    pixels: list[list[tuple[int, int, int]]],
    x: int,
    y: int,
    w: int,
    h: int,
    color: tuple[int, int, int],
) -> None:
    for py in range(y, y + h):
        for px in range(x, x + w):
            set_px(pixels, px, py, color)


def draw_char(
    pixels: list[list[tuple[int, int, int]]],
    char: str,
    tile_x: int,
    tile_y: int,
) -> None:
    bg = bg_for_char(char)
    fg = fg_for_char(char)
    x0 = tile_x * TILE_SIZE
    y0 = tile_y * TILE_SIZE

    fill_rect(pixels, x0, y0, TILE_SIZE, TILE_SIZE, bg)
    fill_rect(pixels, x0, y0 + TILE_SIZE - 8, TILE_SIZE, 8, tuple(max(0, c - 45) for c in bg))

    label = char.upper()
    pattern = FONT_5X7.get(label, FONT_5X7["X"])
    scale = 5
    start_x = x0 + (TILE_SIZE - 5 * scale) // 2
    start_y = y0 + 7

    for row_index, row in enumerate(pattern):
        for col_index, bit in enumerate(row):
            if bit == "1":
                fill_rect(
                    pixels,
                    start_x + col_index * scale,
                    start_y + row_index * scale,
                    scale - 1,
                    scale - 1,
                    fg,
                )

    if char.islower():
        fill_rect(pixels, x0 + 5, y0 + 5, 8, 8, (255, 230, 40))
        fill_rect(pixels, x0 + 7, y0 + 7, 4, 4, (30, 30, 30))
    elif char.isdigit():
        fill_rect(pixels, x0 + 5, y0 + 5, 8, 8, (255, 70, 70))

    # Tile border helps visual editing in Tiled.
    for i in range(TILE_SIZE):
        set_px(pixels, x0 + i, y0, (30, 30, 30))
        set_px(pixels, x0 + i, y0 + TILE_SIZE - 1, (30, 30, 30))
        set_px(pixels, x0, y0 + i, (30, 30, 30))
        set_px(pixels, x0 + TILE_SIZE - 1, y0 + i, (30, 30, 30))


def write_bmp(path: Path, pixels: list[list[tuple[int, int, int]]]) -> None:
    height = len(pixels)
    width = len(pixels[0])
    row_size = (width * 3 + 3) & ~3
    image_size = row_size * height
    file_size = 14 + 40 + image_size

    with path.open("wb") as f:
        f.write(b"BM")
        f.write(struct.pack("<IHHI", file_size, 0, 0, 14 + 40))
        f.write(struct.pack("<IIIHHIIIIII", 40, width, height, 1, 24, 0, image_size, 2835, 2835, 0, 0))

        padding = b"\0" * (row_size - width * 3)
        for row in reversed(pixels):
            for r, g, b in row:
                f.write(bytes((b, g, r)))
            f.write(padding)


def ensure_tileset_image(out_dir: Path) -> Path:
    tile_count = len(CHAR_ORDER)
    rows = math.ceil(tile_count / TILESET_COLUMNS)
    width = TILESET_COLUMNS * TILE_SIZE
    height = rows * TILE_SIZE
    pixels = [[(0, 0, 0) for _ in range(width)] for _ in range(height)]

    for index, char in enumerate(CHAR_ORDER):
        draw_char(pixels, char, index % TILESET_COLUMNS, index // TILESET_COLUMNS)

    image_path = out_dir / "mapa_tileset.bmp"
    write_bmp(image_path, pixels)
    return image_path


def read_txt_map(path: Path) -> list[str]:
    text = path.read_text(encoding="utf-8").splitlines()
    if not text:
        raise ValueError(f"Mapa vazio: {path}")
    width = max(len(line) for line in text)
    return [line.ljust(width) for line in text]


def write_tmx(txt_path: Path, out_path: Path) -> None:
    rows = read_txt_map(txt_path)
    height = len(rows)
    width = len(rows[0])
    out_path.parent.mkdir(parents=True, exist_ok=True)
    image_path = ensure_tileset_image(out_path.parent)

    tile_count = len(CHAR_ORDER)
    rows_tileset = math.ceil(tile_count / TILESET_COLUMNS)

    csv_rows = []
    for row in rows:
        csv_rows.append(",".join(str(CHAR_TO_GID.get(char, 0)) for char in row))

    map_el = ET.Element(
        "map",
        {
            "version": "1.10",
            "tiledversion": "1.11.2",
            "orientation": "orthogonal",
            "renderorder": "right-down",
            "width": str(width),
            "height": str(height),
            "tilewidth": str(TILE_SIZE),
            "tileheight": str(TILE_SIZE),
            "infinite": "0",
            "nextlayerid": "2",
            "nextobjectid": "1",
        },
    )
    tileset = ET.SubElement(
        map_el,
        "tileset",
        {
            "firstgid": "1",
            "name": "mapa_chars",
            "tilewidth": str(TILE_SIZE),
            "tileheight": str(TILE_SIZE),
            "tilecount": str(tile_count),
            "columns": str(TILESET_COLUMNS),
        },
    )
    ET.SubElement(
        tileset,
        "image",
        {
            "source": image_path.name,
            "width": str(TILESET_COLUMNS * TILE_SIZE),
            "height": str(rows_tileset * TILE_SIZE),
        },
    )
    layer = ET.SubElement(
        map_el,
        "layer",
        {"id": "1", "name": "Mapa", "width": str(width), "height": str(height)},
    )
    data = ET.SubElement(layer, "data", {"encoding": "csv"})
    data.text = "\n" + ",\n".join(csv_rows) + "\n"

    ET.indent(map_el)
    tree = ET.ElementTree(map_el)
    tree.write(out_path, encoding="UTF-8", xml_declaration=True)


def read_csv_data(data_text: str) -> list[list[int]]:
    rows = []
    for row in csv.reader(line for line in data_text.strip().splitlines() if line.strip()):
        rows.append([int(value.strip() or "0") for value in row])
    return rows


def import_tmx(tmx_path: Path, txt_path: Path) -> None:
    root = ET.parse(tmx_path).getroot()
    layer = root.find("layer")
    if layer is None:
        raise ValueError("TMX sem camada <layer>")
    data = layer.find("data")
    if data is None or data.text is None:
        raise ValueError("TMX sem dados CSV")

    rows = read_csv_data(data.text)
    text_rows = []
    for row in rows:
        text_rows.append("".join(GID_TO_CHAR.get(gid, " ") if gid > 0 else " " for gid in row))

    txt_path.write_text("\n".join(text_rows) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description="Converte mapas .txt do jogo para Tiled .tmx e de volta.")
    sub = parser.add_subparsers(dest="cmd", required=True)

    export = sub.add_parser("export", help="Converte .txt para .tmx")
    export.add_argument("txt", type=Path)
    export.add_argument("tmx", type=Path, nargs="?")

    import_cmd = sub.add_parser("import", help="Converte .tmx salvo no Tiled para .txt")
    import_cmd.add_argument("tmx", type=Path)
    import_cmd.add_argument("txt", type=Path)

    args = parser.parse_args()

    if args.cmd == "export":
        out_path = args.tmx
        if out_path is None:
            out_path = Path("resources/mapas/tiled") / (args.txt.stem + ".tmx")
        write_tmx(args.txt, out_path)
        print(f"Gerado: {out_path}")
        return 0

    if args.cmd == "import":
        import_tmx(args.tmx, args.txt)
        print(f"Gerado: {args.txt}")
        return 0

    return 1


if __name__ == "__main__":
    raise SystemExit(main())
