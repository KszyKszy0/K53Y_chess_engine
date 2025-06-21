#pragma once
#include "bitboard.h"
#include "position.h"
#include "move.h"
#include "moveList.h"



//Generates moves for knights bishops rooks and queens
void generateMoves(Position& pos, Bitboard target, MoveList& moveList, Bitboard whatCanMove);

// 0 - quiets, 1 - captures
void generatePawnMoves(Position& pos, Bitboard target, MoveList& moveList, int type, Bitboard pins);

//Generates moves from to doesnt care if legal
void addMoves(int startSquare, Bitboard targets, MoveList& moveList, Position& pos);

//Returns all legal moves list
void fullMovesList(Position& pos, MoveList& moveList);

//Returns all legal moves list
void fullCapturesList(Position& pos, MoveList& moveList);


//Returns Bitboards containing all squares attacked by enemies
//forKingMoves ignores king as blocker in consideration which is useful because of checking legal moves and king "hiding square" behind him
Bitboard getSideAttacks(Position& pos, bool white, bool forKingMoves);

//Counts how many pieces attack given square
Bitboard howManyAttacks(Position& pos, bool white, int index);

//Generates king moves
void generateKingsMoves(Position& pos, MoveList& moveList, Bitboard target, bool white, int checks);

//Returns Bitboard containing all pinned pieces
Bitboard getPinners(Position& pos, bool white, MoveList& moveList, int checks, Bitboard captureTarget = MAX);


//Adds pawn moves that are not captures can be promotions
void addPawnWhiteQuiet(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

//Adds pawn moves that are captures can be promotions
void addPawnWhiteCaptures(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

//Adds all legal enpassant moves
void addPawnWhiteEnpassant(MoveList& moveList, Position& pos, Bitboard target, int enPassantSquare);


//Adds pawn moves that are not captures can be promotions
void addPawnBlackQuiet(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

//Adds pawn moves that are captures can be promotions
void addPawnBlackCaptures(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

//Adds all legal enpassant moves
void addPawnBlackEnpassant(MoveList& moveList, Position& pos, Bitboard target, int enPassantSquare);



//Adds moves from lookup considering target
void addKnightsMoves(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

//Adds moves from lookup considering target
void addBishopsMoves(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

//Adds moves from lookup considering target
void addRookMoves(int startSquare, MoveList& moveList, Position& pos, Bitboard target);

//Creates moves from given start and target squares and flag (special moves)
void addMoves(int startSquare, Bitboard targers, MoveList& moveList, Position& pos, int flag);

//Creates moves from given start and target squares (captures and quiets)
void addMoves(int startSquare,int index,int flags, MoveList& moveList);