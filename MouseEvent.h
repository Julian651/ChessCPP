#pragma once
class MouseEvent
{
public:
   virtual bool MouseClicked(float x, float y) = 0;
   virtual bool MouseMove(float x, float y) = 0;
   virtual bool MouseUp(float x, float y) = 0;
};
