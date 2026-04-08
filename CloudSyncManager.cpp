// CloudSyncManager.cpp
#include "CloudSyncManager.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

CloudSyncManager::CloudSyncManager() {
    wifiConnected = false;
    token = "";
    syncCallback = nullptr;
    errorMessage = "";
}

CloudSyncManager::~CloudSyncManager() {
    // 析构函数
}

bool CloudSyncManager::begin(const char* wifiSSID, const char* wifiPassword, const char* ezDataToken) {
    token = String(ezDataToken);
    
    // 连接WiFi
    if (!setupWiFi(wifiSSID, wifiPassword)) {
        errorMessage = "WiFi连接失败";
        return false;
    }
    
    wifiConnected = true;
    errorMessage = "";
    return true;
}

bool CloudSyncManager::setupWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
        M5.Display.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        M5.Display.println("\nWiFi连接成功");
        M5.Display.print("IP地址: ");
        M5.Display.println(WiFi.localIP());
        return true;
    }
    
    M5.Display.println("\nWiFi连接失败");
    return false;
}

bool CloudSyncManager::uploadFoodList(const std::vector<FoodItem>& foods) {
    if (!wifiConnected || token == "") {
        errorMessage = "未连接到WiFi或Token未设置";
        return false;
    }
    
    // 先清空旧的列表
    clearCloudFoodList();
    
    // 将每个食品添加到列表中
    for (const auto& food : foods) {
        if (!uploadFoodItem(food)) {
            errorMessage = "上传失败: " + food.name;
            return false;
        }
        delay(100); // 避免请求过快
    }
    
    errorMessage = "";
    if (syncCallback) syncCallback(true, "上传成功，共" + String(foods.size()) + "条数据");
    return true;
}

bool CloudSyncManager::uploadFoodItem(const FoodItem& food) {
    if (!wifiConnected || token == "") {
        errorMessage = "未连接到WiFi或Token未设置";
        return false;
    }
    
    // 将食品数据编码为字符串格式：id|name|quantity|year|month|day|category|notes
    String foodData = String(food.id) + "|" +
                      food.name + "|" +
                      String(food.quantity) + "|" +
                      String(food.year) + "|" +
                      String(food.month) + "|" +
                      String(food.day) + "|" +
                      food.category + "|" +
                      food.notes;
    
    //使用 setData 存储单个食品
    String topicName = "food_" + String(food.id);
    if (setData(token.c_str(), topicName.c_str(), food.id)) {
         return true;
   }
    
    return false;
}

std::vector<FoodItem> CloudSyncManager::downloadFoodList() {
    std::vector<FoodItem> foods;
    
    if (!wifiConnected || token == "") {
        errorMessage = "未连接到WiFi或Token未设置";
        return foods;
    }
    
    // 使用 HTTP GET 请求从 EzData 获取数据
    String url = "https://ezdata.m5stack.com/api/get?token=" + token + "&list=food_list";
    
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        // 解析返回的数据
        // EzData 返回的是 JSON 数组格式
        foods = parseEzDataResponse(response);
        errorMessage = "";
    } else {
        errorMessage = "HTTP请求失败: " + String(httpCode);
    }
    
    http.end();
    return foods;
}

std::vector<FoodItem> CloudSyncManager::parseEzDataResponse(const String& response) {
    std::vector<FoodItem> foods;
    StaticJsonDocument<4096> doc;
    
    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        return foods;
    }
    
    // EzData 返回格式: {"data": [...]}
    if (doc.containsKey("data") && doc["data"].is<JsonArray>()) {
        JsonArray array = doc["data"].as<JsonArray>();
        for (JsonVariant item : array) {
            String itemStr = item.as<String>();
            FoodItem food = parseFoodDataString(itemStr);
            if (food.id > 0) {
                foods.push_back(food);
            }
        }
    }
    
    return foods;
}

FoodItem CloudSyncManager::parseFoodDataString(const String& dataStr) {
    FoodItem food;
    
    // 解析格式：id|name|quantity|year|month|day|category|notes
    int start = 0;
    int end = dataStr.indexOf('|', start);
    if (end > 0) {
        food.id = dataStr.substring(start, end).toInt();
        
        start = end + 1;
        end = dataStr.indexOf('|', start);
        if (end > 0) {
            food.name = dataStr.substring(start, end);
            
            start = end + 1;
            end = dataStr.indexOf('|', start);
            if (end > 0) {
                food.quantity = dataStr.substring(start, end).toInt();
                
                start = end + 1;
                end = dataStr.indexOf('|', start);
                if (end > 0) {
                    food.year = dataStr.substring(start, end).toInt();
                    
                    start = end + 1;
                    end = dataStr.indexOf('|', start);
                    if (end > 0) {
                        food.month = dataStr.substring(start, end).toInt();
                        
                        start = end + 1;
                        end = dataStr.indexOf('|', start);
                        if (end > 0) {
                            food.day = dataStr.substring(start, end).toInt();
                            
                            start = end + 1;
                            end = dataStr.indexOf('|', start);
                            if (end > 0) {
                                food.category = dataStr.substring(start, end);
                                food.notes = dataStr.substring(end + 1);
                            } else {
                                food.category = dataStr.substring(start);
                            }
                        } else {
                            food.day = dataStr.substring(start).toInt();
                        }
                    } else {
                        food.month = dataStr.substring(start).toInt();
                    }
                } else {
                    food.year = dataStr.substring(start).toInt();
                }
            } else {
                food.quantity = dataStr.substring(start).toInt();
            }
        } else {
            food.name = dataStr.substring(start);
        }
    }
    
    return food;
}

bool CloudSyncManager::clearCloudFoodList() {
    if (!wifiConnected || token == "") {
        errorMessage = "未连接到WiFi或Token未设置";
        return false;
    }
    
    // 使用 removeData 函数删除整个列表
    if (removeData(token.c_str(), "food_list")) {
        errorMessage = "";
        M5.Display.println("列表清除成功");
        if (syncCallback) syncCallback(true, "云端数据已清空");
        return true;
    } else {
        errorMessage = "列表清除失败";
        M5.Display.println("列表清除失败");
        if (syncCallback) syncCallback(false, errorMessage);
        return false;
    }
}

bool CloudSyncManager::syncToCloud(const std::vector<FoodItem>& localFoods) {
    M5.Display.println("正在同步数据到云端...");
    return uploadFoodList(localFoods);
}

std::vector<FoodItem> CloudSyncManager::syncFromCloud() {
    M5.Display.println("正在从云端下载数据...");
    return downloadFoodList();
}

void CloudSyncManager::setSyncCallback(void (*callback)(bool success, const String& message)) {
    syncCallback = callback;
}