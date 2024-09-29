#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include "enums.h"
#include "move.h"
#include "moveList.h"
#include <vector>


Bitboard rectangularLookup[64][64];

//Array for king moves
Bitboard kingMoves[64];

//Array for knight moves
Bitboard knightMoves[64];

//Array for white pawn moves
Bitboard whitePawnMoves[64];

//Array for white pawn captures
Bitboard whitePawnCaptures[64];

//Array for black pawn moves
Bitboard blackPawnMoves[64];

//Array for black pawn captures
Bitboard blackPawnCaptures[64];

//Vector of legal bishop moves[square][magic]
vector<vector<Bitboard>> bishopMoves;

//Vector of legal rook moves[square][magic]
vector<vector<Bitboard>> rookMoves;

//Movement mask without edges
Bitboard rookMasks[64];

//Movement mask without edges
Bitboard bishopMasks[64];

//List of all blockers for all squares
vector<vector<Bitboard>> bishopBlockers;

//List of all blockers for all squares
vector<vector<Bitboard>> rookBlockers;

void generatePawnMoves(Position &pos, Bitboard target, MoveList &moveList, int type, Bitboard pins)
{
    if (pos.STM == WHITE)
    {
        // pawn moves
        Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
        while (iterated)
        {
            // Get pawn index
            int index = popLSB(iterated);

            // Pin moves are generated in getPinners
            if (!isBitSet(pins, index))
            {
                // If only quiets
                if (type == 0)
                {
                    addPawnWhiteQuiet(index, moveList, pos, target);
                }
                else //Captures only
                {
                    addPawnWhiteCaptures(index, moveList, pos, target);
                }
            }
        }
    }
    else
    {
        // pawn moves
        Bitboard iterated = pos.piecesBitboards[BLACK_PAWN];
        while (iterated)
        {
            // Get pawn index
            int index = popLSB(iterated);

            // Pin moves are generated in getPinners
            if (!isBitSet(pins, index))
            {
                // If only quiets
                if (type == 0)
                {
                    addPawnBlackQuiet(index, moveList, pos, target);
                }
                else //Captures only
                {
                    addPawnBlackCaptures(index, moveList, pos, target);
                }
            }
        }
    }
}

//Go through side bitboard and create moves for each piece
void generateMoves(Position &pos, Bitboard target, MoveList &moveList, Bitboard whatCanMove)
{
    //What can move is bitboard without pinned pieces
    while (whatCanMove)
    {

        //Get index of piece
        int index = popLSB(whatCanMove);

        //Check type of piece
        int pieceType = pos.piecesArray[index];


        //Create moves based on that piece movement

        if ((pieceType == WHITE_KNIGHT) || (pieceType == BLACK_KNIGHT))
            addKnightsMoves(index, moveList, pos, target);

        if ((pieceType == WHITE_BISHOP) || (pieceType == BLACK_BISHOP))
            addBishopsMoves(index, moveList, pos, target);

        if ((pieceType == WHITE_ROOK) || (pieceType == BLACK_ROOK))
            addRookMoves(index, moveList, pos, target);

        if ((pieceType == WHITE_QUEEN) || (pieceType == BLACK_QUEEN))
        {
            addBishopsMoves(index, moveList, pos, target);
            addRookMoves(index, moveList, pos, target);
        }
    }
}

// True white attacks on square, False black attacks on square POINTS on bb
Bitboard howManyAttacks(Position &pos, bool white, int index)
{
    Bitboard attacks = 0;
    // Combine all pieces with its attacks from lookup tables for all pieces that attack square
    //"superpiece" at selected index used for check detection
    if (white)
    {
        attacks |= blackPawnCaptures[index] & pos.piecesBitboards[WHITE_PAWN];
        attacks |= knightMoves[index] & pos.piecesBitboards[WHITE_KNIGHT];
        attacks |= bishopMoves[index][getMagicIndex(pos.piecesBitboards[ALL_PIECES] & bishopMasks[index], bishopsMagics[index], bishopBits[index])] & (pos.piecesBitboards[WHITE_BISHOP] | pos.piecesBitboards[WHITE_QUEEN]);
        attacks |= rookMoves[index][getMagicIndex(pos.piecesBitboards[ALL_PIECES] & rookMasks[index], rooksMagics[index], rookBits[index])] & (pos.piecesBitboards[WHITE_ROOK] | pos.piecesBitboards[WHITE_QUEEN]);
        attacks |= kingMoves[index] & pos.piecesBitboards[WHITE_KING];
    }
    else
    {
        attacks |= whitePawnCaptures[index] & pos.piecesBitboards[BLACK_PAWN];
        attacks |= knightMoves[index] & pos.piecesBitboards[BLACK_KNIGHT];
        attacks |= bishopMoves[index][getMagicIndex(pos.piecesBitboards[ALL_PIECES] & bishopMasks[index], bishopsMagics[index], bishopBits[index])] & (pos.piecesBitboards[BLACK_BISHOP] | pos.piecesBitboards[BLACK_QUEEN]);
        attacks |= rookMoves[index][getMagicIndex(pos.piecesBitboards[ALL_PIECES] & rookMasks[index], rooksMagics[index], rookBits[index])] & (pos.piecesBitboards[BLACK_ROOK] | pos.piecesBitboards[BLACK_QUEEN]);
        attacks |= kingMoves[index] & pos.piecesBitboards[BLACK_KING];
    }
    return attacks;
}

void fullMovesList(Position &pos, MoveList &moveList)
{
    // MoveList that will be used for all substeps

    // Index of king
    int kingIndex = LSB(pos.piecesBitboards[pos.STM ? WHITE_KING : BLACK_KING]);

    // Bitboard of all pieces that attack square
    Bitboard checksAttacks = howManyAttacks(pos, !pos.STM, kingIndex);

    // Amount of checks; >2 skips most of generation
    int checks = popCount(checksAttacks);
    moveList.checks = checks;

    // Rect lookup without the checking piece
    Bitboard checkTargets = 0;

    // Index of checking piece
    int singleCheckIndex = NO_SQUARE;

    // We can only make non-king moves if there is one check
    if (checks == 1)
    {
        // Get index of checking piece
        singleCheckIndex = LSB(checksAttacks);

        // Update check target with rect lookup
        checkTargets |= rectangularLookup[kingIndex][singleCheckIndex];

    }

    // This should be used for king only!!!! attacks on board without king
    Bitboard enemyAttacks = getSideAttacks(pos, !pos.STM, true); // Get enemy attacks

    // Kings Moves
    generateKingsMoves(pos, moveList, ~enemyAttacks, pos.STM, checks);

    // Check targets is for quiets
    // Capture targets is for captures
    if (checks < 2)
    {
        // Pin generation for normal pieces (quiets and captures) and normal pins for pawns (quiet and captures) (without enpassant)
        Bitboard pins = getPinners(pos, pos.STM, moveList, checks);

        // If check it is bb with checking piece ELSE its full bitboard of enemies so it doesnt mess in pins only for one checking piece tho
        Bitboard captureTargets = singleCheckIndex == NO_SQUARE ? MAX : 1ULL << singleCheckIndex;

        // If there is no check we set it to enemies pieces for captures
        captureTargets &= pos.STM ? pos.piecesBitboards[BLACK_PIECES] : pos.piecesBitboards[WHITE_PIECES];





        if (pos.STM == WHITE)
        {
            // Quiets pawns
            generatePawnMoves(pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checkTargets, moveList, 0, pins);

            // All moves for other pieces
            generateMoves(pos, checksAttacks == 0 ? (MAX & ~pos.piecesBitboards[WHITE_PIECES]) : (captureTargets | checkTargets), moveList, pos.piecesBitboards[WHITE_PIECES] & ~pins);

            // Capture pawns
            generatePawnMoves(pos, captureTargets, moveList, 1, pins);
        }
        else
        {
            // Quiets pawns
            generatePawnMoves(pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : checkTargets, moveList, 0, pins);

            // All moves for other pieces
            generateMoves(pos, checksAttacks == 0 ? (MAX & ~pos.piecesBitboards[BLACK_PIECES]) : (captureTargets | checkTargets), moveList, pos.piecesBitboards[BLACK_PIECES] & ~pins);

            // Capture pawns
            generatePawnMoves(pos, captureTargets, moveList, 1, pins);
        }

        // Check enpassantSquare
        int enPassantSquare = pos.stateInfoList[pos.stateCounter].enPassantSquare;

         // Only check en passant if its possible
        if (enPassantSquare != 0)
        {
            //If we can make enpassant capture and we are in one check that means we are checked by pawn
            //OR pawn revealed attack with push for example bishop
            //If it was pawn it means we need to add enpassantSquare to check evasions for pawn movegen
            //If it is any other piece we simply add evasions to check

            //Copy check targets with possibly added enpassantSquare
            Bitboard enpassantCheckCapture = checkTargets;

            //We can only take care of pawn check with enpassant
            //If enpassant is legal and we are in check
            //We can only make enpassant if that pawn is checking us
            if (checks == 1)
            {
                //If piece that is checking us is pawn
                if ((1ULL << singleCheckIndex) & (pos.piecesBitboards[WHITE_PAWN] | pos.piecesBitboards[BLACK_PAWN]))
                {
                    //Set new target bit to the enpassantSquare so we can capture that pawn
                    setBit(enpassantCheckCapture, enPassantSquare);
                }else
                {
                    //If we are checked by something that is not pawn we cannot make enpassant to make evasions

                    //End movelist with 0
                    *moveList.cur++ = 0;
                    return;
                }
            }
            if (pos.STM)
            {
                // Enpassant moves and enpassant pins check targets dont have checking piece bcoz enpassant will never capture it
                //                                 if there are no checks  all empty squares         ELSE checkEvasions + pawn that checks us if its pawn!!
                addPawnWhiteEnpassant(moveList, pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : enpassantCheckCapture, enPassantSquare);
            }
            else
            {
                // Enpassant moves and enpassant pins check targets dont have checking piece bcoz enpassant will never capture it
                addPawnBlackEnpassant(moveList, pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : enpassantCheckCapture, enPassantSquare);
            }
        }
    }

    //End movelist with 0
    *moveList.cur++ = 0;
    return;
}

void fullCapturesList(Position &pos, MoveList &moveList)
{
    // MoveList that will be used for all substeps

    // Index of king
    int kingIndex = LSB(pos.piecesBitboards[pos.STM ? WHITE_KING : BLACK_KING]);

    // Bitboard of all pieces that attack square
    Bitboard checksAttacks = howManyAttacks(pos, !pos.STM, kingIndex);

    // Amount of checks; >2 skips most of generation
    int checks = popCount(checksAttacks);
    moveList.checks = checks;

    // Rect lookup without the checking piece
    Bitboard checkTargets = 0;

    // Index of checking piece
    int singleCheckIndex = NO_SQUARE;

    // We can only make non-king moves if there is one check
    if (checks == 1)
    {
        // Get index of checking piece
        singleCheckIndex = LSB(checksAttacks);

        // Update check target with rect lookup
        checkTargets |= rectangularLookup[kingIndex][singleCheckIndex];

    }

    // This should be used for king only!!!! attacks on board without king
    Bitboard enemyAttacks = getSideAttacks(pos, !pos.STM, true); // Get enemy attacks

    //We only consider captures
    Bitboard enemyPieces = pos.STM ? pos.piecesBitboards[BLACK_PIECES] : pos.piecesBitboards[WHITE_PIECES];


    // Kings Moves
    generateKingsMoves(pos, moveList,enemyPieces &~ enemyAttacks, pos.STM, checks);

    // Check targets is for quiets
    // Capture targets is for captures
    if (checks < 2)
    {
        // Pin generation for normal pieces (quiets and captures) and normal pins for pawns (quiet and captures) (without enpassant)
        Bitboard pins = getPinners(pos, pos.STM, moveList, checks);

        // If check it is bb with checking piece ELSE its full bitboard of enemies so it doesnt mess in pins only for one checking piece tho
        Bitboard captureTargets = singleCheckIndex == NO_SQUARE ? MAX : 1ULL << singleCheckIndex;

        // If there is no check we set it to enemies pieces for captures
        captureTargets &= pos.STM ? pos.piecesBitboards[BLACK_PIECES] : pos.piecesBitboards[WHITE_PIECES];





        if (pos.STM == WHITE)
        {

            // Captures for other pieces
            generateMoves(pos, captureTargets, moveList, pos.piecesBitboards[WHITE_PIECES] & ~pins);

            // Capture pawns
            generatePawnMoves(pos, captureTargets, moveList, 1, pins);
        }
        else
        {

            // Captures for other pieces
            generateMoves(pos, captureTargets, moveList, pos.piecesBitboards[BLACK_PIECES] & ~pins);

            // Capture pawns
            generatePawnMoves(pos, captureTargets, moveList, 1, pins);
        }

        // Check enpassantSquare
        int enPassantSquare = pos.stateInfoList[pos.stateCounter].enPassantSquare;

         // Only check en passant if its possible
        if (enPassantSquare != 0)
        {
            //If we can make enpassant capture and we are in one check that means we are checked by pawn
            //OR pawn revealed attack with push for example bishop
            //If it was pawn it means we need to add enpassantSquare to check evasions for pawn movegen
            //If it is any other piece we simply add evasions to check

            //Copy check targets with possibly added enpassantSquare
            Bitboard enpassantCheckCapture = checkTargets;

            //We can only take care of pawn check with enpassant
            //If enpassant is legal and we are in check
            //We can only make enpassant if that pawn is checking us
            if (checks == 1)
            {
                //If piece that is checking us is pawn
                if ((1ULL << singleCheckIndex) & (pos.piecesBitboards[WHITE_PAWN] | pos.piecesBitboards[BLACK_PAWN]))
                {
                    //Set new target bit to the enpassantSquare so we can capture that pawn
                    setBit(enpassantCheckCapture, enPassantSquare);
                }else
                {
                    //If we are checked by something that is not pawn we cannot make enpassant to make evasions

                    //End movelist with 0
                    *moveList.cur++ = 0;
                    return;
                }
            }
            if (pos.STM)
            {
                // Enpassant moves and enpassant pins check targets dont have checking piece bcoz enpassant will never capture it
                //                                 if there are no checks  all empty squares         ELSE checkEvasions + pawn that checks us if its pawn!!
                addPawnWhiteEnpassant(moveList, pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : enpassantCheckCapture, enPassantSquare);
            }
            else
            {
                // Enpassant moves and enpassant pins check targets dont have checking piece bcoz enpassant will never capture it
                addPawnBlackEnpassant(moveList, pos, checksAttacks == 0 ? pos.piecesBitboards[NO_PIECE] : enpassantCheckCapture, enPassantSquare);
            }
        }
    }

    //End movelist with 0
    *moveList.cur++ = 0;
    return;
}

void addMoves(int startSquare, Bitboard targers, MoveList &moveList, Position &pos)
{
    while (targers)
    {
        int index = popLSB(targers);                          // get piece
        int flags = 0;                                        // init flags default quiet
        if (isBitSet(pos.piecesBitboards[ALL_PIECES], index)) // is capture
        {
            flags = 4; // change flag
        }
        // go to move creator
        *moveList.cur++ = createMove(startSquare, index, flags); // add move to moves list
        moveList.size++;
        // moveList.moveList[moveList.size++]=temp;
    }
}

void addMoves(int startSquare, int index, int flags, MoveList &moveList)
{
    // go to move creator
    *moveList.cur++ = createMove(startSquare, index, flags); // add move to moves list
    moveList.size++;
    // moveList.moveList[moveList.size++]=temp;
}

void addMoves(int startSquare, Bitboard targers, MoveList &moveList, Position &pos, int flag)
{
    while (targers)
    {
        int index = popLSB(targers);
        *moveList.cur++ = createMove(startSquare, index, flag); // add move to moves list
        moveList.size++;
        // moveList.moveList[moveList.size++]=temp; n
    }
}

// Combine all pieces with its attacks from lookup tables for all attacks from side
Bitboard getSideAttacks(Position &pos, bool white, bool forKingMoves)
{

    Bitboard attacks = 0;
    if (white)
    {

        Bitboard target = pos.piecesBitboards[ALL_PIECES];
        if (forKingMoves)
        {
            clearBit(target, LSB(pos.piecesBitboards[BLACK_KING]));
        }

        // pawn moves
        Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= whitePawnCaptures[index];
        }

        // white knights
        iterated = pos.piecesBitboards[WHITE_KNIGHT];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= knightMoves[index];
        }

        // white bishops
        iterated = pos.piecesBitboards[WHITE_BISHOP];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= bishopMoves[index][getMagicIndex(target & bishopMasks[index], bishopsMagics[index], bishopBits[index])];
        }

        // white rooks
        iterated = pos.piecesBitboards[WHITE_ROOK];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= rookMoves[index][getMagicIndex(target & rookMasks[index], rooksMagics[index], rookBits[index])];
        }

        // white queens
        iterated = pos.piecesBitboards[WHITE_QUEEN];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= rookMoves[index][getMagicIndex(target & rookMasks[index], rooksMagics[index], rookBits[index])];
            attacks |= bishopMoves[index][getMagicIndex(target & bishopMasks[index], bishopsMagics[index], bishopBits[index])];
        }

        // white kings
        iterated = pos.piecesBitboards[WHITE_KING];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= kingMoves[index];
        }
    }
    else
    {

        Bitboard target = pos.piecesBitboards[ALL_PIECES];
        if (forKingMoves)
        {
            clearBit(target, LSB(pos.piecesBitboards[WHITE_KING]));
        }

        // pawn moves
        Bitboard iterated = pos.piecesBitboards[BLACK_PAWN];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= blackPawnCaptures[index];
        }

        // black knights
        iterated = pos.piecesBitboards[BLACK_KNIGHT];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= knightMoves[index];
        }

        // black bishops
        iterated = pos.piecesBitboards[BLACK_BISHOP];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= bishopMoves[index][getMagicIndex(target & bishopMasks[index], bishopsMagics[index], bishopBits[index])];
        }

        // black rooks
        iterated = pos.piecesBitboards[BLACK_ROOK];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= rookMoves[index][getMagicIndex(target & rookMasks[index], rooksMagics[index], rookBits[index])];
        }

        // black queens
        iterated = pos.piecesBitboards[BLACK_QUEEN];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= rookMoves[index][getMagicIndex(target & rookMasks[index], rooksMagics[index], rookBits[index])];
            attacks |= bishopMoves[index][getMagicIndex(target & bishopMasks[index], bishopsMagics[index], bishopBits[index])];
        }

        // black kings
        iterated = pos.piecesBitboards[BLACK_KING];
        while (iterated)
        {
            int index = popLSB(iterated);
            attacks |= kingMoves[index];
        }
    }
    return attacks;
}

Bitboard getPinners(Position &pos, bool white, MoveList &moveList, int checks)
{
    // Returned bb with all pinned pieces for side
    Bitboard allPinned = 0;

    // Index of king
    int kingIndex = LSB(pos.piecesBitboards[white ? WHITE_KING : BLACK_KING]);

    // Attacks from king rook shape intersect with allies
    Bitboard kingRooksAttacks = rookMoves[kingIndex][getMagicIndex(pos.piecesBitboards[ALL_PIECES] & rookMasks[kingIndex], rooksMagics[kingIndex], rookBits[kingIndex])] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

    // New attacks omitting intersections
    Bitboard pinners = rookMoves[kingIndex][getMagicIndex((pos.piecesBitboards[ALL_PIECES] ^ kingRooksAttacks) & rookMasks[kingIndex], rooksMagics[kingIndex], rookBits[kingIndex])] & (pos.piecesBitboards[white ? BLACK_ROOK : WHITE_ROOK] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN]);

    // Attacks from king bishop shape intersect with allies
    Bitboard kingsBishopAttacks = bishopMoves[kingIndex][getMagicIndex(pos.piecesBitboards[ALL_PIECES] & bishopMasks[kingIndex], bishopsMagics[kingIndex], bishopBits[kingIndex])] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

    // New attacks omitting intersections
    pinners |= bishopMoves[kingIndex][getMagicIndex((pos.piecesBitboards[ALL_PIECES] ^ kingsBishopAttacks) & bishopMasks[kingIndex], bishopsMagics[kingIndex], bishopBits[kingIndex])] & (pos.piecesBitboards[white ? BLACK_BISHOP : WHITE_BISHOP] | pos.piecesBitboards[white ? BLACK_QUEEN : WHITE_QUEEN]);

    // Loop through all PINNERS
    while (pinners)
    {
        // Get index of pinner
        int pinnerIndex = popLSB(pinners);

        // Get lane of pin thorugh rectangular lookup
        Bitboard bbPinned = rectangularLookup[pinnerIndex][kingIndex] & pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

        if (bbPinned == 0) // safety check for position where you check king but intersect with ally on other side
            continue;      // example rnbqkbnr/ppp1pppp/8/3p4/Q1P5/8/PP1PPPPP/RNB1KBNR b KQkq - 1 2 check from a4 but intersection on f7


        // Pop index of lane to get pinned piece eg  lane: 1 1 1 1   &   piece in lane: 0 0 1 0  =  result 0 0 1 0
        // There can only be ONE PIECE in lane because in bbPinned we check for enemies so allies are omitted
        int pinnedIndex = LSB(bbPinned);

        // We add pinned index to all pinned bb
        setBit(allPinned, pinnedIndex);

        // We cannot move pinned piece if we are in check
        if (checks > 0)
            continue;

        // Get target without pinner
        Bitboard target = rectangularLookup[pinnerIndex][kingIndex];

        // Add pinner to bitboard
        setBit(target, pinnerIndex);

        // Get piecetype of pin
        int pieceType = pos.piecesArray[pinnedIndex];

        // Movegen depending on piece
        switch (pieceType)
        {
        case WHITE_PAWN:
            addPawnWhiteQuiet(pinnedIndex, moveList, pos, target);
            addPawnWhiteCaptures(pinnedIndex, moveList, pos, target & pos.piecesBitboards[BLACK_PIECES]);
            break;
        case WHITE_BISHOP:
            addBishopsMoves(pinnedIndex, moveList, pos, target);
            break;
        case WHITE_ROOK:
            addRookMoves(pinnedIndex, moveList, pos, target);
            break;
        case WHITE_QUEEN:
            addBishopsMoves(pinnedIndex, moveList, pos, target);
            addRookMoves(pinnedIndex, moveList, pos, target);
            break;
        case BLACK_PAWN:
            addPawnBlackQuiet(pinnedIndex, moveList, pos, target);
            addPawnBlackCaptures(pinnedIndex, moveList, pos, target & pos.piecesBitboards[WHITE_PIECES]);
            break;
        case BLACK_BISHOP:
            addBishopsMoves(pinnedIndex, moveList, pos, target);
            break;
        case BLACK_ROOK:
            addRookMoves(pinnedIndex, moveList, pos, target);
            break;
        case BLACK_QUEEN:
            addBishopsMoves(pinnedIndex, moveList, pos, target);
            addRookMoves(pinnedIndex, moveList, pos, target);
            break;
        default:
            break;
        }
    }

    return allPinned;
}

void addPawnWhiteQuiet(int startSquare, MoveList &moveList, Position &pos, Bitboard target)
{
    // Default moves Targets are empty squares by default
    Bitboard possibleMoves = whitePawnMoves[startSquare] & target;

    // this is probably useless
    possibleMoves &= ~pos.piecesBitboards[ALL_PIECES];

    // If we can do double pawn move
    if (startSquare / 8 == 1)
    {
        // Get the bitboard with index of double pawn push
        Bitboard doublePawnMove = (1ULL << (startSquare + 16));

        // Intersection with target (possible check protecion)
        Bitboard intersection = doublePawnMove & target;

        // If intersection with target we can consider legality
        if (intersection != 0)
        {
            // If there is nothing in front
            if (!isBitSet(pos.piecesBitboards[ALL_PIECES], startSquare + 8))
            {
                // If there is nothing on target square double push
                if (!isBitSet(pos.piecesBitboards[ALL_PIECES], startSquare + 16))
                    addMoves(startSquare, 1ULL << (startSquare + 16), moveList, pos, PAWN_DOUBLE_MOVE);
            }
        }
    }

    //If we are on promotion square add all types of promotion
    if (startSquare / 8 == 6 && (((1ULL << startSquare + 8) & possibleMoves) != 0))
    {
        addMoves(startSquare, startSquare + 8, KNIGHT_PROMOTION, moveList);
        addMoves(startSquare, startSquare + 8, BISHOP_PROMOTION, moveList);
        addMoves(startSquare, startSquare + 8, ROOK_PROMOTION, moveList);
        addMoves(startSquare, startSquare + 8, QUEEN_PROMOTION, moveList);
    }
    else //Add simple pawn push
    {
        addMoves(startSquare, possibleMoves, moveList, pos);
    }
}

// only simple captures without enpassant
void addPawnWhiteCaptures(int startSquare, MoveList &moveList, Position &pos, Bitboard target)
{
    //Simple captures upright upleft
    Bitboard possibleMoves = whitePawnCaptures[startSquare] & target;

    //If we are on promotion square add all possible promotions
    if (startSquare / 8 == 6)
    {
        if (((1ULL << startSquare + 7) & possibleMoves) != 0)
        {
            addMoves(startSquare, startSquare + 7, KNIGHT_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare + 7, BISHOP_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare + 7, ROOK_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare + 7, QUEEN_PROMOTION_CAPTURE, moveList);
        }
        if (((1ULL << startSquare + 9) & possibleMoves) != 0)
        {
            addMoves(startSquare, startSquare + 9, KNIGHT_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare + 9, BISHOP_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare + 9, ROOK_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare + 9, QUEEN_PROMOTION_CAPTURE, moveList);
        }
    }
    else //If we are not on promotion simply add all possible captures
    {
        addMoves(startSquare, possibleMoves, moveList, pos);
    }
}

// enpassant moves and pins
void addPawnWhiteEnpassant(MoveList &moveList, Position &pos, Bitboard target, int enPassantSquare)
{
    //Get pawns bitboard
    Bitboard iterated = pos.piecesBitboards[WHITE_PAWN];

    //Loop through pawns
    while (iterated)
    {
        //Get index of pawn
        int startSquare = popLSB(iterated);

        //If enpassantSquare intersects with pawns attacks from square and target which is [NO_PIECE] or checkEvasions plus enpassantCapture
        if (((1ULL << enPassantSquare) & whitePawnCaptures[startSquare] & target) == (1ULL << enPassantSquare))
        {
            //It holds attacks from king using queen movements so we can check if we are in check after removing pawns
            Bitboard enpassantAttacks = 0;

            //Init index of taken pawn
            int enpassantTakenIndex = enPassantSquare - 8;

            // Remove pawn and enpassant target to see if its legal
            Bitboard removingPawns = 1ULL << startSquare | 1ULL << enpassantTakenIndex;

            // Get king index
            int kingIndex = LSB(pos.piecesBitboards[WHITE_KING]);

            // Try if we will be in check
            // Target must include target square because rnbqkbnr/1pp2ppp/p7/3pP3/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4 enpassant still blocks check
            //                                                                            //Normal blockers             WITH  enPassantSquare     INTERSECT   mask of piece       REMOVING moved pawns
            //                                                                                                        (target after made move)                                    (the one that took and the one that was taken)
            enpassantAttacks |= bishopMoves[kingIndex][getMagicIndex((pos.piecesBitboards[ALL_PIECES] | 1ULL << enPassantSquare) & bishopMasks[kingIndex] & ~removingPawns, bishopsMagics[kingIndex], bishopBits[kingIndex])] & (pos.piecesBitboards[BLACK_BISHOP] | pos.piecesBitboards[BLACK_QUEEN]);
            enpassantAttacks |= rookMoves[kingIndex][getMagicIndex((pos.piecesBitboards[ALL_PIECES] | 1ULL << enPassantSquare) & rookMasks[kingIndex] & ~removingPawns, rooksMagics[kingIndex], rookBits[kingIndex])] & (pos.piecesBitboards[BLACK_ROOK] | pos.piecesBitboards[BLACK_QUEEN]);

            // If we are not in check after removing pawns
            // we can generate moves
            if (enpassantAttacks == 0)
            {
                //Bitboard with enpassantSquare
                Bitboard possibleEnPassant = whitePawnCaptures[startSquare] & (1ULL << enPassantSquare);

                //Adding moves to list
                addMoves(startSquare, possibleEnPassant, moveList, pos, EN_PASSANT);
            }
        }
    }
}

void addPawnBlackQuiet(int startSquare, MoveList &moveList, Position &pos, Bitboard target)
{
    // Default moves Targets are empty squares by default
    Bitboard possibleMoves = blackPawnMoves[startSquare] & target;

    // this is probably useless
    possibleMoves &= ~pos.piecesBitboards[ALL_PIECES];

    // If we can do double pawn move
    if (startSquare / 8 == 6)
    {
        // Get the bitboard with index of double pawn push
        Bitboard doublePawnMove = (1ULL << (startSquare - 16));

        // Intersection with target (possible check protecion)
        Bitboard intersection = doublePawnMove & target;

        // If intersection with target we can consider legality
        if (intersection != 0)
        {
            // If there is nothing in front
            if (!isBitSet(pos.piecesBitboards[ALL_PIECES], startSquare - 8))
            {
                // If there is nothing on target square double push
                if (!isBitSet(pos.piecesBitboards[ALL_PIECES], startSquare - 16))
                    addMoves(startSquare, 1ULL << (startSquare - 16), moveList, pos, PAWN_DOUBLE_MOVE);
            }
        }
    }

    //If we are on promotion square add all types of promotion
    if (startSquare / 8 == 1 && (((1ULL << startSquare - 8) & possibleMoves) != 0))
    {
        addMoves(startSquare, startSquare - 8, KNIGHT_PROMOTION, moveList);
        addMoves(startSquare, startSquare - 8, BISHOP_PROMOTION, moveList);
        addMoves(startSquare, startSquare - 8, ROOK_PROMOTION, moveList);
        addMoves(startSquare, startSquare - 8, QUEEN_PROMOTION, moveList);
    }
    else //Add simple pawn push
    {
        addMoves(startSquare, possibleMoves, moveList, pos);
    }
}

// only simple captures without enpassant
void addPawnBlackCaptures(int startSquare, MoveList &moveList, Position &pos, Bitboard target)
{
    //Simple captures upright upleft
    Bitboard possibleMoves = blackPawnCaptures[startSquare] & target;

    //If we are on promotion square add all possible promotions
    if (startSquare / 8 == 1)
    {
        if (((1ULL << startSquare - 7) & possibleMoves) != 0)
        {
            addMoves(startSquare, startSquare - 7, KNIGHT_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare - 7, BISHOP_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare - 7, ROOK_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare - 7, QUEEN_PROMOTION_CAPTURE, moveList);
        }
        if (((1ULL << startSquare - 9) & possibleMoves) != 0)
        {
            addMoves(startSquare, startSquare - 9, KNIGHT_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare - 9, BISHOP_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare - 9, ROOK_PROMOTION_CAPTURE, moveList);
            addMoves(startSquare, startSquare - 9, QUEEN_PROMOTION_CAPTURE, moveList);
        }
    }
    else //If we are not on promotion simply add all possible captures
    {
        addMoves(startSquare, possibleMoves, moveList, pos);
    }
}

// enpassant moves and pins
void addPawnBlackEnpassant(MoveList &moveList, Position &pos, Bitboard target, int enPassantSquare)
{
    //Get pawns bitboard
    Bitboard iterated = pos.piecesBitboards[BLACK_PAWN];

    //Loop through pawns
    while (iterated)
    {
        //Get index of pawn
        int startSquare = popLSB(iterated);

        //If enpassantSquare intersects with pawns attacks from square and target which is [NO_PIECE] or checkEvasions plus enpassantCapture
        if (((1ULL << enPassantSquare) & blackPawnCaptures[startSquare] & target) == (1ULL << enPassantSquare))
        {
            //It holds attacks from king using queen movements so we can check if we are in check after removing pawns
            Bitboard enpassantAttacks = 0;

            //Init index of taken pawn
            int enpassantTakenIndex = enPassantSquare + 8;

            // Remove pawn and enpassant target to see if its legal
            Bitboard removingPawns = 1ULL << startSquare | 1ULL << enpassantTakenIndex;

            // Get king index
            int kingIndex = LSB(pos.piecesBitboards[BLACK_KING]);

            // Try if we will be in check
            // Target must include target square because rnbqkbnr/1pp2ppp/p7/3pP3/8/8/PPPKPPPP/RNBQ1BNR w kq d6 0 4 enpassant still blocks check
            //                                                                            //Normal blockers             WITH  enPassantSquare     INTERSECT   mask of piece       REMOVING moved pawns
            //                                                                                                        (target after made move)                                    (the one that took and the one that was taken)
            enpassantAttacks |= bishopMoves[kingIndex][getMagicIndex((pos.piecesBitboards[ALL_PIECES] | 1ULL << enPassantSquare) & bishopMasks[kingIndex] & ~removingPawns, bishopsMagics[kingIndex], bishopBits[kingIndex])] & (pos.piecesBitboards[WHITE_BISHOP] | pos.piecesBitboards[WHITE_QUEEN]);
            enpassantAttacks |= rookMoves[kingIndex][getMagicIndex((pos.piecesBitboards[ALL_PIECES] | 1ULL << enPassantSquare) & rookMasks[kingIndex] & ~removingPawns, rooksMagics[kingIndex], rookBits[kingIndex])] & (pos.piecesBitboards[WHITE_ROOK] | pos.piecesBitboards[WHITE_QUEEN]);

            // If we are not in check after removing pawns
            // we can generate moves
            if (enpassantAttacks == 0)
            {
                //Bitboard with enpassantSquare
                Bitboard possibleEnPassant = blackPawnCaptures[startSquare] & (1ULL << enPassantSquare);

                //Adding moves to list
                addMoves(startSquare, possibleEnPassant, moveList, pos, EN_PASSANT);
            }
        }
    }
}

void addKnightsMoves(int startSquare, MoveList &moveList, Position &pos, Bitboard target)
{
    // lookup
    Bitboard possibleMoves = knightMoves[startSquare] & target;

    // go to move creator
    addMoves(startSquare, possibleMoves, moveList, pos);
}

void addBishopsMoves(int startSquare, MoveList &moveList, Position &pos, Bitboard target)
{
    // lookup
    Bitboard possibleMoves = bishopMoves[startSquare][getMagicIndex(pos.piecesBitboards[ALL_PIECES] & bishopMasks[startSquare], bishopsMagics[startSquare], bishopBits[startSquare])] & target;

    // go to move creator
    addMoves(startSquare, possibleMoves, moveList, pos);
}

void addRookMoves(int startSquare, MoveList &moveList, Position &pos, Bitboard target)
{
    // lookup
    Bitboard possibleMoves = rookMoves[startSquare][getMagicIndex(pos.piecesBitboards[ALL_PIECES] & rookMasks[startSquare], rooksMagics[startSquare], rookBits[startSquare])] & target;

    // go to move creator
    addMoves(startSquare, possibleMoves, moveList, pos);
}

void generateKingsMoves(Position &pos, MoveList &moveList, Bitboard target, bool white, int checks)
{
    // Get king index
    int index = LSB(pos.piecesBitboards[white ? WHITE_KING : BLACK_KING]);

    // Get possible moves from lookup
    Bitboard possibleMoves = kingMoves[index] & target;

    // Subtract own pieces from lookup
    possibleMoves &= ~pos.piecesBitboards[white ? WHITE_PIECES : BLACK_PIECES];

    // Add simple moves and captures for king
    addMoves(index, possibleMoves, moveList, pos);

    // If there are no checks we can consider castling
    if (checks < 1)
    {
        //Check castling rights from state list
        int castlingRights = pos.stateInfoList[pos.stateCounter].castlingRights;

        //If there is no castling rights end king movegen
        if(castlingRights == 0)
        {
            return;
        }

        //For white
        if (white)
        {

            //If we have castling rights for short AND emptySpaces aren't targeted AND there are no pieces on empty spaces
            if ((castlingRights & 8) && ((96 & target) == 96) && ((96 & pos.piecesBitboards[ALL_PIECES]) == 0))
            {
                //Add short castle
                addMoves(index, 1ULL << g1, moveList, pos, 2);
            }


            //If we have castling rights for long AND emptySpaces aren't targeted AND there are no pieces on empty spaces
            if ((castlingRights & 4) && ((12 & target) == 12) && ((14 & pos.piecesBitboards[ALL_PIECES]) == 0))
            {
                //Add long castle
                addMoves(index, 1ULL << c1, moveList, pos, 3);
            }
        }
        else //Black
        {

            //If we have castling rights for short AND emptySpaces aren't targeted AND there are no pieces on empty spaces
            if ((castlingRights & 2) && ((6917529027641081856 & target) == 6917529027641081856) && ((6917529027641081856 & pos.piecesBitboards[ALL_PIECES]) == 0))
            {
                //Add short castle
                addMoves(index, 1ULL << g8, moveList, pos, 2);
            }

            //If we have castling rights for long AND emptySpaces aren't targeted AND there are no pieces on empty spaces
            if ((castlingRights & 1) && ((864691128455135232 & target) == 864691128455135232) && ((1008806316530991104 & pos.piecesBitboards[ALL_PIECES]) == 0))
            {
                //Add long castle
                addMoves(index, 1ULL << c8, moveList, pos, 3);
            }
        }
    }
}