#include "BluetoothSerial.h"

// ตรวจสอบว่าบอร์ด ESP32 ที่เลือกเปิดใช้งานฟังก์ชัน Bluetooth หรือยัง
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// สร้างออบเจกต์สำหรับใช้งาน Bluetooth
BluetoothSerial SerialBT;

void setup() {
  // เปิดใช้งาน Serial Monitor ที่ความเร็ว 115200
  Serial.begin(115200);
  
  // ตั้งชื่อ Bluetooth ของบอร์ด ESP32 (เปลี่ยนชื่อได้ตามต้องการ)
  SerialBT.begin("ESP32_MyBluetooth"); 
  
  Serial.println("=========================================");
  Serial.println("Bluetooth เปิดทำงานแล้ว!");
  Serial.println("กรุณาจับคู่และเชื่อมต่อบลูทูธชื่อ: ESP32_MyBluetooth");
  Serial.println("=========================================");
}

void loop() {
  // 1. รับข้อมูลจากคอมพิวเตอร์ (Serial Monitor) -> ส่งไปที่มือถือ (Bluetooth)
  if (Serial.available()) {
    char dataFromPC = Serial.read();
    SerialBT.write(dataFromPC);
  }
  
  // 2. รับข้อมูลจากมือถือ (Bluetooth) -> ส่งมาแสดงบนคอมพิวเตอร์ (Serial Monitor)
  if (SerialBT.available()) {
    char dataFromPhone = SerialBT.read();
    Serial.write(dataFromPhone);
  }
  
  delay(20); // หน่วงเวลาเล็กน้อยเพื่อความเสถียร
}