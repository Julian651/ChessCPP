#pragma once
#include "Component.h"
template <class DERIVED_TYPE>
class BaseWindow
{
public:
   static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
   {
      DERIVED_TYPE* pThis = NULL;

      if (uMsg == WM_NCCREATE)
      {
         CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
         pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
         SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

         pThis->f_hWnd = hWnd;
      }
      else
      {
         pThis = (DERIVED_TYPE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
      }
      if (pThis)
      {
         return pThis->HandleMessage(uMsg, wParam, lParam);
      }
      else
      {
         return DefWindowProc(hWnd, uMsg, wParam, lParam);
      }
   }

   BaseWindow() : f_hWnd(NULL) { }

   BOOL Create(
      PCWSTR lpWindowName,
      DWORD dwStyle,
      DWORD dwExStyle = 0,
      int x = CW_USEDEFAULT,
      int y = CW_USEDEFAULT,
      int nWidth = CW_USEDEFAULT,
      int nHeight = CW_USEDEFAULT,
      HWND hWndParent = 0,
      HMENU hMenu = 0
   )
   {
      WNDCLASSEX wcex = { 0 };

      wcex.cbSize = sizeof(WNDCLASSEX);
      wcex.lpfnWndProc = WindowProc;
      wcex.hInstance = GetModuleHandle(NULL);
      wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
      wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
      wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
      wcex.lpszClassName = ClassName();
      wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

      RegisterClassEx(&wcex);

      f_hWnd = CreateWindow(ClassName(), lpWindowName, dwStyle,
         x, y, nWidth, nHeight, hWndParent, hMenu, wcex.hInstance, this);

      f_g2d = new Graphics2D(f_hWnd);
      return (f_hWnd ? TRUE : FALSE);
   }

   HWND Window() const { return f_hWnd; }

protected:

   virtual PCWSTR  ClassName() const = 0;
   virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

   HWND f_hWnd;

   Graphics2D* f_g2d = NULL;
   std::list<Component*> f_components = {};

public:

   virtual void SetVisible(bool visible)
   {
      if (f_hWnd)
      {
         if (visible) ShowWindow(f_hWnd, SW_SHOW);
         else ShowWindow(f_hWnd, SW_HIDE);
      }
   }
   virtual void Paint(Graphics2D* g)
   {
      if (g)
      {
         g->Clear(D2D1::ColorF::White);


         if (f_components.size() > 0)
         {
            std::list<Component*>::iterator begin = f_components.begin();
            std::list<Component*>::iterator end = f_components.end();
            for (std::list<Component*>::iterator it = begin; it != end; ++it)
            {
               (*it)->Paint(g);
            }
         }
      }
   }
   virtual void Repaint() { SendMessage(this->Window(), WM_PAINT, 0, 0); }
   virtual void Add(Component* c) { f_components.push_back(c); }
   virtual void Remove(Component* c) { f_components.remove(c); }
   virtual void RemoveAll() { f_components.clear(); }
   virtual void UpdateAll() { for (Component* c : f_components) c->Update(); }
};

class Frame : public BaseWindow<Frame>
{
public:
   Frame() {}
   PCWSTR  ClassName() const { return L"Sample Window Class"; }
   LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
   {
      switch (uMsg)
      {
      case WM_CREATE:
         return 0;
      case WM_DESTROY:
         if (f_g2d)
         {
            delete f_g2d;
            f_g2d = NULL;
         }
         for (Component* c : f_components)
         {
            if (c) { delete c; c = NULL; }
         }
         PostQuitMessage(0);
         return 0;
      case WM_PAINT:
         PAINTSTRUCT ps;
         BeginPaint(f_hWnd, &ps);
         f_g2d->BeginDraw();
         Paint(f_g2d);
         f_g2d->EndDraw();
         EndPaint(f_hWnd, &ps);
         return 0;
      case WM_LBUTTONDOWN:
      {
         float x = LOWORD(lParam);
         float y = HIWORD(lParam);


         for (Component* c : f_components)
         {
            for (MouseEvent* e : c->c_events)
            {
               if (e->MouseClicked(x, y)) Repaint();
            }
         }
      }
         return 0;
      case WM_MOUSEMOVE:
      {
         if (wParam == MK_LBUTTON)
         {
            float x = LOWORD(lParam);
            float y = HIWORD(lParam);
            for (Component* c : f_components)
            {
               for (MouseEvent* e : c->c_events)
               {
                  if (e->MouseMove(x, y)) Repaint();
               }
            }
         }
      }
         return 0;
      case WM_LBUTTONUP:
      {
         float x = LOWORD(lParam);
         float y = HIWORD(lParam);
         for (Component* c : f_components)
         {
            for (MouseEvent* e : c->c_events)
            {
               if (e->MouseUp(x, y)) Repaint();
            }
         }
      }
      default:
         return DefWindowProc(f_hWnd, uMsg, wParam, lParam);
      }
   }
   void Paint(Graphics2D* g) override
   {
      BaseWindow::Paint(g);

      //g->FillRect(D2D1::RectF(0.f, 0.f, 100.f, 100.f));
   }

};