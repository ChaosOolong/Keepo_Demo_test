// CameraManager.cpp
#include "CameraManager.h"

CameraManager::CameraManager() {
    initialized = false;
    displayX = 0;
    displayY = 0;
    displayW = 320;  // M5CoreS3默认宽度
    displayH = 240;  // M5CoreS3默认高度
    useCustomArea = false;
    currentFrameBuf = nullptr;
    currentFrameLen = 0;
}

CameraManager::~CameraManager() {
    end();
}

bool CameraManager::begin() {
    if (CoreS3.Camera.begin()) {
        initialized = true;
        return true;
    }
    initialized = false;
    return false;
}

void CameraManager::end() {
    if (initialized) {
        // 释放任何未释放的帧
        releaseFrame();
        // 注意：CoreS3.Camera没有显式的end方法，但可以重置状态
        initialized = false;
    }
}

void CameraManager::updateDisplay() {
    if (!initialized) return;
    
    if (CoreS3.Camera.get()) {
        // 获取当前帧
        uint16_t* frameBuffer = (uint16_t*)CoreS3.Camera.fb->buf;
        
        // 保存当前帧副本用于识别（如果需要）
        if (currentFrameBuf) {
            free(currentFrameBuf);
        }
        currentFrameLen = CoreS3.Camera.fb->len;
        currentFrameBuf = (uint8_t*)malloc(currentFrameLen);
        if (currentFrameBuf) {
            memcpy(currentFrameBuf, CoreS3.Camera.fb->buf, currentFrameLen);
        }
        
        // 显示到屏幕
        if (useCustomArea) {
            // 使用自定义显示区域
            CoreS3.Display.pushImage(displayX, displayY, displayW, displayH, frameBuffer);
        } else {
            // 全屏显示
            CoreS3.Display.pushImage(0, 0, displayW, displayH, frameBuffer);
        }
        
        CoreS3.Camera.free();
    }
}

uint8_t* CameraManager::getCurrentFrame() {
  if (currentFrameBuf == nullptr && initialized) {
    if (CoreS3.Camera.get()) {
      currentFrameLen = CoreS3.Camera.fb->len;
      currentFrameBuf = (uint8_t*)malloc(currentFrameLen);
      if (currentFrameBuf) {
        memcpy(currentFrameBuf, CoreS3.Camera.fb->buf, currentFrameLen);
      }
      CoreS3.Camera.free();
      }
  }
    return currentFrameBuf;
}

size_t CameraManager::getFrameWidth() {
    return displayW;
}

size_t CameraManager::getFrameHeight() {
    return displayH;
}

void CameraManager::releaseFrame() {
    if (currentFrameBuf) {
        free(currentFrameBuf);
        currentFrameBuf = nullptr;
        currentFrameLen = 0;
    }
}

void CameraManager::setDisplayArea(int x, int y, int w, int h) {
    displayX = x;
    displayY = y;
    displayW = w;
    displayH = h;
    useCustomArea = true;
}

void CameraManager::resetDisplayArea() {
    useCustomArea = false;
    displayW = 320;
    displayH = 240;
}