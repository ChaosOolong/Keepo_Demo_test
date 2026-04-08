// TouchHandler.cpp
#include "TouchHandler.h"
#include "Colors.h"

TouchHandler::TouchHandler() {
  wasTouching = false;
  lastTouchTime = 0;
  currentPoint = {0, 0, false, false, false};
}

void TouchHandler::init() {
  // 触摸已在M5.begin()中初始化
}

void TouchHandler::setCallback(TouchCallback cb) {
  callback = cb;
}

TouchPoint TouchHandler::getTouchPoint() {
  return currentPoint;
}

void TouchHandler::update() {
  M5.update();  // 改为 M5.update()
  
  auto touch = M5.Touch.getDetail();  // 改为 M5.Touch
  
  // 检测触摸状态（state != 0 表示有触摸）
  bool isTouching = (touch.state != 0);
  
  // 更新触摸点状态
  currentPoint.x = touch.x;
  currentPoint.y = touch.y;
  currentPoint.isTouching = isTouching;
  currentPoint.justPressed = (isTouching && !wasTouching);
  currentPoint.justReleased = (!isTouching && wasTouching);
  
  // 防抖：避免重复触发
  if (currentPoint.justPressed && (millis() - lastTouchTime > 200)) {
    lastTouchTime = millis();
    if (callback) {
      callback(currentPoint);
    }
  }
  
  // 显示触摸点反馈
  if (currentPoint.justPressed) {
    M5.Display.fillCircle(touch.x, touch.y, 5, COLOR_TOUCH_FEEDBACK);
    delay(50);
    M5.Display.fillCircle(touch.x, touch.y, 5, COLOR_BG);
  }
  
  wasTouching = isTouching;
}