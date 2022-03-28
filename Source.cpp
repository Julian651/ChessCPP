#include "BaseWindow.h"
#include "Board.h"
int main()
{
   Frame* fr = new Frame();
   if (!fr->Create(L"OKAY", WS_OVERLAPPEDWINDOW, 0, 0, 0, 1000, 1000))
   {
      return 0;
   }
   fr->SetVisible(true);
   fr->Add(new Board(100.f, 100.f, "w rnbqkbnr/pppppppp/11111111/11111111/11111111/11111111/PPPPPPPP/RNBQKBNR KQkq - 0 1"));


   // Run the message loop.

   MSG msg;
   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   delete fr;

   return 0;
}