// FoodManager.h
#ifndef FOOD_MANAGER_H
#define FOOD_MANAGER_H

#include <functional>
#include <vector>
#include "FoodItem.h"
#include "DataManager.h"
#include "TimeManager.h"

// 事件类型
enum class FoodEventType {
  DATA_CHANGED,      // 数据整体变化（列表刷新）
  FOOD_ADDED,
  FOOD_UPDATED,
  FOOD_DELETED,
  FOOD_REMOVED,      // 出库
  FOOD_EXPIRED,
  MESSAGE            // 普通消息
};

// 事件数据
struct FoodEvent {
  FoodEventType type;
  int foodId;
  int quantity;
  String message;
};

// 事件监听器接口
class FoodEventListener {
public:
  virtual void onFoodEvent(const FoodEvent& event) = 0;
};

class FoodManager {
private:
  DataManager* dataManager;
  TimeManager* timeManager;  // 时间模块接口（依赖注入）
  std::vector<FoodEventListener*> listeners;
  
  void notifyEvent(const FoodEvent& event);
  
public:
  // 构造函数：注入TimeManager依赖
  FoodManager(TimeManager* tm);
  
  // 添加/移除监听器
  void addListener(FoodEventListener* listener);
  void removeListener(FoodEventListener* listener);
  
  // 食物操作
  bool addFood(const FoodItem& food);
  bool removeFood(int id, int quantity);  // 出库
  bool deleteFood(int id);
  bool updateFood(const FoodItem& food);
  
  // 查询操作
  FoodItem getFood(int id);
  int getAllFoods(FoodItem* foods, int maxCount);
  std::vector<FoodItem> getAllFoods();
  int getFoodsByCategory(const String& category, FoodItem* foods, int maxCount);
  int getExpiredFoods(FoodItem* foods, int maxCount);
  int getExpiringSoonFoods(int days, FoodItem* foods, int maxCount);
  
  // 保质期管理（调用时间模块）
  void updateAllExpiredStatus();
  bool checkIsExpired(int year, int month, int day);
  int getRemainingDays(int year, int month, int day);
  
  // 统计信息
  int getTotalFoodCount();
  int getTotalQuantity();
};

#endif