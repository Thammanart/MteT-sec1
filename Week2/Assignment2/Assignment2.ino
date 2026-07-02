#include <WiFi.h>
#include <WebServer.h>

// ==========================================
// 1. ตั้งค่า WiFi
// ==========================================
const char* ssid = "Tsujikori";         // เปลี่ยนเป็นชื่อ WiFi ของคุณ
const char* password = "Nicky2547";   // เปลี่ยนเป็นรหัสผ่าน WiFi ของคุณ

// 2. กำหนดให้ใช้ Pin 2 (Built-in LED บนบอร์ด ESP32 ส่วนใหญ่)
//    หมายเหตุ: ห้ามใช้ Pin 0, 1, 3 เพราะเป็นขา UART/Boot ของบอร์ด
const int outputPin = 2;
String pinState = "OFF";

// สร้าง Web Server ที่ Port 80
WebServer server(80);

// ==========================================
// ส่วนของ HTML Code สำหรับสร้างหน้า Webpage
// ==========================================
String getHTML() {
  String html = "<!DOCTYPE html><html>\n";
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  html += "<title>ESP32 LED Control</title>\n";

  // ตกแต่งปุ่มด้วย CSS
  html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  html += ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; \n";
  html += "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; border-radius: 10px;}\n";
  html += ".button-off {background-color: #f44336;}</style></head>\n";

  // ส่วนแสดงผลข้อความและปุ่ม
  html += "<body><h1>ESP32 Built-in LED (Pin 2)</h1>\n";
  html += "<p>Status: <strong>" + pinState + "</strong></p>\n";

  // สร้างปุ่มที่ลิงก์ไป URL /on หรือ /off ตามสถานะปัจจุบัน
  if (pinState == "OFF") {
    html += "<p><a href=\"/on\"><button class=\"button\">Turn ON</button></a></p>\n";
  } else {
    html += "<p><a href=\"/off\"><button class=\"button button-off\">Turn OFF</button></a></p>\n";
  }

  html += "</body></html>\n";
  return html;
}

// ==========================================
// ส่วนควบคุมการทำงานเมื่อมีการกดปุ่ม
// ==========================================
void handleRoot() {
  server.send(200, "text/html", getHTML());
}

void handleOn() {
  digitalWrite(outputPin, HIGH); // สั่งเปิด LED
  pinState = "ON";
  Serial.println("Web Command: LED turned ON"); // แสดงสถานะใน Serial Monitor
  server.send(200, "text/html", getHTML());
}

void handleOff() {
  digitalWrite(outputPin, LOW); // สั่งปิด LED
  pinState = "OFF";
  Serial.println("Web Command: LED turned OFF"); // แสดงสถานะใน Serial Monitor
  server.send(200, "text/html", getHTML());
}

// ==========================================
// ฟังก์ชัน Setup (ทำงานครั้งแรกครั้งเดียว)
// ==========================================
void setup() {
  // เปิดใช้งาน Serial Monitor
  Serial.begin(115200);

  // เพิ่ม delay เพื่อให้ Serial Monitor เปิดรับข้อมูลทัน ป้องกันอาการค้างที่ Boot
  delay(1000);
  Serial.println("\n\n--- ESP32 is Starting! ---");

  // ตั้งค่า Pin ให้เป็น OUTPUT
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW);

  // เริ่มต้นการเชื่อมต่อ WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // รอจนกว่าจะเชื่อมต่อสำเร็จ (ใส่ timeout กันค้างตลอดไป)
  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 15000; // รอสูงสุด 15 วินาที

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttemptTime < wifiTimeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    // แสดง IP Address บน Serial Monitor เมื่อเชื่อมต่อสำเร็จ
    Serial.println("\nWiFi connected successfully!");
    Serial.print(">>> Your ESP32 IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("---------------------------------");

    // กำหนด Routing (URL) ให้กับ Web Server
    server.on("/", handleRoot);
    server.on("/on", handleOn);
    server.on("/off", handleOff);

    server.begin();
    Serial.println("HTTP server started. Waiting for client...");
  } else {
    // ต่อ WiFi ไม่สำเร็จภายในเวลาที่กำหนด
    Serial.println("\n!!! WiFi connection FAILED !!!");
    Serial.println("โปรดตรวจสอบชื่อ WiFi (SSID) และรหัสผ่านอีกครั้ง");
    Serial.print("Status code: ");
    Serial.println(WiFi.status());
  }
}

// ==========================================
// ฟังก์ชัน Loop (ทำงานวนซ้ำไปเรื่อยๆ)
// ==========================================
void loop() {
  server.handleClient(); // รอรับและจัดการคำสั่งจาก Web Browser
}
