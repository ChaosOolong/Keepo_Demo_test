#ifndef FOOD_ITEM_H
#define FOOD_ITEM_H

#include <Arduino.h>

struct FoodItem {
  int id = -1;
  String name = "";
  int quantity = 0;
  int year = 2025;
  int month = 1;
  int day = 1;
  String category = "未分类";
  String notes = "";
  bool isExpired = false;
  long lastUpdateTime = 0;
  
  String getExpiryDateString() const {
    char buffer[12];
    sprintf(buffer, "%04d-%02d-%02d", year, month, day);
    return String(buffer);
  }
  
  bool isValid() const {
    return id > 0 && name.length() > 0;
  }
};

#endif