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
import os
import sys
import tempfile


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
            for row in range(4):
                column = x + ((row + 1) % 2)
                self.board[row][column] = BlackDraught()
                self.board[row + 6][column] = WhiteDraught()


class ChessBoard(AbstractBoard):

    def __init__(self):
        super().__init__(8, 8)


    def populate_board(self):
        self.board[0][0] = BlackChessRook()
        self.board[0][1] = BlackChessKnight()
        self.board[0][2] = BlackChessBishop()
        self.board[0][3] = BlackChessQueen()
        self.board[0][4] = BlackChessKing()
        self.board[0][5] = BlackChessBishop()
        self.board[0][6] = BlackChessKnight()
        self.board[0][7] = BlackChessRook()
        self.board[7][0] = WhiteChessRook()
        self.board[7][1] = WhiteChessKnight()
        self.board[7][2] = WhiteChessBishop()
        self.board[7][3] = WhiteChessQueen()
        self.board[7][4] = WhiteChessKing()
        self.board[7][5] = WhiteChessBishop()
        self.board[7][6] = WhiteChessKnight()
        self.board[7][7] = WhiteChessRook()
        for column in range(8):
            self.board[1][column] = BlackChessPawn()
            self.board[6][column] = WhiteChessPawn()


class Piece(str):

    __slots__ = ()


class BlackDraught(Piece):

    __slots__ = ()

    def __new__(Class):
        return super().__new__(Class, "\N{black draughts man}")


class WhiteDraught(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{white draughts man}")


class BlackChessKing(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{black chess king}")


class WhiteChessKing(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{white chess king}")


class BlackChessQueen(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{black chess queen}")


class WhiteChessQueen(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{white chess queen}")


class BlackChessRook(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{black chess rook}")


class WhiteChessRook(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{white chess rook}")


class BlackChessBishop(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{black chess bishop}")


class WhiteChessBishop(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{white chess bishop}")


class BlackChessKnight(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{black chess knight}")


class WhiteChessKnight(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{white chess knight}")


class BlackChessPawn(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{black chess pawn}")


class WhiteChessPawn(Piece):

    __slots__ = ()
    
    def __new__(Class):
        return super().__new__(Class, "\N{white chess pawn}")


if __name__ == "__main__":
    main()
