// DataManager.cpp
#include "DataManager.h"

DataManager* DataManager::instance = nullptr;

DataManager* DataManager::getInstance() {
  if (instance == nullptr) {
    instance = new DataManager();
  }
  return instance;
}

bool DataManager::saveFood(const FoodItem& food) {
  if (!food.isValid()) return false;
  
  String namespaceName = "food_" + String(food.id);
  if (!prefs.begin(namespaceName.c_str(), false)) return false;
  
  prefs.putString("name", food.name);
  prefs.putInt("qty", food.quantity);
  prefs.putInt("year", food.year);
  prefs.putInt("month", food.month);
  prefs.putInt("day", food.day);
  prefs.putString("category", food.category);
  prefs.putString("notes", food.notes);
  prefs.putBool("expired", food.isExpired);
  prefs.putLong("update", food.lastUpdateTime);
  
  prefs.end();
  return true;
}

FoodItem DataManager::loadFood(int id) {
  FoodItem food;
  String namespaceName = "food_" + String(id);
  
  if (prefs.begin(namespaceName.c_str(), true)) {
    food.id = id;
    food.name = prefs.getString("name", "");
    food.quantity = prefs.getInt("qty", 0);
    food.year = prefs.getInt("year", 2025);
    food.month = prefs.getInt("month", 1);
    food.day = prefs.getInt("day", 1);
    food.category = prefs.getString("category", "未分类");
    food.notes = prefs.getString("notes", "");
    food.isExpired = prefs.getBool("expired", false);
    food.lastUpdateTime = prefs.getLong("update", 0);
    prefs.end();
  }
  
  return food;
}

bool DataManager::deleteFood(int id) {
  String namespaceName = "food_" + String(id);
  return prefs.remove(namespaceName.c_str());
}

bool DataManager::updateFood(const FoodItem& food) {
  return saveFood(food);
}

int DataManager::getAllFoodIds(int* ids, int maxCount) {
  int count = 0;
  for (int id = 1; id <= 1000 && count < maxCount; id++) {
    String namespaceName = "food_" + String(id);
    if (prefs.begin(namespaceName.c_str(), true)) {
      prefs.end();
      ids[count++] = id;
    }
  }
  return count;
}

int DataManager::getFoodCount() {
  int count = 0;
  for (int id = 1; id <= 1000; id++) {
    String namespaceName = "food_" + String(id);
    if (prefs.begin(namespaceName.c_str(), true)) {
      count++;
      prefs.end();
    }
  }
  return count;
}

int DataManager::getNextId() {
  for (int id = 1; id <= 1000; id++) {
    String namespaceName = "food_" + String(id);
    if (!prefs.begin(namespaceName.c_str(), true)) {
      prefs.end();
      return id;
    }
    prefs.end();
  }
  return -1;
}

void DataManager::clearAllFoods() {
  for (int id = 1; id <= 1000; id++) {
    deleteFood(id);
  }
}