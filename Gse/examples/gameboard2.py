#!/usr/bin/env python3
# Copyright © 2012-13 Qtrac Ltd. All rights reserved.
# This program or module is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version. It is provided for
# educational purposes and is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.

import io
import itertools
import os
import sys
import tempfile
import unicodedata


BLACK, WHITE = ("BLACK", "WHITE")


def main():
    checkers = CheckersBoard()
    print(checkers)

    chess = ChessBoard()
    print(chess)

    if sys.platform.startswith("win"):
        filename = os.path.join(tempfile.gettempdir(), "gameboard.txt")
        with open(filename, "w", encoding="utf-8") as file:
            file.write(sys.stdout.getvalue())
        print("wrote '{}'".format(filename), file=sys.__stdout__)


if sys.platform.startswith("win"):
    def console(char, background):
        return char or " "
    sys.stdout = io.StringIO()
else:
    def console(char, background):
        return "\x1B[{}m{}\x1B[0m".format(
                43 if background == BLACK else 47, char or " ")


class AbstractBoard:

    def __init__(self, rows, columns):
        self.board = [[None for _ in range(columns)] for _ in range(rows)]
        self.populate_board()


    def populate_board(self):
        raise NotImplementedError()


    def __str__(self):
        squares = []
        for y, row in enumerate(self.board):
            for x, piece in enumerate(row):
                square = console(piece, BLACK if (y + x) % 2 else WHITE)
                squares.append(square)
            squares.append("\n")
        return "".join(squares)


class CheckersBoard(AbstractBoard):

    def __init__(self):
        super().__init__(10, 10)


    def populate_board(self):
        for x in range(0, 9, 2):
            for y in range(4):
                column = x + ((y + 1) % 2)
                for row, color in ((y, "black"), (y + 6, "white")):
                    self.board[row][column] = create_piece("draught",
                            color)


class ChessBoard(AbstractBoard):

    def __init__(self):
        super().__init__(8, 8)


    def populate_board(self):
        for row, color in ((0, "black"), (7, "white")):
            for columns, kind in (((0, 7), "rook"), ((1, 6), "knight"),
                    ((2, 5), "bishop"), ((3,), "queen"), ((4,), "king")):
                for column in columns:
                    self.board[row][column] = create_piece(kind, color)
        for column in range(8):
            for row, color in ((1, "black"), (6, "white")):
                self.board[row][column] = create_piece("pawn", color)



def create_piece(kind, color):
    if kind == "draught":
        return eval("{}{}()".format(color.title(), kind.title()))
    return eval("{}Chess{}()".format(color.title(), kind.title()))
    # Using eval() is risky


class Piece(str):

    __slots__ = ()


for code in itertools.chain((0x26C0, 0x26C2), range(0x2654, 0x2660)):
    char = chr(code)
    name = unicodedata.name(char).title().replace(" ", "")
    if name.endswith("sMan"):
        name = name[:-4]
    exec("""\
class {}(Piece):

    __slots__ = ()

    def __new__(Class):
        return super().__new__(Class, "{}")""".format(name, char))
    # Using exec() is risky


if __name__ == "__main__":
    main()
