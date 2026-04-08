// TouchHandler.h
#ifndef TOUCH_HANDLER_H
#define TOUCH_HANDLER_H

#include <M5Unified.h>
#include <functional>
#include "UIManager.h"

// 原始触摸数据
struct TouchPoint {
  int x;
  int y;
  bool isTouching;
  bool justPressed;
  bool justReleased;
};

// 触摸回调函数
typedef std::function<void(const TouchPoint&)> TouchCallback;

class TouchHandler {
private:
  TouchCallback callback;
  bool wasTouching;
  TouchPoint currentPoint;
  unsigned long lastTouchTime;
  
public:
  TouchHandler();
  void init();
  void update();
  void setCallback(TouchCallback cb);
  TouchPoint getTouchPoint();
};

#endif