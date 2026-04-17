#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// --- ĐỊNH NGHĨA CHÂN NGOẠI VI ---
#define Up 16
#define Right 25
#define Down 5
#define Left 4
#define JoyRbtn 27       // Nút CHỌN
#define JoyLbtn 23       // Nút QUAY LẠI
#define READ_POWER 32    
#define JOY_L_X 34       
#define JOY_L_Y 35       
#define JOY_R_X 36       
#define JOY_R_Y 39       

#define I2C_SDA 21
#define I2C_SCL 22

// --- CẤU HÌNH MÀN HÌNH 0.96 INCH (SSD1306) ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define i2c_Address 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- CẤU HÌNH BLE ---
#define BLE_SERVER_NAME "BN_GAMEPAD_V1"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Callbacks để biết có thiết bị kết nối hay không
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

// --- BIẾN TRẠNG THÁI HỆ THỐNG ---
enum UI_Mode { MODE_BOOTING, MODE_MAIN_MENU, MODE_CONNECT_MENU, MODE_TESTER, MODE_BLE_ACTIVE };
UI_Mode currentMode = MODE_BOOTING;

bool isBluetoothConnected = false; 
unsigned long bootStartTime = 0; 
unsigned long lastBtnPress = 0;  
unsigned long lastBleSend = 0;

int mainMenuIndex = 0;
const int NUM_MAIN_ITEMS = 2;
String mainItems[NUM_MAIN_ITEMS] = {"1. Connect", "2. Gamepad Tester"};

int connectMenuIndex = 0;
const int NUM_CONNECT_ITEMS = 2;
String connectItems[NUM_CONNECT_ITEMS] = {"1. Bluetooth", "2. BLE"};

void setup() {
  Serial.begin(9600);
  
  pinMode(Up, INPUT_PULLUP);
  pinMode(Right, INPUT_PULLUP);
  pinMode(Down, INPUT_PULLUP);
  pinMode(Left, INPUT_PULLUP);
  pinMode(JoyRbtn, INPUT_PULLUP);
  pinMode(JoyLbtn, INPUT_PULLUP); 
  
  Wire.begin(I2C_SDA, I2C_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, i2c_Address)) {
    Serial.println("OLED init failed!");
    for(;;);
  }
  
  display.clearDisplay();
  display.display();
  bootStartTime = millis();

  // Khởi tạo BLE nhưng chưa phát quảng cáo
  BLEDevice::init(BLE_SERVER_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
}

// ================= CÁC HÀM VẼ GIAO DIỆN =================
void drawStatusBar() {
  display.drawLine(0, 12, 128, 12, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 2);
  
  isBluetoothConnected = deviceConnected; // Cập nhật icon theo trạng thái BLE thực tế
  
  if (isBluetoothConnected) {
    display.print("BT: ON"); 
  } else {
    if ((millis() / 500) % 2 == 0) display.print("BT: wait");
  }

  int rawADC = analogRead(READ_POWER); 
  int battFill = map(rawADC, 2000, 2800, 0, 10); 
  battFill = constrain(battFill, 0, 10); 
  display.drawRect(110, 2, 14, 8, WHITE); 
  display.drawRect(124, 4, 2, 4, WHITE); 
  display.fillRect(112, 4, battFill, 4, WHITE);
}

void drawBootingScreen() {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20, 20);
  display.print("BAC NAM");
  display.setTextSize(1);
  display.setCursor(10, 45);
  int dots = (millis() / 500) % 4;
  String loading = "Searching";
  for(int i=0; i<dots; i++) loading += ".";
  display.print(loading);
}

void drawMenu(String title, String items[], int numItems, int currentIndex) {
  display.setCursor(35, 16);
  display.print(title);
  int startY = 30;
  for (int i = 0; i < numItems; i++) {
    if (i == currentIndex) {
      display.fillRect(10, startY + (i * 12) - 1, 108, 11, WHITE);
      display.setTextColor(BLACK, WHITE);
    } else {
      display.setTextColor(WHITE);
    }
    display.setCursor(14, startY + (i * 12) + 1);
    display.print(items[i]);
  }
  display.setTextColor(WHITE);
}

void drawGamepadTester() {
  int r = 14; 
  int leftCenter_X = 22, center_Y = 40;
  int rightCenter_X = 106;
  display.drawCircle(leftCenter_X, center_Y, r, WHITE);
  display.drawCircle(rightCenter_X, center_Y, r, WHITE);

  int mapLX = map(analogRead(JOY_L_X), 0, 4095, -10, 10);
  int mapLY = map(analogRead(JOY_L_Y), 0, 4095, -10, 10);
  int mapRX = map(analogRead(JOY_R_X), 0, 4095, -10, 10);
  int mapRY = map(analogRead(JOY_R_Y), 0, 4095, -10, 10);

  display.fillCircle(leftCenter_X + mapLX, center_Y + mapLY, 3, WHITE);
  display.fillCircle(rightCenter_X + mapRX, center_Y + mapRY, 3, WHITE);

  int midX = 64, midY = 40;
  if(digitalRead(Up) == LOW) display.fillRect(midX - 3, midY - 12, 6, 6, WHITE);
  else display.drawRect(midX - 3, midY - 12, 6, 6, WHITE);

  if(digitalRead(Down) == LOW) display.fillRect(midX - 3, midY + 6, 6, 6, WHITE);
  else display.drawRect(midX - 3, midY + 6, 6, 6, WHITE);

  if(digitalRead(Left) == LOW) display.fillRect(midX - 12, midY - 3, 6, 6, WHITE);
  else display.drawRect(midX - 12, midY - 3, 6, 6, WHITE);

  if(digitalRead(Right) == LOW) display.fillRect(midX + 6, midY - 3, 6, 6, WHITE);
  else display.drawRect(midX + 6, midY - 3, 6, 6, WHITE);
  
  display.setCursor(45, 18);
  display.print("TESTER");
}

void drawBleActiveScreen(String payload) {
  display.setCursor(25, 20);
  display.print("- BLE MODE -");
  display.setCursor(5, 35);
  if(deviceConnected) {
    display.print("Status: Connected");
    display.setCursor(5, 50);
    display.print(payload); // Hiển thị chuỗi đang gửi đi lên màn hình
  } else {
    display.print("Status: Advertising");
    display.setCursor(5, 50);
    display.print("Waiting for client...");
  }
}

// ================= VÒNG LẶP CHÍNH =================
void loop() {
  display.clearDisplay();

  bool btnUp     = (digitalRead(Up) == LOW);
  bool btnDown   = (digitalRead(Down) == LOW);
  bool btnLeft   = (digitalRead(Left) == LOW);
  bool btnRight  = (digitalRead(Right) == LOW);
  bool btnSelect = (digitalRead(JoyRbtn) == LOW);
  bool btnBack   = (digitalRead(JoyLbtn) == LOW);

  // --- MÁY TRẠNG THÁI (STATE MACHINE) ---
  if (currentMode == MODE_BOOTING) {
    drawBootingScreen();
    if (millis() - bootStartTime > 3500) currentMode = MODE_MAIN_MENU;
  } 
  else {
    drawStatusBar();

    if (currentMode == MODE_MAIN_MENU) {
      drawMenu("- MAIN MENU -", mainItems, NUM_MAIN_ITEMS, mainMenuIndex);
      if (btnDown && millis() - lastBtnPress > 200) {
        mainMenuIndex = (mainMenuIndex + 1) % NUM_MAIN_ITEMS;
        lastBtnPress = millis();
      }
      if (btnUp && millis() - lastBtnPress > 200) {
        mainMenuIndex = (mainMenuIndex - 1 + NUM_MAIN_ITEMS) % NUM_MAIN_ITEMS;
        lastBtnPress = millis();
      }
      if (btnSelect && millis() - lastBtnPress > 300) {
        if (mainMenuIndex == 0) currentMode = MODE_CONNECT_MENU;
        else if (mainMenuIndex == 1) currentMode = MODE_TESTER;
        lastBtnPress = millis();
      }
    }
    
    else if (currentMode == MODE_CONNECT_MENU) {
      drawMenu("- CONNECT -", connectItems, NUM_CONNECT_ITEMS, connectMenuIndex);
      if (btnDown && millis() - lastBtnPress > 200) {
        connectMenuIndex = (connectMenuIndex + 1) % NUM_CONNECT_ITEMS;
        lastBtnPress = millis();
      }
      if (btnUp && millis() - lastBtnPress > 200) {
        connectMenuIndex = (connectMenuIndex - 1 + NUM_CONNECT_ITEMS) % NUM_CONNECT_ITEMS;
        lastBtnPress = millis();
      }
      
      if (btnSelect && millis() - lastBtnPress > 300) {
        if (connectMenuIndex == 1) { // Chọn "2. BLE"
          BLEDevice::startAdvertising(); // Bắt đầu phát sóng BLE
          currentMode = MODE_BLE_ACTIVE;
        }
        lastBtnPress = millis();
      }
      
      if (btnBack && millis() - lastBtnPress > 300) {
        currentMode = MODE_MAIN_MENU;
        lastBtnPress = millis();
      }
    }
    
    else if (currentMode == MODE_TESTER) {
      drawGamepadTester();
      if (btnBack && millis() - lastBtnPress > 300) {
        currentMode = MODE_MAIN_MENU;
        lastBtnPress = millis();
      }
    }

    else if (currentMode == MODE_BLE_ACTIVE) {
      // Đóng gói trạng thái nút bấm thành chuỗi: ví dụ "U:1 D:0 L:0 R:0" (1 là đang bấm)
      String payload = "U:" + String(btnUp) + 
                      " D:" + String(btnDown) + 
                      " L:" + String(btnLeft) + 
                      " R:" + String(btnRight);
                      
      drawBleActiveScreen(payload);

      // Gửi dữ liệu qua BLE mỗi 50ms nếu có thiết bị kết nối
      if (deviceConnected && (millis() - lastBleSend > 50)) {
        pCharacteristic->setValue(payload.c_str());
        pCharacteristic->notify();
        lastBleSend = millis();
      }

      // Xử lý khi ngắt kết nối (tự động phát lại sóng để chờ kết nối lại)
      if (!deviceConnected && oldDeviceConnected) {
        delay(500); 
        pServer->startAdvertising(); 
        oldDeviceConnected = deviceConnected;
      }
      if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
      }

      // Thoát khỏi chế độ BLE
      if (btnBack && millis() - lastBtnPress > 300) {
        // Tắt phát sóng nếu muốn
        // BLEDevice::getAdvertising()->stop();
        currentMode = MODE_CONNECT_MENU;
        lastBtnPress = millis();
      }
    }
  }

  display.display();
}