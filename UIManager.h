// UIManager.h
#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <M5Unified.h>
#include "FoodManager.h"
#include "TimeManager.h"
#include "FoodItem.h"
#include "CameraManager.h"
#include "UIImages.h"
#include "NotoSerifSCFont.h"
#include "NotoSerifSCTitleFont.h"

class UIManager : public FoodEventListener {
public:
    // 屏幕类型
    enum ScreenType {
        SCREEN_MAIN,
        SCREEN_ADD,
        SCREEN_EDIT,
        SCREEN_DETAIL,
        SCREEN_CAMERA,
        SCREEN_CLOUD 
    };
    
    // 按钮动作ID
    enum ButtonAction {
        BTN_ADD = 1,
        BTN_EDIT,
        BTN_DELETE,
        BTN_CLOUD,
        BTN_SAVE,
        BTN_CANCEL,
        BTN_BACK,
        BTN_RECOGNIZE,
        BTN_EXIT_CAM,
        BTN_TAKE_PHOTO,
        BTN_SYNC_UP,
        BTN_SYNC_DOWN,  // 从云端下载
        BTN_SYNC_BACK
    };
    
    struct Button {
        int x, y, w, h;
        String label;
        int actionId;
    };
    
    UIManager(FoodManager* fm, TimeManager* tm);
    ~UIManager();
    void init();
    void update();
    void requestRefresh();
    void setScreen(ScreenType screen);
    void setSelectedFood(int id);
    FoodItem getSelectedFoodItem();
    int getSelectedFood() { return selectedFoodId; }
    ScreenType getCurrentScreen() { return currentScreen; }
    
    // UI交互方法
    int getListItemIndex(int x, int y);
    int getButtonAtPosition(int x, int y);
    int getButtons(Button* btnArray, int maxCount);
    void showMessage(const String& msg);
    
    // 摄像头相关方法
    void updateCameraDisplay();
    CameraManager* getCameraManager() { return &cameraManager; }
    bool isCameraReady() { return cameraInitialized; }

    // 云同步相关方法
    void setCloudSyncCallback(void (*callback)(int action));
    
    // FoodEventListener接口
    void onFoodEvent(const FoodEvent& event) override;

    // 获取当前编辑的食物（用于添加页面拍照）
    FoodItem& getEditingFood() { return editingFood; }
    void setEditingFood(const FoodItem& food) { editingFood = food; }
    
private:
    FoodManager* foodManager;
    TimeManager* timeManager;
    CameraManager cameraManager;
    std::vector<FoodItem> currentFoods;
    ScreenType currentScreen;
    int selectedFoodId;
    int scrollOffset;
    int buttonCount;
    Button buttons[10];
    bool needRefresh;
    unsigned long lastRefreshTime;
    bool cameraInitialized;
    FoodItem editingFood;  // 正在编辑的食物（用于添加页面）
    void (*cloudCallback)(int action);  // 云同步回调
    
    void loadFoodList();
    void drawMainScreen();
    void drawAddScreen();
    void drawEditScreen();
    void drawDetailScreen();
    void drawCameraScreen();
    void drawCloudScreen();
    void drawCameraButtons();  // 新增：绘制摄像头按钮
    void drawFoodList();
    void drawStatusBar();
    void drawButton(const Button& btn);
    void drawButton_black(const Button& btn);
    void drawButton_white(const Button& btn);
    void initCamera();
    const uint16_t* getButtonImage(int actionId) const;
    //lgfx::v1::VLWfont notoSerifFont;
    bool notoFontLoaded = false;
    void applyTitleFont();
    void applyBodyFont();

};

#endif