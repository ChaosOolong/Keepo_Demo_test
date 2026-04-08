// CloudSyncManager.h
#ifndef CLOUD_SYNC_MANAGER_H
#define CLOUD_SYNC_MANAGER_H

#include <Arduino.h>
#include <M5Unified.h>
#include <M5_EzData.h>
#include <vector>
#include "FoodItem.h"

class CloudSyncManager {
public:
    CloudSyncManager();
    ~CloudSyncManager();
    
    // 配置WiFi和EzData
    bool begin(const char* wifiSSID, const char* wifiPassword, const char* ezDataToken);
    bool isConnected() { return wifiConnected && token != ""; }
    
    // 上传食品清单到云端
    bool uploadFoodList(const std::vector<FoodItem>& foods);
    bool uploadFoodItem(const FoodItem& food);
    
    // 从云端下载食品清单
    std::vector<FoodItem> downloadFoodList();
    
    // 删除云端数据
    bool clearCloudFoodList();
    
    // 同步数据
    bool syncToCloud(const std::vector<FoodItem>& localFoods);
    std::vector<FoodItem> syncFromCloud();
    
    // 获取连接状态
    bool isWiFiConnected() { return wifiConnected; }
    String getErrorMessage() { return errorMessage; }
    
    // 设置回调函数
    void setSyncCallback(void (*callback)(bool success, const String& message));
    
private:
    bool wifiConnected;
    String token;
    String errorMessage;
    void (*syncCallback)(bool success, const String& message);
    
    bool setupWiFi(const char* ssid, const char* password);
    std::vector<FoodItem> parseEzDataResponse(const String& response);
    FoodItem parseFoodDataString(const String& dataStr);
};

#endif