#pragma once
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <iostream>


typedef D2D1_POINT_2F float2;

struct ImageData
{
   ID2D1Bitmap* bmp = NULL;
   D2D1_RECT_F src = {};
};

class Graphics2D
{
private:
   template <class T> void SafeRelease(T*& pT)
   {
      if (NULL != pT)
      {
         pT->Release();
         pT = NULL;
      }
   }
   ID2D1Factory* g_pFactory;
   ID2D1HwndRenderTarget* g_pRT;
   ID2D1SolidColorBrush* g_pBrush;
   IDWriteFactory* g_pWriteFact;
   IDWriteTextFormat* g_pTextFormat;
   IWICImagingFactory* g_pIWICFactory;

   HWND g_hWnd;

   HRESULT CreateGraphicsResources()
   {
      HRESULT hr = S_OK;
      if (g_pRT == NULL)
      {

         hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &g_pFactory);

         if (SUCCEEDED(hr))
         {
            RECT rc;
            GetClientRect(this->g_hWnd, &rc);

            // Size of the drawing area,
            // which in this case is the whole screen
            D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

            // Create the render target
            hr = g_pFactory->CreateHwndRenderTarget(
               D2D1::RenderTargetProperties(),
               D2D1::HwndRenderTargetProperties(this->g_hWnd, size),
               &g_pRT);

            // Create the brush
            if (SUCCEEDED(hr))
            {
               const D2D1_COLOR_F color = { 0.0f, 0.0f, 0.0f, 1.0f };
               hr = g_pRT->CreateSolidColorBrush(color, &g_pBrush);
            }
         }

         // Create the factory needed for writing
         hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(g_pWriteFact),
            reinterpret_cast<IUnknown**>(&g_pWriteFact));

         // Create the text format
         if (SUCCEEDED(hr))
         {
            hr = g_pWriteFact->CreateTextFormat(
               L"Verdana", NULL,
               DWRITE_FONT_WEIGHT_NORMAL,
               DWRITE_FONT_STYLE_NORMAL,
               DWRITE_FONT_STRETCH_NORMAL,
               50, L"", &g_pTextFormat);

            // Set alignment for the text format
            if (SUCCEEDED(hr))
            {
               g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
               g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }
         }
      }
      hr = CoCreateInstance(
         CLSID_WICImagingFactory,
         NULL,
         CLSCTX_INPROC_SERVER,
         IID_PPV_ARGS(&g_pIWICFactory)
      );


      return hr;
   }
   void DiscardGraphicsResources()
   {
      SafeRelease(g_pFactory);
      SafeRelease(g_pRT);
      SafeRelease(g_pBrush);
      SafeRelease(g_pWriteFact);
      SafeRelease(g_pTextFormat);
      SafeRelease(g_pIWICFactory);
   }

public:
   Graphics2D(HWND hWnd) : g_pFactory(NULL), g_pRT(NULL), g_pBrush(NULL),
      g_pWriteFact(NULL), g_pTextFormat(NULL), g_hWnd(hWnd) { CreateGraphicsResources(); }
   ~Graphics2D() { DiscardGraphicsResources(); }

   void SetBrush(D2D1::ColorF color) { if (g_pBrush) g_pBrush->SetColor(color); }

   void BeginDraw() { if (g_pRT && this) g_pRT->BeginDraw(); }
   void EndDraw() { if (g_pRT && this) g_pRT->EndDraw(); }

   void Clear(D2D1::ColorF color) { if (g_pRT) g_pRT->Clear(color); }

   void DrawRect(D2D1_RECT_F rect) { if (g_pRT && g_pBrush) g_pRT->DrawRectangle(rect, g_pBrush); }
   void FillRect(D2D1_RECT_F rect) { if (g_pRT && g_pBrush) g_pRT->FillRectangle(rect, g_pBrush); }
   void DrawCircle(D2D1_ELLIPSE ellipse) { if (g_pRT && g_pBrush) g_pRT->DrawEllipse(ellipse, g_pBrush); }
   void FillCirlce(D2D1_ELLIPSE ellipse) { if (g_pRT && g_pBrush) g_pRT->FillEllipse(ellipse, g_pBrush); }
   void DrawString(LPCWSTR lpText, float x, float y, float fontSize)
   {
      if (g_pWriteFact)
      {
         IDWriteTextLayout* g_pTextLayout;
         int strLen = wcslen(lpText);

         HRESULT hr = g_pWriteFact->CreateTextLayout(
            lpText, strLen, g_pTextFormat,
            (float)strLen * fontSize / 1.75f,
            0.0f, &g_pTextLayout);

         if (SUCCEEDED(hr))
         {
            DWRITE_TEXT_RANGE allText = { 0, (UINT32)strLen };
            hr = g_pTextLayout->SetFontSize(fontSize, allText);
            if (SUCCEEDED(hr))
            {
               g_pRT->DrawTextLayout(
                  D2D1::Point2F(x, y),
                  g_pTextLayout,
                  g_pBrush,
                  D2D1_DRAW_TEXT_OPTIONS_NONE);
            }
         }

         SafeRelease(g_pTextLayout);
      }
   }
   void DrawImage(ID2D1Bitmap* pBitmap, D2D1_RECT_F dest, D2D1_RECT_F src)
   {
      D2D1_SIZE_F size = pBitmap->GetSize();
      g_pRT->DrawBitmap(pBitmap,
         dest,
         1.0f,
         D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
         src);

   }
   HRESULT LoadImageFromResource(LPCWSTR lpName, LPCWSTR lpType, ImageData** ppImageData)
   {
      IWICBitmapDecoder* pDecoder = NULL;
      IWICBitmapFrameDecode* pSource = NULL;
      IWICStream* pStream = NULL;
      IWICFormatConverter* pConverter = NULL;

      HRSRC imageResHandle = NULL;
      HGLOBAL imageResDataHandle = NULL;
      void* pImageFile = NULL;
      DWORD imageFileSize = 0;

      HRESULT hr = S_OK;
      if (!g_pIWICFactory)
      {
         hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&g_pIWICFactory));
      }

      if (SUCCEEDED(hr))
      {
         // Locate the resource.
         imageResHandle = FindResourceW(GetModuleHandle(NULL), lpName, lpType);
         hr = imageResHandle ? S_OK : E_FAIL;
      }

      if (SUCCEEDED(hr))
      {
         // Load the resource.
         imageResDataHandle = LoadResource(GetModuleHandle(NULL), imageResHandle);

         hr = imageResDataHandle ? S_OK : E_FAIL;
      }

      if (SUCCEEDED(hr))
      {
         // Lock it to get a system memory pointer.
         pImageFile = LockResource(imageResDataHandle);

         hr = pImageFile ? S_OK : E_FAIL;
      }
      if (SUCCEEDED(hr))
      {
         // Calculate the size.
         imageFileSize = SizeofResource(NULL, imageResHandle);

         hr = imageFileSize ? S_OK : E_FAIL;

      }

      if (SUCCEEDED(hr))
      {
         // Create a WIC stream to map onto the memory.
         hr = g_pIWICFactory->CreateStream(&pStream);
      }
      if (SUCCEEDED(hr))
      {
         // Initialize the stream with the memory pointer and size.
         hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
         );
      }

      if (SUCCEEDED(hr))
      {
         // Create a decoder for the stream.
         hr = g_pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
         );
      }

      if (SUCCEEDED(hr))
      {
         // Create the initial frame.
         hr = pDecoder->GetFrame(0, &pSource);
      }

      if (SUCCEEDED(hr))
      {
         // Convert the image format to 32bppPBGRA
         // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
         hr = g_pIWICFactory->CreateFormatConverter(&pConverter);
      }

      if (SUCCEEDED(hr))
      {
         hr = pConverter->Initialize(
            pSource,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
         );

         if (SUCCEEDED(hr))
         {
            *ppImageData = new ImageData;

            ImageData* pImageData = *ppImageData;

            //create a Direct2D bitmap from the WIC bitmap.
            hr = g_pRT->CreateBitmapFromWicBitmap(
               pConverter,
               NULL,
               &pImageData->bmp
            );
         }
      }

      SafeRelease(pDecoder);
      SafeRelease(pSource);
      SafeRelease(pStream);
      SafeRelease(pConverter);

      return hr;
   }
};

