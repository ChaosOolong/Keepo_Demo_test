#include <M5Unified.h>
#include "CameraManager.h"
#include "FoodItem.h"
#include "DataManager.h"
#include "FoodManager.h"
#include "UIManager.h"
#include "TouchHandler.h"
#include "TimeManager.h"
#include "Colors.h"
#include "CloudSyncManager.h"
#include "UIImages.h"
#include "NotoSerifSCFont.h"

// WiFi和EzData配置
const char* WIFI_SSID = "ZPilot";
const char* WIFI_PASSWORD = "zp888888";
const char* EZDATA_TOKEN = "5OWuHdA6uk5SqUCWkQ7q95n9PqdLZHY0";

// DummyTimeManager 实现（保持原有代码不变）
class DummyTimeManager : public TimeManager {
private:
  Date mockDate;
  Time mockTime;
  
public:
  DummyTimeManager() {
    mockDate = Date(2025, 6, 15);
    mockTime = Time(14, 30, 0);
  }
  
  void setMockDate(int year, int month, int day) {
    mockDate = Date(year, month, day);
  }
  
  Date getCurrentDate() override { return mockDate; }
  Time getCurrentTime() override { return mockTime; }
  DateTime getCurrentDateTime() override { return DateTime(mockDate, mockTime); }
  unsigned long getCurrentTimestamp() override { return millis(); }
  
  int compareDates(const Date& date1, const Date& date2) override {
    if (date1.year != date2.year) return date1.year - date2.year;
    if (date1.month != date2.month) return date1.month - date2.month;
    return date1.day - date2.day;
  }
  
  long getDaysDifference(const Date& date1, const Date& date2) override {
    long days1 = (long)date1.year * 365 + (long)date1.month * 30 + date1.day;
    long days2 = (long)date2.year * 365 + (long)date2.month * 30 + date2.day;
    return days1 - days2;
  }
  
  bool isExpired(const Date& expiryDate) override {
    Date currentDate = getCurrentDate();
    return compareDates(expiryDate, currentDate) < 0;
  }
  
  int getRemainingDays(const Date& expiryDate) override {
    Date currentDate = getCurrentDate();
    return -getDaysDifference(expiryDate, currentDate);
  }
  
  String formatDate(const Date& date, const String& format) override {
    String result = format;
    result.replace("2026", String(date.year));
    result.replace("04", String(date.month));
    result.replace("06", String(date.day));
    return result;
  }
  
  Date parseDate(const String& dateStr) override {
    int year, month, day;
    sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day);
    return Date(year, month, day);
  }
};

// 全局对象
DummyTimeManager dummyTimeManager;
FoodManager foodManager(&dummyTimeManager);
UIManager ui(&foodManager, &dummyTimeManager);
TouchHandler touchHandler;
CloudSyncManager cloudSync;

// 云同步回调函数
void onCloudSyncAction(int action) {
    switch(action) {
        case UIManager::BTN_SYNC_UP:
            if (cloudSync.isWiFiConnected()) {
                std::vector<FoodItem> foods = foodManager.getAllFoods();
                if (cloudSync.syncToCloud(foods)) {
                    ui.showMessage("上传成功");
                } else {
                    ui.showMessage("上传失败: " + cloudSync.getErrorMessage());
                }
            } else {
                ui.showMessage("WiFi未连接");
            }
            break;
            
        case UIManager::BTN_SYNC_DOWN:
            if (cloudSync.isWiFiConnected()) {
                std::vector<FoodItem> cloudFoods = cloudSync.syncFromCloud();
                if (!cloudFoods.empty()) {
                    // 清空本地数据并加载云端数据
                    // 注意：这里需要实现清空和批量添加的逻辑
                    ui.showMessage("下载成功，共" + String(cloudFoods.size()) + "条数据");
                } else {
                    ui.showMessage("下载失败或无数据");
                }
            } else {
                ui.showMessage("WiFi未连接");
            }
            break;
            
        case UIManager::BTN_SYNC_BACK:
            ui.setScreen(UIManager::SCREEN_MAIN);
            break;
    }
}

// 处理触摸事件
void onTouch(const TouchPoint& touch) {
  if (!touch.justPressed) return;
  
  int x = touch.x;
  int y = touch.y;
  
  switch (ui.getCurrentScreen()) {
    case UIManager::SCREEN_MAIN: {
      int buttonId = ui.getButtonAtPosition(x, y);
      if (buttonId != -1) {
        switch (buttonId) {
          case UIManager::BTN_ADD:
            ui.setScreen(UIManager::SCREEN_ADD);
            break;
          case UIManager::BTN_EDIT:
            if (ui.getSelectedFood() != -1) {
              ui.setScreen(UIManager::SCREEN_EDIT);
            } else {
              ui.showMessage("请先选择食物");
            }
            break;
          case UIManager::BTN_DELETE:
            if (ui.getSelectedFood() != -1) {
              foodManager.deleteFood(ui.getSelectedFood());
              ui.setSelectedFood(-1);
            } else {
              ui.showMessage("请先选择食物");
            }
            break;
          case UIManager::BTN_CLOUD:
            ui.setScreen(UIManager::SCREEN_CLOUD);
            break;
        }
        return;
      }
      
      int listIndex = ui.getListItemIndex(x, y);
      if (listIndex != -1) {
        std::vector<FoodItem> foods = foodManager.getAllFoods();
        if (listIndex < (int)foods.size()) {
          ui.setSelectedFood(foods[listIndex].id);
        }
      }
      break;
    }
  
    case UIManager::SCREEN_ADD: {
      int buttonId = ui.getButtonAtPosition(x, y);
      if (buttonId == UIManager::BTN_SAVE) {
        FoodItem newFood;
        newFood.name = "苹果";
        newFood.quantity = 1;
        Date currentDate = dummyTimeManager.getCurrentDate();
        newFood.year = currentDate.year;
        newFood.month = currentDate.month + 1;
        newFood.day = currentDate.day;
        if (newFood.month > 12) {
          newFood.month = 1;
          newFood.year++;
        }
        foodManager.addFood(newFood);
        ui.setScreen(UIManager::SCREEN_MAIN);
      } else if (buttonId == UIManager::BTN_CANCEL) {
        ui.setScreen(UIManager::SCREEN_MAIN);
      } else if (buttonId == UIManager::BTN_TAKE_PHOTO) {
        // 拍照按钮：打开摄像头
        ui.setScreen(UIManager::SCREEN_CAMERA);
      }
      break;
    }
    
    case UIManager::SCREEN_EDIT: {
      int buttonId = ui.getButtonAtPosition(x, y);
      if (buttonId == UIManager::BTN_SAVE) {
        FoodItem food = ui.getSelectedFoodItem();
        if (food.isValid()) {
          food.quantity += 1;
          foodManager.updateFood(food);
        }
        ui.setScreen(UIManager::SCREEN_MAIN);
      } else if (buttonId == UIManager::BTN_BACK) {
        ui.setScreen(UIManager::SCREEN_MAIN);
      }
      break;
    }
    
    case UIManager::SCREEN_DETAIL: {
      int buttonId = ui.getButtonAtPosition(x, y);
      if (buttonId == UIManager::BTN_BACK) {
        ui.setScreen(UIManager::SCREEN_MAIN);
      }
      break;
    }
    
    case UIManager::SCREEN_CAMERA: {
    int buttonId = ui.getButtonAtPosition(x, y);
    if (buttonId == UIManager::BTN_RECOGNIZE) {
        // 使用CameraManager获取当前帧进行识别
        CameraManager* cam = ui.getCameraManager();
        if (cam->isInitialized()) {
            uint8_t* frameData = cam->getCurrentFrame();
            if (frameData) {
                // TODO: 调用识别接口处理 frameData
                //ui.showMessage("识别功能开发中...");
                cam->releaseFrame(); // 释放帧数据
            } else {
                ui.showMessage("请等待画面稳定");
            }
        } else {
            ui.showMessage("摄像头未就绪");
        }
    } else if (buttonId == UIManager::BTN_EXIT_CAM || buttonId == UIManager::BTN_BACK) {
        ui.setScreen(UIManager::SCREEN_MAIN);
    }
    break;
  }

  case UIManager::SCREEN_CLOUD: {
    int buttonId = ui.getButtonAtPosition(x, y);
    if (buttonId == UIManager::BTN_SYNC_UP || 
        buttonId == UIManager::BTN_SYNC_DOWN ||
        buttonId == UIManager::BTN_SYNC_BACK) {
      onCloudSyncAction(buttonId);
    }
    break;
  }
  }
}

void setup() {
  M5.begin();
  auto cfg = M5.config();
  M5.begin(cfg);
  
  Serial.begin(115200);
  M5.Display.setFont(&fonts::efontCN_24);

  // 初始化云同步
  M5.Display.println("正在连接WiFi...");
  if (cloudSync.begin(WIFI_SSID, WIFI_PASSWORD, EZDATA_TOKEN)) {
    M5.Display.println("云同步初始化成功");
  } else {
    M5.Display.println("云同步初始化失败");
  }

  // 初始化其他模块
  foodManager.addListener(&ui);
  ui.init();
  ui.setCloudSyncCallback(onCloudSyncAction);
  touchHandler.init();
  touchHandler.setCallback(onTouch);
 /* 
  // 添加一些示例数据
  FoodItem sampleFood;
  sampleFood.name = "香蕉";
  sampleFood.quantity = 4;
  sampleFood.year = 2025;
  sampleFood.month = 12;
  sampleFood.day = 31;
  foodManager.addFood(sampleFood);
  
  sampleFood.name = "牛奶";
  sampleFood.quantity = 2;
  sampleFood.year = 2025;
  sampleFood.month = 6;
  sampleFood.day = 30;
  foodManager.addFood(sampleFood);
  
  foodManager.updateAllExpiredStatus();
*/
  M5.Display.fillScreen(COLOR_BG);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(COLOR_TITLE);
  M5.Display.setCursor(50, 120);
  M5.Display.println("冰箱管理系统");
  delay(1500);
  
  ui.setScreen(UIManager::SCREEN_MAIN);
}

void loop() {
  touchHandler.update();
  
  // 如果在摄像头屏幕，持续更新摄像头画面（与示例程序逻辑完全相同）
  if (ui.getCurrentScreen() == UIManager::SCREEN_CAMERA) {
    ui.updateCameraDisplay();
  }
  
  // 每秒更新时间显示
  static unsigned long lastTimeUpdate = 0;
  if (millis() - lastTimeUpdate > 1000) {
    if (ui.getCurrentScreen() == UIManager::SCREEN_MAIN) {
      ui.requestRefresh();
      ui.update();
    }
    lastTimeUpdate = millis();
  }
  
  delay(10);
}