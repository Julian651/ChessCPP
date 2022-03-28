#include "Board.h"
constexpr auto WHITE = 1;
constexpr auto BLACK = 0;
constexpr auto BLANK = -1;
int Board::Color(char ch)
{
   if (ch > 64 && ch < 91) return 1;
   else if (ch > 96 && ch < 123) return 0;
   else if (ch == '1') return -1;
   else return -2;
}
HRESULT Board::CreateIfNExists(LPCWSTR lpName, ImageData** ppImageData, Graphics2D* g)
{
   return S_OK;
}
HRESULT Board::DrawPiece(char ch, float x, float y, Graphics2D* g)
{
   bool isWhite = false;
   float location = 0;
   if (Color(ch) == WHITE) isWhite = true;
   switch (ch)
   {
   case 'P':
   case 'p':
      location = 0;
      break;
   case 'R':
   case 'r':
      location = 80.f;
      break;
   case 'N':
   case 'n':
      location = 160.f;
      break;
   case 'B':
   case 'b':
      location = 240.f;
      break;
   case 'Q':
   case 'q':
      location = 320.f;
      break;
   case 'K':
   case 'k':
      location = 400.f;
      break;
   default:
      return E_FAIL;
   }

   HRESULT hr = S_OK;
   if (!pID_PIECES)
   {
      hr = g->LoadImageFromResource(MAKEINTRESOURCE(201), RT_RCDATA, &pID_PIECES);
      //D2D1_SIZE_F size = pImageData->bmp->GetSize();
      super::SetSize(80.0f, 80.0f);
      if (SUCCEEDED(hr))
      {
         // black
         if (!isWhite)
         {
            pID_PIECES->src = D2D1::RectF(location, 80.f, 80.f + location, 160.f);
         }

         // white
         else
         {
            pID_PIECES->src = D2D1::RectF(location, 0.0f, 80.f + location, 80.0f);
         }

      }
   }
   else
   {
      // black
      if (!isWhite)
      {
         pID_PIECES->src = D2D1::RectF(location, 80.f, 80.f + location, 160.f);
      }

      // white
      else
      {
         pID_PIECES->src = D2D1::RectF(location, 0.0f, 80.f + location, 80.0f);
      }
   }
   if (SUCCEEDED(hr))
   {
      g->DrawImage(pID_PIECES->bmp,
         D2D1::RectF(x, y, x + 80.0f, y + 80.0f),
         pID_PIECES->src);
   }
   return hr;
}
const std::string Board::PossibleMoves(char ch, const char pos[3])
{
   std::string str = "";
   int colorOfPiece = Color(ch);
   if (buffer.at(0) == 'w' && colorOfPiece == BLACK) return str;
   if (buffer.at(0) == 'b' && colorOfPiece == WHITE) return str;
   int index = (colorOfPiece == WHITE ? 0 : 2);

   const char kingPos[3] = { kingPositions[index], kingPositions[index + 1], '\0' };
   std::string possibleChecks = inCheck(kingPos);

   int sign = 1;
   if (colorOfPiece == WHITE) sign = -1;
   switch (ch)
   {
   case 'P':
      /**
      *
      * if pos is on the 2nd or 7th rank, then double forward is possible,
      *     IF AND ONLY IF nothing is infront, and nothing is in front x2
      * if pos is on any other rank, then single forward is possible,
      *     IF AND ONLY IF nothing is infront
      * if pos is on any other rank, then take is possible diagonally left and diagonally right,
      *     IF AND ONLY IF color of diagonal pos is opposite current pos
      *
      */
   case 'p':
   {

      if (pos[1] == '2' || pos[1] == '7')
      {

         const char forward[3] = { pos[0], (char)(pos[1] - sign), pos[2] };
         const char doubleForward[3] = { pos[0], (char)(pos[1] - (2 * sign)), pos[2] };
         if (Peek(forward) == '1')
         {
            Set(forward, ch);
            if (inCheck(kingPos).size() == 0)
            {
               str.append(forward);
               str.append("/");
               if (Peek(doubleForward) == '1')
               {
                  str.append(doubleForward);
                  str.append("/");
               }
            }
            Retrieve(forward);

         }
      }
      else
      {
         const char forward[3] = { pos[0], (char)(pos[1] - sign), pos[2] };
         if (Peek(forward) == '1') { str.append(forward); str.append("/"); }
      }
      const char diagonalL[3] = {(char)(pos[0] - 1), (char)(pos[1] - sign), pos[2] };
      const char diagonalR[3] = { (char)(pos[0] + 1), (char)(pos[1] - sign), pos[2] };
      int colorDL = Color(Peek(diagonalL));
      int colorDR = Color(Peek(diagonalR));
      
      if (colorDL != -2 && colorDL != BLANK && colorDL != colorOfPiece) { str.append(diagonalL); str.append("/"); }
      if (colorDR != -2 && colorDR != BLANK && colorDR != colorOfPiece) { str.append(diagonalR); str.append("/"); }

   }
      break;
   case 'R':
      /**
      * Possible spaces include everything in front, left, or right of the rook
      *     UP UNTIL a capture or a same color piece
      */
   case 'r':
   {
      char currentPos[3];
      char pieceAtPos = Peek(pos);

      strcpy_s(currentPos, 3, pos);

      // front
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[1] -= sign;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos);
         str.append("/");
         if (pieceAtPos != '1') break;
      }

      // back
      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[1] += sign;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos);
         str.append("/");
         if (pieceAtPos != '1') break;
      }

      // left
      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]--;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos);
         str.append("/");
         if (pieceAtPos != '1') break;
      }

      // right
      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]++;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos);
         str.append("/");
         if (pieceAtPos != '1') break;
      }
   }
   break;
   case 'N':
      /**
      * Move is possible if
      *     +2, +1
      *     +2, -1
      *     -2, +1
      *     -2, -2
      *     +1, +2
      *     +1, -2
      *     -1, +2
      *     -1, -2
      */
   case 'n':
   {
      char current[3] = { pos[0] + 2, pos[1] + 1, '\0' };
      char pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece)
      { str.append(1, pos[0] + 2); str.append(1, pos[1] + 1); str.append("/"); }
      
      current[1] = pos[1] - 1;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece)
      { str.append(1, pos[0] + 2); str.append(1, pos[1] - 1); str.append("/"); }
      
      current[0] = pos[0] - 2;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece)
      { str.append(1, pos[0] - 2); str.append(1, pos[1] - 1); str.append("/"); }
      
      current[1] = pos[1] + 1;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece)
      { str.append(1, pos[0] - 2); str.append(1, pos[1] + 1); str.append("/"); }
      
      current[0] = pos[0] + 1;
      current[1] = pos[1] + 2;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece)
      { str.append(1, pos[0] + 1); str.append(1, pos[1] + 2); str.append("/"); }
      
      current[1] = pos[1] - 2;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece)
      { str.append(1, pos[0] + 1); str.append(1, pos[1] - 2); str.append("/"); }

      current[0] = pos[0] - 1;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece)
      { str.append(1, pos[0] - 1); str.append(1, pos[1] - 2); str.append("/"); }

      current[1] = pos[1] + 2;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece)
      { str.append(1, pos[0] - 1); str.append(1, pos[1] + 2); str.append("/"); }
   }
      break;
   case 'B':
      /**
      * Move only diagonally in each direction
      */
   case 'b':
   {
      char currentPos[3];
      strcpy_s(currentPos, 3, pos);
      char pieceAtPos = Peek(currentPos);

      while (pieceAtPos != '0')
      {
         currentPos[0]++;
         currentPos[1]++;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos);
         str.append("/");
         if (pieceAtPos != '1') break;
      }

      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]--;
         currentPos[1]--;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos);
         str.append("/");
         if (pieceAtPos != '1') break;
      }

      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]++;
         currentPos[1]--;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos);
         str.append("/");
         if (pieceAtPos != '1') break;
      }

      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]--;
         currentPos[1]++;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos);
         str.append("/");
         if (pieceAtPos != '1') break;
      }
   }
   break;
   case 'Q':
   case 'q':
   {
      char currentPos[3];
      char pieceAtPos = Peek(pos);

      strcpy_s(currentPos, 3, pos);

      // front
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[1]-=sign;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos); str.append("/");
         if (pieceAtPos != '1') break;
      }

      // back
      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[1]+=sign;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos); str.append("/");
         if (pieceAtPos != '1') break;
      }

      // left
      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]--;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos); str.append("/");
         if (pieceAtPos != '1') break;
      }

      // right
      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]++;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos); str.append("/");
         if (pieceAtPos != '1') break;
      }

      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]++;
         currentPos[1]++;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos); str.append("/");
         if (pieceAtPos != '1') break;
      }

      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]--;
         currentPos[1]--;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos); str.append("/");
         if (pieceAtPos != '1') break;
      }

      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]++;
         currentPos[1]--;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos); str.append("/");
         if (pieceAtPos != '1') break;
      }

      strcpy_s(currentPos, 3, pos);
      pieceAtPos = Peek(currentPos);
      while (pieceAtPos != '0')
      {
         currentPos[0]--;
         currentPos[1]++;
         pieceAtPos = Peek(currentPos);
         if (Color(pieceAtPos) == colorOfPiece || pieceAtPos == '0') break;
         str.append(currentPos); str.append("/");
         if (pieceAtPos != '1') break;
      }
   }
      break;
   case 'K':
   case 'k':
   {
      if (possibleChecks.size() > 0)
      {
         std::cout << "\nCHECK!!!\n";
      }
      char current[3] = { pos[0] + 1, pos[1] + 1, '\0' };
      char pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece && inCheck(current).size() == 0)
      { str.append(1, pos[0] + 1); str.append(1, pos[1] + 1); str.append("/"); }

      current[1] = pos[1] - 1;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece && inCheck(current).size() == 0)
      { str.append(1, pos[0] + 1); str.append(1, pos[1] - 1); str.append("/"); }

      current[0] = pos[0] - 1;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece && inCheck(current).size() == 0)
      { str.append(1, pos[0] - 1); str.append(1, pos[1] - 1); str.append("/"); }

      current[1] = pos[1] + 1;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece && inCheck(current).size() == 0)
      { str.append(1, pos[0] - 1); str.append(1, pos[1] + 1); str.append("/"); }

      current[0] = pos[0];
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece && inCheck(current).size() == 0)
      { str.append(1, pos[0]); str.append(1, pos[1] + 1); str.append("/"); }

      current[1] = pos[1] - 1;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece && inCheck(current).size() == 0)
      { str.append(1, pos[0]); str.append(1, pos[1] - 1); str.append("/"); }

      current[0] = pos[0] + 1;
      current[1] = pos[1];
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece && inCheck(current).size() == 0)
      { str.append(1, pos[0] + 1); str.append(1, pos[1]); str.append("/"); }

      current[0] = pos[0] - 1;
      pieceCurrent = Peek(current);
      if (pieceCurrent != '0' && Color(pieceCurrent) != colorOfPiece && inCheck(current).size() == 0)
      { str.append(1, pos[0] - 1); str.append(1, pos[1]); str.append("/"); }

   }
      break;
   }


   return str;
}

const std::string Board::inCheck(const char pos[3])
{
   std::string str = "";
   int colorOfKing = (buffer.at(0) == 'w' ? WHITE : BLACK);
   // rook / queen
   //===============

   // up
   char current[3] = { pos[0], pos[1], '\0' };
   char pieceCurrent = Peek(current);
   int count = 0;
   int pieceBias = (colorOfKing == WHITE ? 32 : 0);
   while (pieceCurrent != '0')
   {
      current[1]++;
      count++;
      pieceCurrent = Peek(current);
      if (pieceCurrent == ('R' + pieceBias) || pieceCurrent == ('Q' + pieceBias))
      {
         for (int i = count; i > -1; i--)
         {
            str.append(current);
            str.append("/");
            current[1]--;
         }
         break;
      }
      else if (pieceCurrent != '1') break;
   }
   current[1] = pos[1];
   pieceCurrent = Peek(current);
   count = 0;
   while (pieceCurrent != '0')
   {
      current[1]--;
      count++;
      pieceCurrent = Peek(current);
      if (pieceCurrent == ('R' + pieceBias) || pieceCurrent == ('Q' + pieceBias))
      {
         for (int i = count; i > -1; i--)
         {
            str.append(current);
            str.append("/");
            current[1]++;
         }
         break;
      }
      else if (pieceCurrent != '1') break;
   }
   current[1] = pos[1];
   pieceCurrent = Peek(current);
   count = 0;
   while (pieceCurrent != '0')
   {
      current[0]--;
      count++;
      pieceCurrent = Peek(current);
      if (pieceCurrent == ('R' + pieceBias) || pieceCurrent == ('Q' + pieceBias))
      {
         for (int i = count; i > -1; i--)
         {
            str.append(current);
            str.append("/");
            current[0]++;
         }
         break;
      }
      else if (pieceCurrent != '1') break;
   }
   current[0] = pos[0];
   pieceCurrent = Peek(current);
   count = 0;
   while (pieceCurrent != '0')
   {
      current[0]++;
      count++;
      pieceCurrent = Peek(current);
      if (pieceCurrent == 'k' - pieceBias) continue;
      if (pieceCurrent == ('R' + pieceBias) || pieceCurrent == ('Q' + pieceBias))
      {
         for (int i = count; i > -1; i--)
         {
            str.append(current);
            str.append("/");
            current[0]--;
         }
         break;
      }
      else if (pieceCurrent != '1') break;
   }
   current[0] = pos[0];

   // bishop / queen
   //===============

   pieceCurrent = Peek(current);
   count = 0;
   while (pieceCurrent != '0')
   {
      current[0]++;
      current[1]++;
      count++;
      pieceCurrent = Peek(current);
      if (pieceCurrent == ('B' + pieceBias) || pieceCurrent == ('Q' + pieceBias))
      {
         for (int i = count; i > -1; i--)
         {
            str.append(current);
            str.append("/");
            current[0]--;
            current[1]--;
         }
         break;
      }
      else if (pieceCurrent != '1') break;
   }
   current[0] = pos[0];
   current[1] = pos[1];
   pieceCurrent = Peek(current);
   count = 0;
   while (pieceCurrent != '0')
   {
      current[0]++;
      current[1]--;
      count++;
      pieceCurrent = Peek(current);
      if (pieceCurrent == ('B' + pieceBias) || pieceCurrent == ('Q' + pieceBias))
      {
         for (int i = count; i > -1; i--)
         {
            str.append(current);
            str.append("/");
            current[0]--;
            current[1]++;
         }
         break;
      }
      else if (pieceCurrent != '1') break;
   }
   current[0] = pos[0];
   current[1] = pos[1];
   pieceCurrent = Peek(current);
   count = 0;
   while (pieceCurrent != '0')
   {
      current[0]--;
      current[1]++;
      count++;
      pieceCurrent = Peek(current);
      if (pieceCurrent == ('B' + pieceBias) || pieceCurrent == ('Q' + pieceBias))
      {
         for (int i = count; i > -1; i--)
         {
            str.append(current);
            str.append("/");
            current[0]++;
            current[1]--;
         }
         break;
      }
      else if (pieceCurrent != '1') break;
   }
   current[0] = pos[0];
   current[1] = pos[1];
   pieceCurrent = Peek(current);
   count = 0;
   while (pieceCurrent != '0')
   {
      current[0]--;
      current[1]--;
      count++;
      pieceCurrent = Peek(current);
      if (pieceCurrent == ('B' + pieceBias) || pieceCurrent == ('Q' + pieceBias))
      {
         for (int i = count; i > -1; i--)
         {
            str.append(current);
            str.append("/");
            current[0]++;
            current[1]++;
         }
         break;
      }
      else if (pieceCurrent != '1') break;
   }
   current[0] = pos[0];
   current[1] = pos[1];


   // pawn
   //===============

   int sign = (buffer.at(0) == 'w' ? -1 : 1);
   current[0]++;
   current[1] -= sign;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('P' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }
   current[0] = pos[0] - 1;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('P' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }

   // knight
   //===============
   current[0] = pos[0] + 2; current[1] = pos[1] + 1;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('N' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }
   current[0] = pos[0] + 2; current[1] = pos[1] - 1;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('N' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }
   current[0] = pos[0] - 2; current[1] = pos[1] + 1;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('N' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }
   current[0] = pos[0] - 2; current[1] = pos[1] - 1;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('N' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }

   current[0] = pos[0] + 1; current[1] = pos[1] + 2;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('N' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }
   current[0] = pos[0] + 1; current[1] = pos[1] - 2;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('N' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }
   current[0] = pos[0] - 1; current[1] = pos[1] + 2;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('N' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }
   current[0] = pos[0] - 1; current[1] = pos[1] - 2;
   pieceCurrent = Peek(current);
   if (pieceCurrent == ('N' + pieceBias))
   {
      str.append(current);
      str.append("/");
   }
   std::cout << " POS: " << pos << " STRING: " << str << std::endl;
   return str;
}
int Board::toNumber(char ch)
{
   if (ch > 64 && ch < 73)
   {
      return ch - 65;
   }
   else if (ch > 96 && ch < 105)
   {
      return ch - 97;
   }
   else if (ch > 47 && ch < 58)
   {
      return ch - 48;
   }
   return -1;
}

char Board::Set(const char pos[3], char piece)
{
   char pieceRemoved = '1';

   int col = toNumber(*pos); pos++;
   int row = 9 - toNumber(*pos);

   int line = 0;
   int count = 0;
   for (unsigned int i = 0; i < buffer.size(); i++)
   {
      char c = buffer.at(i);
      if (c == '/' || c == ' ') { line++; continue; }

      if (line == row)
      {
         if (count == col)
         {
            pieceRemoved = c;
            buffer.replace(i, 1, 1, piece);
         }
         count++;
      }
   }

   return pieceRemoved;
}

char Board::Retrieve(const char pos[3])
{
   char pieceRetrieved = '0';

   int col = toNumber(*pos); pos++;
   int row = 9 - toNumber(*pos);

   int line = 0;
   int count = 0;
   for (unsigned int i = 0; i < buffer.size(); i++)
   {
      char c = buffer.at(i);
      if (c == '/' || c == ' ') { line++; continue; }

      if (line == row)
      {
         if (count == col)
         {
            pieceRetrieved = c;
            buffer.replace(i, 1, 1, '1');
         }
         count++;
      }
   }

   return pieceRetrieved;
}

char Board::Peek(const char pos[3])
{
   char piecePeeked = '0';

   if (pos[0] < 'A' || pos[0] > 'H' || pos[1] < '1' || pos[1] > '8') return '0';

   int col = toNumber(*pos); pos++;
   int row = 9 - toNumber(*pos);

   int line = 0;
   int count = 0;
   for (unsigned int i = 0; i < buffer.size(); i++)
   {
      char c = buffer.at(i);
      if (c == '/' || c == ' ') { line++; continue; }

      if (line == row)
      {
         if (count == col)
         {
            piecePeeked = c;
         }
         count++;
      }
   }

   return piecePeeked;
}

bool Board::MovePossible(const char fromPos[3], const char toPos[3], char piece, char player)
{

   // move is IMpossible if any of the possible moves compares to a checks move
   for (unsigned int i = 0; i < possible.size(); i++)
   {
      char c = possible.at(i);
      if (i != 0 && i % 3 != 0) continue;
      if (c == '/') { continue; }

      const char pos[3] = { possible.at(i), possible.at(i + 1), '\0' };
      if (strcmp(toPos, pos) == 0)
      {
         return true;
      }
   }
   return false;
}

bool Board::Move(char player, const char* fromPos, const char* toPos)
{
   if (player != buffer.at(0)) return false;



   if (player == buffer.at(0))
   {
      char piece = Retrieve(fromPos);

      if (MovePossible(fromPos, toPos, piece, player))
      {
         Set(toPos, piece);
         if (player == 'w') buffer.replace(0, 1, 1, 'b');
         else buffer.replace(0, 1, 1, 'w');

         possible = "";

         int index = (player == 'w' ? 2 : 0);
         const char king[3] = { kingPositions[index], kingPositions[index + 1], kingPositions[2] };
         if (inCheck(king).size() > 1) std::cout << "CHECK!";

         if (piece == 'K') { kingPositions[0] = toPos[0]; kingPositions[1] = toPos[1]; }
         else if (piece == 'k') { kingPositions[2] = toPos[0]; kingPositions[3] = toPos[1]; }
         return true;
      }
      else
      {
         std::cout << "\nMove impossible.\n";
         Set(fromPos, piece);
         return false;
      }

   }
   return false;
}

void Board::Print()
{
   std::cout << "\n====================================================================================\n";
   std::cout << "\n" << buffer << "\n";

   int line = 0;
   for (unsigned int i = 0; i < buffer.size(); i++)
   {
      char c = buffer.at(i);
      if (c == '/' || c == ' ') { line++; std::cout << std::endl; continue; }

      //0st line - who's to move

      if (line == 0)
      {
         if (c == 'w') std::cout << "\nWhite to move.\n";
         else std::cout << "\nBlack to move.\n";
      }

      //1nd - 9th lines - board state

      else if (line < 9) std::cout << c;

      //9th line - castle viability

      else if (line == 9)
      {
         if (c == 'K') std::cout << "\nWhite King side castle avaliable.";
         else if (c == 'Q') std::cout << "\nWhite Queen side castle avaliable.";
         else if (c == 'k') std::cout << "\nBlack King side casle avaliable.";
         else if (c == 'q') std::cout << "\nBlack Queen side castle avaliable.";
      }

      // 10th line, en passant target square
      else if (line == 10)
      {
         if (i - 1 >= 0 && buffer.at(i - 1) == ' ')
         {
            std::cout << "\nEn passant target square ";
         }
         std::cout << c;
      }

      // 11th line, halfmove clock

      // 12th line, fullmove number
   }
}

void Board::Paint(Graphics2D* g)
{
   //std::cout << "POSSIBLE: " << possible << std::endl;
   g->SetBrush(D2D1::ColorF(1.f, 0.f, 0.f, 0.5f));
   // E3/E4/E5/E6 -> i = 0, i = 3, i = 6, i = 9

   for (unsigned int i = 0; i < possible.size(); i++)
   {
      char c = possible.at(i);
      if (i != 0 && i % 3 != 0) continue;
      if (c == '/') { continue; }

      const char pos[3] = { possible.at(i), possible.at(i + 1), '\0' };
      int row = 9 - toNumber(pos[1]);
      int col = toNumber(pos[0]);

      float left = super::c_x + (col * 80);
      float top = super::c_y + (row * 80);
      g->FillRect(D2D1::RectF(left, top, left + 80.f, top + 80.f));
   }

   int line = 0;
   int count = 0;
   //std::cout << "\n====================================================================================\n";
   //std::cout << "\n" << buffer << "\n";
   for (unsigned int i = 0; i < buffer.size(); i++)
   {
      char c = buffer.at(i);
      if (c == '/' || c == ' ') { line++; count = 0; continue; }

      //0st line - who's to move

      if (line == 0)
      {
         //if (c == 'w') std::cout << "\nWhite to move.\n";
         //else std::cout << "\nBlack to move.\n";
      }

      //1nd - 9th lines - board state

      else if (line < 9)
      {
         float left = super::c_x + (count * 80);
         float top = super::c_y + (line * 80);

         if ((count + line) % 2 == 0) g->SetBrush(D2D1::ColorF(0.f, 0.f, 0.f, 0.5f));
         else g->SetBrush(D2D1::ColorF(1.f, 1.f, 1.f, 0.5f));
         g->FillRect(D2D1::RectF(left, top, left + 80.f, top + 80.f));
         DrawPiece(c, left, top, g);
      }
      //9th line - castle viability

      else if (line == 9)
      {
         //if (c == 'K') std::cout << "\nWhite King side castle avaliable.";
         //else if (c == 'Q') std::cout << "\nWhite Queen side castle avaliable.";
         //else if (c == 'k') std::cout << "\nBlack King side casle avaliable.";
         //else if (c == 'q') std::cout << "\nBlack Queen side castle avaliable.";
      }

      // 10th line, en passant target square
      else if (line == 10)
      {
         if (i - 1 >= 0 && buffer.at(i - 1) == ' ')
         {
            //std::cout << "\nEn passant target square ";
         }
         //std::cout << c;
      }

      // 11th line, halfmove clock

      // 12th line, fullmove number
      count++;
   }
   if (pPI_CLICK)
   {
      DrawPiece(pPI_CLICK->c, pPI_CLICK->x - 40, pPI_CLICK->y - 40, g);
   }
}

// MouseEvent interface

bool Board::MouseClicked(float x, float y)
{
   float col = (x - super::c_x) / 80;
   float row = (y - super::c_y) / 80;
   if (!pPI_CLICK)
   {
      const char pos1[3] = { (char)(col + 'A'), (char)((10 - row) + '0'), '\0' };
      char piece = Peek(pos1);
      if (Color(piece) == -2) return false;
      if (Color(piece) != BLANK)
      {
         std::cout << "Mouse clicked: " << pos1 << std::endl;
         pPI_CLICK = new PIECE_INFO{ piece, x, y };
         strcpy_s(pPI_CLICK->pos, 3, pos1);

         possible = PossibleMoves(piece, pos1);
         std::cout << possible << std::endl;
         int index = (buffer.at(0) == 'w' ? 0 : 2);
         const char kingPos[3] = { kingPositions[index], kingPositions[index + 1], '\0' };
         std::string checks = inCheck(kingPos);

         std::string possible2 = "";
         if (piece != 'k' && piece != 'K')
         {
            for (int i = 0; i < (int)checks.size(); i++)
            {
               char c = checks.at(i);
               if (i != 0 && i % 3 != 0) continue;
               if (c == '/') { continue; }
               const char pos[3] = { c, checks.at(i + 1), '\0' };
               for (int j = 0; j < (int)possible.size(); j++)
               {
                  char c2 = possible.at(j);
                  if (j != 0 && j % 3 != 0) continue;
                  if (c == '/') continue;
                  const char pos2[3] = { c2, possible.at(j + 1), '\0' };
                  if (strcmp(pos, pos2) == 0 || piece == 'k' || piece == 'K')
                  {
                     possible2.append(pos2);
                     possible2.append("/");
                  }
               }
            }
            if (checks.size() > 0) possible = possible2;
         }

         Retrieve(pos1);
         return true;
      }
      possible = "";
   }
   return false;
}

bool Board::MouseMove(float x, float y)
{
   if (pPI_CLICK)
   {
      pPI_CLICK->x = x;
      pPI_CLICK->y = y;
      return true;
   }
   return false;
}

bool Board::MouseUp(float x, float y)
{
   float col = (x - super::c_x) / 80;
   float row = (y - super::c_y) / 80;
   if (pPI_CLICK)
   {
      const char pos2[3] = { (char)(col + 'A'), (char)((10 - row) + '0'), '\0' };
      Set(pPI_CLICK->pos, pPI_CLICK->c);
      Move(buffer.at(0), pPI_CLICK->pos, pos2);
      std::cout << pPI_CLICK->pos << " " << pos2 << std::endl;
      delete pPI_CLICK;
      pPI_CLICK = NULL;
      return true;
   }
   return false;
}
