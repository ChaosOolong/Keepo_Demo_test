// DataManager.h
#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <Preferences.h>
#include "FoodItem.h"

class DataManager {
private:
  Preferences prefs;
  static DataManager* instance;
  DataManager() {}
  
public:
  static DataManager* getInstance();
  
  // 基础CRUD操作
  bool saveFood(const FoodItem& food);
  FoodItem loadFood(int id);
  bool deleteFood(int id);
  bool updateFood(const FoodItem& food);
  
  // 查询操作
  int getAllFoodIds(int* ids, int maxCount);
  int getFoodCount();
  int getNextId();
  
  // 批量操作
  void clearAllFoods();
};

#endif