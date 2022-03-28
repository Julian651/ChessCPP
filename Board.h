#pragma once
#include <iostream>
#include <cstring>
#include "Component.h"

/**
 * @section DESCRIPTION
 *
 * The Board class represents the chess board, along with the functionality
 * required to move pieces on the board.
 *
 * @section PIECE INFORMATION
 *
 * The following characters are representations of pieces used for the program.
 * Capital characters are white pieces and lowercase or black pieces
 *
 * ROOK     - r R
 * KNIGHT   - n N
 * BISHOP   - b B
 * QUEEN    - q Q
 * KING     - k K
 * PAWN     - p P
 *
 */

class Board : public Component, public MouseEvent
{
private:
   struct PIECE_INFO
   {
      char c;
      float x, y;
      char* pos = (char*)calloc(3, sizeof(char));
   };

   std::string buffer;
   ImageData* pID_PIECES = NULL;

   /**
   * Function that converts a number from a character using ASCII values.
   *
   * @param  ch is the character that needs to be converted
   * @return the integer value.
   */
   int toNumber(char ch);

   /**
   * Function that sets a piece on the board.
   *
   * @param  pos is the position where the piece is to be set
   *         It is expected that input is a string with 2 characters.
   * @param  piece is the char representation of the piece.
   * @return the char representation of the piece that was
             previously in that position.
   */
   char Set(const char pos[3], char piece);

   /**
   * Function that retrieves a piece from the board and removes that piece.
   *
   * @param  pos is the position where the user wishes to retrieve a piece.
   * @return the char representation of the piece that is in that position.
   */
   char Retrieve(const char pos[3]);

   /**
   * Function that peeks in the board. Similar functionality to
   * Retrieve().
   *
   * @param  pos is the position where the user wishes to look in at the board.
   * @return the char representation of the piece in that position.
   */
   char Peek(const char pos[3]);

   /**
   * Function that checks whether a move is possible or not given a specific piece.
   *
   * @param  fromPos is the initial position.
   * @param  toPos is the desired position.
   * @param  piece is the char representation of the piece to move.
   * @param  player is the char representation of the player ('b' or 'w').
   * @return true if the move is possible, else false.
   */
   bool MovePossible(const char fromPos[3], const char toPos[3], char piece, char player);

   /**
   * Function that determines the color of a certain piece
   *
   * @param  ch is the piece
   * @return 1 = WHITE, 0 = BLACK, -1 = BLANK/UNDETERMINED
   */
   int Color(char ch);

   /**
   * Function that creates an image data if it doesn't exist yet
   *
   * @param  lpName is the name of the resource found in the resource file
   * @param  ppImageData is the image data to be created
   * @param  g is the graphics context
   * @return S_OK if succeded and or created, E_FAIL if not created
   */
   HRESULT CreateIfNExists(LPCWSTR lpName, ImageData** ppImageData, Graphics2D* g);

   /**
   * Function that draws a piece using its character representation
   *
   * @param  ch is the character representation of the piece
   * @param  x is the x coordinate of the board in screen space
   * @param  y is the y coordinate of the board in screen space
   * @param  g is the graphics context
   * @return S_OK if succeeded in drawing, E_FAIL if not
   */
   HRESULT DrawPiece(char ch, float x, float y, Graphics2D* g);

   const std::string PossibleMoves(char ch, const char pos[3]);

   char kingPositions[5] = { 'E', '1', 'E', '8', '\0' };
   const std::string inCheck(const char pos[3]);

public:

   /**
   * Constructor that creates the initial FEN buffer.
   *
   * @param  FEN is a string of characters ending with zero (sz) that represents
   *         the board state of any particular game.
   * @param  x is the x coordinate of the board in screen space
   * @param  y is the y coordinate of the board in screen space
   *
   * Deconstructor that cleans up the memory.
   */
   Board(float x, float y, const char* FEN) : Component(x, y, 0, 0) { buffer = FEN; AddMouseEvent(this); }
   ~Board()
   {
      delete pID_PIECES;
   }

   /**
   * Function that tries to move a piece from a position to another position
   *
   * @param  player is the current player ('w' or 'b') that is trying to move.
   *         If the player is trying to move a black piece while it is white's
   *         turn to move, the move will not happen and vice versa.
   * @param  fromPos is the position of the piece the player is trying to move.
   * @param  toPos is the position that the player is trying to move the piece
   *         from fromPos to.
   * @return TRUE if move succeeded, FALSE otherwise
   */
   bool Move(char player, const char* fromPos, const char* toPos);

   /**
   * Function that prints the board out to the screen
   */
   void Print();

   /**
   * Function that paints the board to the screen
   *
   * @param  The graphics context
   */
   void Paint(Graphics2D* g) override;

   /**
   * Function that updates the board
   */
   void Update() override {}

   /**
   * Function that determines what happens to the board when mouse is clicked on a certain position
   * 
   * @param  x is the x coordinate in screen space
   * @param  y is the y coordinate in screen space
   * @return TRUE if something happened, false otherwise
   */
   bool MouseClicked(float x, float y) override;

   PIECE_INFO* pPI_CLICK = NULL;
   PIECE_INFO* pPI_DROP = NULL;
   std::string possible = "";
   /**
   * Function that determines what happens when a mouse cursor moves on screen
   * 
   * @param  x is the x coordinate in screen space
   * @param  y is the y coordinate in screen space
   * @return TRUE if something happened, false otherwise
   */
   bool MouseMove(float x, float y) override;

   bool MouseUp(float x, float y) override;
};
