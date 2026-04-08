// FoodManager.cpp
#include "FoodManager.h"

FoodManager::FoodManager(TimeManager* tm) {
  dataManager = DataManager::getInstance();
  timeManager = tm;  // 依赖注入
}

void FoodManager::addListener(FoodEventListener* listener) {
  listeners.push_back(listener);
}

void FoodManager::removeListener(FoodEventListener* listener) {
  for (auto it = listeners.begin(); it != listeners.end(); ++it) {
    if (*it == listener) {
      listeners.erase(it);
      break;
    }
  }
}

void FoodManager::notifyEvent(const FoodEvent& event) {
  for (auto listener : listeners) {
    listener->onFoodEvent(event);
  }
}

bool FoodManager::addFood(const FoodItem& food) {
  FoodItem newFood = food;
  if (newFood.id <= 0) {
    newFood.id = dataManager->getNextId();
  }
  newFood.lastUpdateTime = timeManager->getCurrentTimestamp();
  newFood.isExpired = checkIsExpired(newFood.year, newFood.month, newFood.day);
  
  if (dataManager->saveFood(newFood)) {
    FoodEvent event{FoodEventType::FOOD_ADDED, newFood.id, newFood.quantity, "食物已添加"};
    notifyEvent(event);
    
    FoodEvent event2{FoodEventType::DATA_CHANGED, -1, 0, ""};
    notifyEvent(event2);
    return true;
  }
  return false;
}

bool FoodManager::removeFood(int id, int quantity) {
  FoodItem food = dataManager->loadFood(id);
  if (!food.isValid()) {
    FoodEvent event{FoodEventType::MESSAGE, id, quantity, "食物不存在"};
    notifyEvent(event);
    return false;
  }
  
  if (food.quantity < quantity) {
    FoodEvent event{FoodEventType::MESSAGE, id, quantity, "库存不足"};
    notifyEvent(event);
    return false;
  }
  
  food.quantity -= quantity;
  food.lastUpdateTime = timeManager->getCurrentTimestamp();
  
  if (food.quantity == 0) {
    if (dataManager->deleteFood(id)) {
      FoodEvent event{FoodEventType::FOOD_REMOVED, id, quantity, "食物已耗尽"};
      notifyEvent(event);
      FoodEvent event2{FoodEventType::FOOD_DELETED, id, 0, "食物已从列表删除"};
      notifyEvent(event2);
      FoodEvent event3{FoodEventType::DATA_CHANGED, -1, 0, ""};
      notifyEvent(event3);
      return true;
    }
  } else {
    if (dataManager->updateFood(food)) {
      FoodEvent event{FoodEventType::FOOD_REMOVED, id, quantity, "出库成功"};
      notifyEvent(event);
      FoodEvent event2{FoodEventType::FOOD_UPDATED, id, food.quantity, ""};
      notifyEvent(event2);
      FoodEvent event3{FoodEventType::DATA_CHANGED, -1, 0, ""};
      notifyEvent(event3);
      return true;
    }
  }
  
  return false;
}

bool FoodManager::deleteFood(int id) {
  FoodItem food = dataManager->loadFood(id);
  if (!food.isValid()) return false;
  
  if (dataManager->deleteFood(id)) {
    FoodEvent event{FoodEventType::FOOD_DELETED, id, 0, "食物已删除"};
    notifyEvent(event);
    FoodEvent event2{FoodEventType::DATA_CHANGED, -1, 0, ""};
    notifyEvent(event2);
    return true;
  }
  return false;
}

bool FoodManager::updateFood(const FoodItem& food) {
  FoodItem updatedFood = food;
  updatedFood.lastUpdateTime = timeManager->getCurrentTimestamp();
  updatedFood.isExpired = checkIsExpired(updatedFood.year, updatedFood.month, updatedFood.day);
  
  if (dataManager->updateFood(updatedFood)) {
    FoodEvent event{FoodEventType::FOOD_UPDATED, updatedFood.id, updatedFood.quantity, "食物已更新"};
    notifyEvent(event);
    FoodEvent event2{FoodEventType::DATA_CHANGED, -1, 0, ""};
    notifyEvent(event2);
    return true;
  }
  return false;
}

FoodItem FoodManager::getFood(int id) {
  return dataManager->loadFood(id);
}

int FoodManager::getAllFoods(FoodItem* foods, int maxCount) {
  int* ids = new int[maxCount];
  int count = dataManager->getAllFoodIds(ids, maxCount);
  
  for (int i = 0; i < count && i < maxCount; i++) {
    foods[i] = dataManager->loadFood(ids[i]);
  }
  
  delete[] ids;
  return count;
}

std::vector<FoodItem> FoodManager::getAllFoods() {
  std::vector<FoodItem> result;
  int ids[200];
  int count = dataManager->getAllFoodIds(ids, 200);
  
  for (int i = 0; i < count; i++) {
    result.push_back(dataManager->loadFood(ids[i]));
  }
  
  return result;
}

int FoodManager::getExpiredFoods(FoodItem* foods, int maxCount) {
  int* ids = new int[maxCount];
  int totalCount = dataManager->getAllFoodIds(ids, maxCount);
  int expiredCount = 0;
  
  for (int i = 0; i < totalCount && expiredCount < maxCount; i++) {
    FoodItem food = dataManager->loadFood(ids[i]);
    if (food.isExpired) {
      foods[expiredCount++] = food;
    }
  }
  
  delete[] ids;
  return expiredCount;
}

int FoodManager::getExpiringSoonFoods(int days, FoodItem* foods, int maxCount) {
  int* ids = new int[maxCount];
  int totalCount = dataManager->getAllFoodIds(ids, maxCount);
  int soonCount = 0;
  
  for (int i = 0; i < totalCount && soonCount < maxCount; i++) {
    FoodItem food = dataManager->loadFood(ids[i]);
    int remaining = getRemainingDays(food.year, food.month, food.day);
    if (!food.isExpired && remaining >= 0 && remaining <= days) {
      foods[soonCount++] = food;
    }
  }
  
  delete[] ids;
  return soonCount;
}

// 调用时间模块接口
bool FoodManager::checkIsExpired(int year, int month, int day) {
  if (timeManager == nullptr) {
    // 如果没有时间模块，默认未过期
    return false;
  }
  Date expiryDate(year, month, day);
  return timeManager->isExpired(expiryDate);
}

int FoodManager::getRemainingDays(int year, int month, int day) {
  if (timeManager == nullptr) {
    // 如果没有时间模块，返回一个大数值
    return 365;
  }
  Date expiryDate(year, month, day);
  return timeManager->getRemainingDays(expiryDate);
}

void FoodManager::updateAllExpiredStatus() {
  if (timeManager == nullptr) return;
  
  std::vector<FoodItem> foods = getAllFoods();
  bool needUpdate = false;
  
  for (auto& food : foods) {
    Date expiryDate(food.year, food.month, food.day);
    bool newStatus = timeManager->isExpired(expiryDate);
    if (food.isExpired != newStatus) {
      food.isExpired = newStatus;
      dataManager->updateFood(food);
      needUpdate = true;
      if (newStatus) {
        FoodEvent event{FoodEventType::FOOD_EXPIRED, food.id, 0, "食物已过期"};
        notifyEvent(event);
      }
    }
  }
  
  if (needUpdate) {
    FoodEvent event{FoodEventType::DATA_CHANGED, -1, 0, ""};
    notifyEvent(event);
  }
}

int FoodManager::getTotalFoodCount() {
  return dataManager->getFoodCount();
}

int FoodManager::getTotalQuantity() {
  std::vector<FoodItem> foods = getAllFoods();
  int total = 0;
  for (const auto& food : foods) {
    total += food.quantity;
  }
  return total;
}