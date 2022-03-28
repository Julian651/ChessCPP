#pragma once
#include "Graphics2D.h"
#include <list>
#include "MouseEvent.h"
class Component
{
private:
protected:
   typedef Component super;
public:
   float c_x, c_y, c_width, c_height;
   std::list<MouseEvent*> c_events = {};

   Component(float x, float y, float width, float height) :
      c_x(x), c_y(y), c_width(width), c_height(height) {}
   virtual ~Component()
   {
   }

   virtual void Paint(Graphics2D* g) = 0;
   virtual void Update() = 0;

   void SetLocation(float x, float y) { this->c_x = x; this->c_y = y; }
   void SetSize(float width, float height) { this->c_width = width; this->c_height = height; }
   void AddMouseEvent(MouseEvent* e) { c_events.push_back(e); }

   static void Sort(std::list<Component*> list)
   {
      list.sort([](Component* one, Component* two) { return one->c_x < two->c_x; });
   }
};

