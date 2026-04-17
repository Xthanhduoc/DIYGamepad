# 🎮 BN GAMEPAD V1.0

![Version](https://img.shields.io/badge/Version-1.0-blue.svg)
![Status](https://img.shields.io/badge/Status-In_Development-orange.svg)
![Hardware](https://img.shields.io/badge/Hardware-ESP32-green.svg)

Dự án thiết kế và chế tạo tay cầm chơi game không dây (Bluetooth HID) tùy biến, được xây dựng trên nền tảng vi điều khiển ESP32. "BN GAMEPAD" không chỉ là một thiết bị điều khiển, mà còn là một nền tảng phần cứng mở tích hợp màn hình OLED, cảm biến gia tốc và hệ thống phản hồi lực.

---

## ✨ Tính năng nổi bật (Features)
* **Kết nối không dây:** Hỗ trợ Bluetooth/BLE mô phỏng thiết bị HID (Human Interface Device) có độ trễ cực thấp.
* **Cảm biến chuyển động (Motion Control):** Tích hợp IMU 6 trục (MPU-6050) để đo gia tốc và góc nghiêng.
* **Hiển thị trực quan:** Màn hình OLED I2C theo dõi trạng thái pin, thông số góc nghiêng và menu kết nối.
* **Phản hồi lực (Haptic Feedback):** Động cơ rung tích hợp với mạch điều khiển bảo vệ chống nhiễu (Flyback Diode).
* **Quản lý năng lượng:** Mạch sạc pin LiPo tích hợp (TP4056) qua cổng USB Type-C hiện đại.
* **Tương thích cao:** Nút bấm thiết kế chuẩn Active-Low với điện trở Pull-up 10k, chống nhiễu tuyệt đối.

---

## 🛠 Cấu hình phần cứng (Hardware Specifications)
* **Vi điều khiển (MCU):** ESP32-WROOM-32E (Tích hợp ăng-ten PCB)
* **Nguồn cấp:** Cổng USB Type-C, Pin LiPo 3.7V, IC hạ áp LDO 3.3V
* **Màn hình:** OLED 1.3" I2C
* **Cảm biến:** MPU-6050 (I2C)
* **Cụm điều khiển:** * 2x Thumb Joystick Analog (L3/R3)
  * D-Pad & Action Buttons (Digital)
  * L1/L2 & R1/R2 Triggers

---

## 🗺 Lộ trình phát triển (Roadmap)

Dự án được chia thành các giai đoạn rõ ràng từ lúc hình thành ý tưởng đến khi ra thành phẩm cuối cùng:

### Giai đoạn 1: Ý tưởng & Thiết kế nguyên lý (Schematic) - `[HOÀN THÀNH]`
- [x] Lựa chọn vi điều khiển (ESP32) và phác thảo sơ đồ khối.
- [x] Thiết kế mạch nguồn, sạc pin (TP4056) và bảo vệ an toàn.
- [x] Giao tiếp ngoại vi I2C cho màn hình OLED và MPU-6050.
- [x] Tính toán giá trị linh kiện chống nhiễu (Pull-up 10k, Tụ lọc 100nF, Schottky Diode SS26).

### Giai đoạn 2: Bố trí linh kiện & Đi dây (PCB Layout) - `[HOÀN THÀNH]`
- [x] Chuẩn hóa kích thước mạch in (Board Shape).
- [x] Sắp xếp linh kiện 2 mặt (Top Layer cho tương tác, Bottom Layer cho xử lý).
- [x] Tối ưu hóa vùng cấm phủ đồng (Keep-out) cho ăng-ten ESP32 để đảm bảo sóng Bluetooth.
- [x] Phủ Mass (Polygon Pour) và thêm nhãn Silkscreen thương hiệu "BAC NAM".
- [x] Xuất file Gerber và đặt gia công mạch.

### Giai đoạn 3: Phát triển phần mềm (Firmware) - `[ĐANG TIẾN HÀNH]`
- [ ] **Phase 3.1:** Test ngoại vi cơ bản (Đọc ADC từ Joystick, đọc Digital từ nút nhấn, nhấp nháy LED, bật motor rung).
- [ ] **Phase 3.2:** Khởi tạo I2C, hiển thị logo và thông số lên OLED, đọc dữ liệu góc nghiêng từ MPU-6050.
- [ ] **Phase 3.3:** Viết cấu hình Bluetooth BLE Gamepad HID, map dữ liệu phần cứng thành nút bấm ảo.
- [ ] **Phase 3.4:** Tối ưu hóa (Lọc nhiễu Joystick, thêm chức năng Deep Sleep tiết kiệm pin).

### Giai đoạn 4: Gia công & Lắp ráp (Assembly) - `[SẮP TỚI]`
- [ ] Hàn linh kiện dán (SMD) và linh kiện xuyên lỗ (THT) lên PCB thực tế.
- [ ] Cắm các module (OLED, MPU-6050, Joystick) và test toàn hệ thống.

### Giai đoạn 5: Thiết kế vỏ hộp (Enclosure) - `[SẮP TỚI]`
- [ ] Đo đạc kích thước thực tế sau khi lắp ráp.
- [ ] Dựng file 3D vỏ tay cầm (SolidWorks/Fusion360) đảm bảo công thái học (Ergonomics).
- [ ] In 3D và hoàn thiện sản phẩm.

---

## 📂 Cấu trúc thư mục dự án
```text
BN-Gamepad-V1/
├── Hardware/           # Chứa file Altium (Schematic, PCB, Gerber)
├── Firmware/           # Chứa code Arduino/ESP-IDF cho ESP32
├── 3D_Models/          # Chứa file STL/STEP vỏ hộp in 3D (Đang cập nhật)
├── Docs/               # Sơ đồ chân (Pinout), Datasheet linh kiện
└── README.md
