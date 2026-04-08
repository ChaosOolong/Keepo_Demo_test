// CameraManager.h
#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include <M5Unified.h>
#include "M5CoreS3.h"
#include "esp_camera.h"

class CameraManager {
public:
    CameraManager();
    ~CameraManager();
    
    // 初始化摄像头
    bool begin();
    
    // 结束摄像头
    void end();
    
    // 获取摄像头状态
    bool isInitialized() { return initialized; }
    
    // 获取并显示摄像头画面（与示例程序逻辑相同）
    void updateDisplay();
    
    // 获取当前帧数据（用于识别接口）
    uint8_t* getCurrentFrame();
    size_t getFrameWidth();
    size_t getFrameHeight();
    void releaseFrame();
    
    // 设置显示区域（可选，默认全屏）
    void setDisplayArea(int x, int y, int w, int h);
    void resetDisplayArea(); // 重置为全屏
    
private:
    bool initialized;
    int displayX, displayY, displayW, displayH;
    bool useCustomArea;
    
    // 帧缓冲区管理
    uint8_t* currentFrameBuf;
    size_t currentFrameLen;
};

#endif