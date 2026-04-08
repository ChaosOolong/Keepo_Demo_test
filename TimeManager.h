// TimeManager.h
#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>

// 日期结构体
struct Date {
  int year;
  int month;
  int day;
  
  Date() : year(2025), month(1), day(1) {}
  Date(int y, int m, int d) : year(y), month(m), day(d) {}
  
  String toString() const {
    char buffer[12];
    sprintf(buffer, "%04d-%02d-%02d", year, month, day);
    return String(buffer);
  }
  
  String toTimeString() const {
    char buffer[20];
    sprintf(buffer, "%04d-%02d-%02d", year, month, day);
    return String(buffer);
  }
};

// 时间结构体
struct Time {
  int hour;
  int minute;
  int second;
  
  Time() : hour(0), minute(0), second(0) {}
  Time(int h, int m, int s) : hour(h), minute(m), second(s) {}
  
  String toString() const {
    char buffer[10];
    sprintf(buffer, "%02d:%02d:%02d", hour, minute, second);
    return String(buffer);
  }
  
  String toShortString() const {
    char buffer[6];
    sprintf(buffer, "%02d:%02d", hour, minute);
    return String(buffer);
  }
};

// 日期时间结构体
struct DateTime {
  Date date;
  Time time;
  
  DateTime() {}
  DateTime(const Date& d, const Time& t) : date(d), time(t) {}
  
  String toDateString() const {
    return date.toString();
  }
  
  String toTimeString() const {
    return time.toShortString();
  }
  
  String toString() const {
    return date.toString() + " " + time.toString();
  }
};

// 时间接口类（纯虚函数）
class TimeManager {
public:
  virtual ~TimeManager() {}
  
  // 获取当前日期
  virtual Date getCurrentDate() = 0;
  
  // 获取当前时间
  virtual Time getCurrentTime() = 0;
  
  // 获取当前日期时间
  virtual DateTime getCurrentDateTime() = 0;
  
  // 获取当前时间戳（毫秒）
  virtual unsigned long getCurrentTimestamp() = 0;
  
  // 比较两个日期
  virtual int compareDates(const Date& date1, const Date& date2) = 0;
  
  // 计算两个日期之间的天数差
  virtual long getDaysDifference(const Date& date1, const Date& date2) = 0;
  
  // 检查日期是否过期（相对于当前日期）
  virtual bool isExpired(const Date& expiryDate) = 0;
  
  // 获取剩余天数（正数表示还有多少天过期，负数表示已过期多少天）
  virtual int getRemainingDays(const Date& expiryDate) = 0;
  
  // 格式化日期为字符串
  virtual String formatDate(const Date& date, const String& format = "YYYY-MM-DD") = 0;
  
  // 解析字符串为日期
  virtual Date parseDate(const String& dateStr) = 0;
};

#endif