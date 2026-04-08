// UIManager.cpp
#include "UIManager.h"
#include "Colors.h"
#include "UIImages.h"

UIManager::UIManager(FoodManager* fm, TimeManager* tm) {
    foodManager = fm;
    timeManager = tm;
    currentScreen = SCREEN_MAIN;
    selectedFoodId = -1;
    scrollOffset = 0;
    buttonCount = 0;
    needRefresh = true;
    lastRefreshTime = 0;
    cameraInitialized = false;
    cloudCallback = nullptr;
}

UIManager::~UIManager() {
    cameraManager.end();
}

void UIManager::init() {
    M5.Display.setRotation(1);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(COLOR_TITLE, COLOR_BG);
    M5.Display.setColorDepth(16);
    M5.Display.fillScreen(COLOR_BG);
        notoFontLoaded = true;
   /* if (notoFontLoaded) {
        notoSerifFont = *(static_cast<lgfx::v1::VLWfont*>(M5.Display.getFont()));
    }*/

    loadFoodList();
    
    // 初始化摄像头
    initCamera();
    
    needRefresh = true;
}

void UIManager::initCamera() {
    if (cameraManager.begin()) {
        cameraInitialized = true;
    } else {
        cameraInitialized = false;
    }
}

void UIManager::applyTitleFont() {
    if (notoFontLoaded) {
        M5.Display.loadFont(font_noto_serif_sc_vlw);   // 24号，大的
    } else {
        M5.Display.setFont(&fonts::Font2);
    }
    M5.Display.setTextSize(1);
}

void UIManager::applyBodyFont() {
    if (notoFontLoaded) {
        M5.Display.loadFont(font_noto_serif_sc_title_vlw);   // 20号，小的
    } else {
        M5.Display.setFont(&fonts::Font2);
    }
    M5.Display.setTextSize(1);
}
void UIManager::update() {
    if (!needRefresh) return;
    if (millis() - lastRefreshTime < 50) return;
    
    switch (currentScreen) {
        case SCREEN_MAIN:
            drawMainScreen();
            break;
        case SCREEN_ADD:
            drawAddScreen();
            break;
        case SCREEN_EDIT:
            drawEditScreen();
            break;
        case SCREEN_DETAIL:
            drawDetailScreen();
            break;
        case SCREEN_CAMERA:
            drawCameraScreen();
            break;
        case SCREEN_CLOUD:
            drawCloudScreen();
            break;
    }
    
    needRefresh = false;
    lastRefreshTime = millis();
}

void UIManager::updateCameraDisplay() {
    if (currentScreen == SCREEN_CAMERA && cameraInitialized) {
        // 设置摄像头显示区域（避开底部按钮区域）
        cameraManager.setDisplayArea(0, 0, 320, 240);
        cameraManager.updateDisplay();
        cameraManager.resetDisplayArea();
        
        // 在摄像头画面上方重新绘制按钮
        drawCameraButtons();
    }
}

void UIManager::drawCameraButtons() {
    // 绘制半透明背景条
    M5.Display.fillRect(0, 170, 320, 70, COLOR_BG);
    M5.Display.drawRect(0, 170, 320, 70, COLOR_BORDER);
    
    // 绘制按钮
    buttonCount = 0;
    buttons[buttonCount++] = {40, 185, 107, 32, "识别", BTN_RECOGNIZE};
    buttons[buttonCount++] = {180, 185, 107, 32, "退出", BTN_EXIT_CAM};
        for (int i = 0; i < buttonCount; i++) {
            drawButton(buttons[i]);
    }

}

void UIManager::drawCameraScreen() {
    if (!cameraInitialized) {
        M5.Display.fillScreen(COLOR_BG);
        M5.Display.setTextSize(2);
        M5.Display.setTextColor(COLOR_ERROR);
        M5.Display.setCursor(10, 100);
        M5.Display.println("摄像头初始化失败");
        
        buttonCount = 0;
        buttons[buttonCount++] = {10, 200, 300, 40, "返回", BTN_BACK};
        drawButton(buttons[0]);
        return;
    }
    
    // 初始显示：先绘制摄像头画面和按钮
    drawCameraButtons();
}

void UIManager::drawCloudScreen() {
    M5.Display.fillScreen(COLOR_BG);
    drawStatusBar();
    
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 35);
    M5.Display.println("云同步");
    
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 80);
    M5.Display.println("选择同步方式：");
    
    buttonCount = 0;
    buttons[buttonCount++] = {40, 120, 100, 40, "上传", BTN_SYNC_UP};
    buttons[buttonCount++] = {180, 120, 100, 40, "下载", BTN_SYNC_DOWN};
    buttons[buttonCount++] = {110, 190, 100, 40, "返回", BTN_SYNC_BACK};
    
    for (int i = 0; i < buttonCount-1; i++) {
        drawButton_black(buttons[i]);
    }
        drawButton_white(buttons[2]);
}

void UIManager::requestRefresh() {
    needRefresh = true;
}

void UIManager::loadFoodList() {
    currentFoods = foodManager->getAllFoods();
}

void UIManager::drawMainScreen() {
 drawStatusBar();

    M5.Display.fillRect(0, 26, 320, 240, COLOR_BG);

    applyTitleFont();
    M5.Display.setTextColor(COLOR_TITLE, COLOR_BG);
    M5.Display.setCursor(10, 32);
    M5.Display.println("冰箱食物清单");

    applyBodyFont();
    M5.Display.setTextColor(COLOR_TITLE, COLOR_BG);
    M5.Display.setCursor(10, 58);
 //   M5.Display.printf("共 %d 种食物", (int)currentFoods.size());
    M5.Display.printf("共 %d 种食物", 3);
   // M5.Display.drawRect(20, 68, 310, 110, COLOR_BORDER);
    drawFoodList();
    
    // 底部按钮：添加、编辑、删除、云同步
    buttonCount = 0;
//buttonCount = 0;

//四按钮
//buttons[buttonCount++] = {14, 195, 66, 45, "添加", BTN_ADD};
//buttons[buttonCount++] = {89, 195, 66, 45, "编辑", BTN_EDIT};
//buttons[buttonCount++] = {164, 195, 66, 45, "删除", BTN_DELETE};
//buttons[buttonCount++] = {239, 195, 66, 45, "云同步", BTN_CLOUD};

//三按钮
buttons[buttonCount++] = {49, 187, 66, 45, "添加", BTN_ADD};
buttons[buttonCount++] = {127, 187, 66, 45, "编辑", BTN_EDIT};
buttons[buttonCount++] = {205, 187, 66, 45, "删除", BTN_DELETE};

    for (int i = 0; i < buttonCount; i++) {
        drawButton(buttons[i]);
    }
}

void UIManager::drawAddScreen() {
    drawStatusBar();
    
    M5.Display.fillRect(0, 26, 320, 240, COLOR_BG);
    
    applyTitleFont();
    M5.Display.setCursor(10, 35);
    M5.Display.println("添加食物");
    
    applyBodyFont();
    M5.Display.drawRect(10, 70, 300, 35, COLOR_BORDER);
    M5.Display.setCursor(20, 80);
    M5.Display.println("名称:");
    
    M5.Display.drawRect(10, 115, 300, 35, COLOR_BORDER);
    M5.Display.setCursor(20, 125);
    M5.Display.println("数量:");
    
    M5.Display.drawRect(10, 160, 300, 35, COLOR_BORDER);
    M5.Display.setCursor(20, 170);
    M5.Display.println("保质期(YYYY-MM-DD):");
    
    // 添加页面按钮：保存、取消、拍照
    buttonCount = 0;
    buttons[buttonCount++] = {10, 190, 90, 40, "保存", BTN_SAVE};
    buttons[buttonCount++] = {115, 190, 90, 40, "取消", BTN_CANCEL};
    buttons[buttonCount++] = {220, 190, 90, 40, "拍照", BTN_TAKE_PHOTO};
    
    for (int i = 0; i < buttonCount; i++) {
        drawButton(buttons[i]);
    }
}

void UIManager::drawEditScreen() {
    FoodItem food = getSelectedFoodItem();
    
    drawStatusBar();
    
    M5.Display.fillRect(0, 26, 320, 240, COLOR_BG);
    applyTitleFont();
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 35);
    M5.Display.printf("编辑: %s", food.name.c_str());
    
    applyBodyFont();
    M5.Display.setCursor(10, 80);
    M5.Display.printf("数量:       %d", food.quantity);
    M5.Display.setCursor(10, 115);
    M5.Display.printf("保质期:    %s", food.getExpiryDateString().c_str());
    
    buttonCount = 0;
    buttons[buttonCount++] = {40, 185, 100, 40, "返回", BTN_BACK};//10, 190, 140, 40
    buttons[buttonCount++] = {180, 185, 100, 40, "修改", BTN_SAVE};//170, 190, 140, 40
    for (int i = 0; i < buttonCount; i++) {
        drawButton(buttons[i]);
    }

}

void UIManager::drawDetailScreen() {
    FoodItem food = getSelectedFoodItem();
    
    drawStatusBar();
    
    M5.Display.fillRect(0, 26, 320, 240, COLOR_BG);
    
    applyTitleFont();
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 35);
    M5.Display.println("食物详情");
    
    applyBodyFont();
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 75);
    M5.Display.printf("名称: %s", food.name.c_str());
    M5.Display.setCursor(10, 100);
    M5.Display.printf("数量: %d", food.quantity);
    M5.Display.setCursor(10, 125);
    M5.Display.printf("保质期: %s", food.getExpiryDateString().c_str());
    M5.Display.setCursor(10, 150);
    M5.Display.printf("分类: %s", food.category.c_str());
    M5.Display.setCursor(10, 175);
    M5.Display.printf("状态: %s", food.isExpired ? "已过期" : "正常");
    
    if (!food.isExpired && timeManager != nullptr) {
        Date expiryDate(food.year, food.month, food.day);
        int remainingDays = timeManager->getRemainingDays(expiryDate);
        M5.Display.setCursor(10, 200);
        M5.Display.printf("剩余: %d天", remainingDays);
    }
    
    buttonCount = 0;
    buttons[buttonCount++] = {10, 220, 140, 40, "返回", BTN_BACK};
    
    for (int i = 0; i < buttonCount; i++) {
        drawButton(buttons[i]);
    }
}

void UIManager::drawFoodList() {
    applyBodyFont();
    M5.Display.fillRect(0, 82, 320, 27, COLOR_URGENCY);   
    M5.Display.fillRect(0, 109, 320, 27, COLOR_MIDDLE);
    M5.Display.fillRect(0, 136, 320, 27, COLOR_BTN);
    
    int displayCount = currentFoods.size();
    if (displayCount > 3) displayCount = 3;
    
    for (int i = 0; i < displayCount; i++) {
        int y = 85 + i * 28;
        
        if (currentFoods[i].id == selectedFoodId) {
            M5.Display.fillRect(6, y-3 , 308, 27, COLOR_HIGHLIGHT);
            M5.Display.setTextColor(BLACK);
        } else {
            M5.Display.setTextColor(COLOR_TITLE);
        }
        
        M5.Display.setCursor(10, y);
        M5.Display.printf("%s x%d", 
            currentFoods[i].name.c_str(), 
            currentFoods[i].quantity);
        
        if (currentFoods[i].isExpired) {
            M5.Display.setTextColor(COLOR_ERROR);
            M5.Display.setCursor(250, y);
            M5.Display.println("过期");
        } else if (timeManager != nullptr) {
            Date expiryDate(currentFoods[i].year, currentFoods[i].month, currentFoods[i].day);
            int remaining = timeManager->getRemainingDays(expiryDate);
            if (remaining >= 0 && remaining <= 3) {
                M5.Display.setTextColor(COLOR_WARNING);
                M5.Display.setCursor(250, y);
                M5.Display.printf("%d天", remaining);
            }
        }
        
        M5.Display.setTextColor(COLOR_TITLE);
    }
}

void UIManager::drawStatusBar() {
    applyBodyFont();
    M5.Display.fillRect(0, 0, 320, 25, COLOR_TITLE);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(0x0000);
    M5.Display.setCursor(3, 3);
    M5.Display.println("冰箱管理系统");
    
    if (timeManager != nullptr) {
        DateTime now = timeManager->getCurrentDateTime();
        M5.Display.setCursor(260, 4);
        M5.Display.println(now.toTimeString());
    } else {
        M5.Display.setCursor(260, 4);
        M5.Display.println("--:--");
    }
}

void UIManager::drawButton_black(const Button& btn) {
    M5.Display.fillRoundRect(btn.x, btn.y, btn.w, btn.h, 5, COLOR_BTN_BLACK);
    M5.Display.drawRoundRect(btn.x, btn.y, btn.w, btn.h, 5, COLOR_BORDER);

    applyBodyFont();
    M5.Display.setTextColor(WHITE, COLOR_BTN_BLACK);

    // 以中心点为基准绘制
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString(btn.label, btn.x + btn.w / 2, btn.y + btn.h / 2);

    // 用完恢复默认，避免影响别的地方
    M5.Display.setTextDatum(top_left);
}

void UIManager::drawButton_white(const Button& btn) {
    M5.Display.fillRoundRect(btn.x, btn.y, btn.w, btn.h, 5, WHITE);
    M5.Display.drawRoundRect(btn.x, btn.y, btn.w, btn.h, 5, COLOR_BORDER);

    applyBodyFont();
    M5.Display.setTextColor(COLOR_BTN_TEXT, WHITE);

    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString(btn.label, btn.x + btn.w / 2, btn.y + btn.h / 2);

    M5.Display.setTextDatum(top_left);
}
const uint16_t* UIManager::getButtonImage(int actionId) const {
    switch (actionId) {
        case BTN_ADD:
            return img_add_button;
        case BTN_EDIT:
            return img_amend_button;
        case BTN_DELETE:
            return img_outbound_button;
        case BTN_RECOGNIZE:
        case BTN_SAVE:
            return img_confirm_button;
        case BTN_EXIT_CAM:
            return img_return_button;
        case BTN_BACK:
            return img_return_button;
        default:
            return nullptr;
    }
}

void UIManager::drawButton(const Button& btn) {
    const uint16_t* img = getButtonImage(btn.actionId);

    // 66x45 小图标按钮：添加 / 编辑 / 删除
    bool isMainImageButton =
        (img != nullptr && btn.w == IMG_MAIN_BTN_W && btn.h == IMG_MAIN_BTN_H);

    // 107x32 长按钮图片：确认 / 返回
    bool isLongImageButton =
        ((btn.actionId == BTN_SAVE || btn.actionId == BTN_RECOGNIZE ||
          btn.actionId == BTN_BACK || btn.actionId == BTN_EXIT_CAM) &&
         img != nullptr &&
         btn.w == IMG_LONG_BTN_W && btn.h == IMG_LONG_BTN_H);

    if (isMainImageButton || isLongImageButton) {
        bool oldSwap = M5.Display.getSwapBytes();
        M5.Display.setSwapBytes(true);   // 如果颜色又反了，再改成 false 试一次
        M5.Display.pushImage(btn.x, btn.y, btn.w, btn.h, img);
        M5.Display.setSwapBytes(oldSwap);
        return;
    }

    // 其他普通按钮
    M5.Display.fillRoundRect(btn.x, btn.y, btn.w, btn.h, 5, BLACK);
    M5.Display.drawRoundRect(btn.x, btn.y, btn.w, btn.h, 5, COLOR_BORDER);

    applyBodyFont();
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString(btn.label, btn.x + btn.w / 2, btn.y + btn.h / 2);
    M5.Display.setTextDatum(top_left);
}
void UIManager::setScreen(ScreenType screen) {
    if (currentScreen != screen) {
        currentScreen = screen;
        selectedFoodId = -1;
        needRefresh = true;
        update();
    }
}

void UIManager::setSelectedFood(int id) {
    if (selectedFoodId != id) {
        selectedFoodId = id;
        needRefresh = true;
        update();
    }
}

FoodItem UIManager::getSelectedFoodItem() {
    if (selectedFoodId != -1) {
        return foodManager->getFood(selectedFoodId);
    }
    return FoodItem();
}

void UIManager::onFoodEvent(const FoodEvent& event) {
    switch (event.type) {
        case FoodEventType::DATA_CHANGED:
            loadFoodList();
            needRefresh = true;
            update();
            break;
            
        case FoodEventType::MESSAGE:
            showMessage(event.message);
            break;
            
        case FoodEventType::FOOD_REMOVED:
            showMessage(event.message);
            break;
            
        default:
            break;
    }
}

int UIManager::getButtons(Button* btnArray, int maxCount) {
    int count = buttonCount < maxCount ? buttonCount : maxCount;
    for (int i = 0; i < count; i++) {
        btnArray[i] = buttons[i];
    }
    return count;
}

int UIManager::getListItemIndex(int x, int y) {
    if (x < 5 || x > 315) return -1;
    if (y < 68 || y > 213) return -1;
    
    int index = (y - 78) / 28;
    if (index >= 0 && index < (int)currentFoods.size()) {
        return index;
    }
    return -1;
}

int UIManager::getButtonAtPosition(int x, int y) {
    for (int i = 0; i < buttonCount; i++) {
        if (x >= buttons[i].x && x <= buttons[i].x + buttons[i].w &&
            y >= buttons[i].y && y <= buttons[i].y + buttons[i].h) {
            return buttons[i].actionId;
        }
    }
    return -1;
}

void UIManager::showMessage(const String& msg) {
    M5.Display.fillRoundRect(40, 100, 240, 40, 10, BLACK);
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(50, 118);
    M5.Display.println(msg);
    delay(1500);
    needRefresh = true;
    update();
}

void UIManager::setCloudSyncCallback(void (*callback)(int action)) {
    cloudCallback = callback;
}