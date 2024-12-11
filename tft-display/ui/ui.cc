#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_FT6206.h> // Touch Screen Library

#include "ui.h"

void PrintPoint(const TS_Point& p) {
  Serial.print("(");
  Serial.print(p.x);
  Serial.print(",");
  Serial.print(p.y);
  Serial.print(")");
}

void Panel::AddChild(Window* w) {
  w->setParent(this);
  if (!first_child_) {
    first_child_ = w;
  } else {
    Window* last_child = first_child_;
    while (last_child->nextSibling() != nullptr) {
      last_child = last_child->nextSibling();
    }
    last_child->setNextSibling(w);
  }
}

bool Panel::onTouch(const TouchEvent& event) {
  if (!PointInWindow(event.point())) {
    return false;
  }

  for (Window* w = first_child_; w != nullptr; w = w->nextSibling()) {
    if (w->onTouch(event)) {
      return true;
    }
  }
  return false;
}

void Panel::Update() {
  for (Window* w = first_child_; w != nullptr; w = w->nextSibling()) {
    w->Update();
  }
  Window::Update();
}

void Panel::Draw(Adafruit_GFX* gfx) {
  for (Window* w = first_child_; w != nullptr; w = w->nextSibling()) {
    if (w->NeedsRefresh()) {
      w->Draw(gfx);
    }
  }
  Window::Draw(gfx);
}

void Button::Draw(Adafruit_GFX* gfx) {
  gfx->fillRect(x(), y(), cx(), cy(), 0xEF5D);

  gfx->drawFastHLine(x(), y(), cx(), 0xF7BE);
  gfx->drawFastHLine(x(), y() + 1, cx(), 0xF7BE);
  
  gfx->drawFastVLine(x(), y(), cy(), 0xF7BE);
  gfx->drawFastVLine(x() + 1, y(), cy(), 0xF7BE);

  gfx->drawFastHLine(x(), y() + cy() - 1, cx(), 0xBDF7);
  gfx->drawFastHLine(x(), y() + cy() - 2, cx(), 0xBDF7);

  gfx->drawFastVLine(x() + cx() - 1, y(), cy(), 0xBDF7);
  gfx->drawFastVLine(x() + cx() - 2, y(), cy(), 0xBDF7);

  int16_t tx, ty;
  uint16_t bx, by;
  gfx->setTextColor(WHITE);
  gfx->setTextSize(1);
  gfx->getTextBounds(getText(), x(), y(), &tx, &ty, &bx, &by);
  gfx->setCursor(x() + (cx() - bx) / 2, y() + (cy() - by) / 2);
  gfx->print(getText());
  Window::Draw(gfx);
}

bool Button::onTouch(const TouchEvent& event) {
  if (PointInWindow(event.point())) {
    if (event.type() == TouchEventType::End) {
      onPress(event);
    }
    return true;
  }
  return false;
}

void Label::Draw(Adafruit_GFX* gfx) {
  gfx->fillRect(x(), y(), cx(), cy(), bg_);
  gfx->setCursor(x(), y());
  gfx->setTextColor(fg_);
  gfx->setTextSize(size_);
  gfx->print(getText());
  Window::Draw(gfx);
}

void SmoothLabel::Draw(Adafruit_GFX* gfx) {
  for (int y = 0; y < cy(); y++) {
    GFXcanvas16 canvas(cx(), 1);
    canvas.fillScreen(bg_);
    canvas.setCursor(0, 0 - y);
    canvas.setTextColor(fg_);
    canvas.setTextSize(size_);
    canvas.print(getText());
    gfx->drawRGBBitmap(x(), this->y() + y, canvas.getBuffer(),
        canvas.width(), canvas.height());
  }
  Window::Draw(gfx);
}

void DispatchProc(Adafruit_GFX* gfx, Window* mainWindow) {
  mainWindow->Update();
  if (mainWindow->NeedsRefresh()) {
    mainWindow->Draw(gfx);
  }
}

void RotateTouchPoint(TS_Point* p, uint8_t r) {
  uint16_t x, y;
  x = 320 - p->y;
  y = p->x;
  p->x = x;
  p->y = y;
}

void UILoop(Adafruit_GFX* gfx, Adafruit_FT6206* ts, Window* main_window) {
  bool is_touched = false;
  TS_Point last(0,0,0);

  for (;;) {
    TS_Point p(0,0,0);

    if (ts->touched()) {
      p = ts->getPoint();
      RotateTouchPoint(&p, 3);
    }

    if (ts->touched()) {
      if (is_touched) {
        if (p != last) {
          Serial.print("DRAG TO ");
          PrintPoint(p);
          Serial.println();
        }
      } else {
        Serial.print("TOUCH AT ");
        PrintPoint(p);
        Serial.println();
      }
      is_touched = true;
    } else {
      if (is_touched) {
        Serial.println("TOUCH END");
        TouchEvent event(TouchEventType::End, last);
        main_window->onTouch(event);
      }
      is_touched = false;
    }
    last = p;

    DispatchProc(gfx, main_window);
  }
}
