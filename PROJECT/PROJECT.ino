//                                      ______________
//                                 CLK |    | USB |   |  5V
//                                  D0 |    | port|   |  CMD
//                                  D1 |    |_____|   |  D3
//                                  15 |      ALL     |  D2
//                 LED CHECK ATTEND  2 |     GPIO     |  13
//                                   0 |     3.3v     |  GND
//                                   4 |   ________   |  12
//           FINGERPRINT SENSOR RX  16 |  |        |  |  14
//           FINGERPRINT SENSOR RX  17 |  |        |  |  27
//                                   5 |  |        |  |  26
//                                  18 |  |        |  |  25
//                                  19 |  |        |  |  33
//                                 GND |  |  ESP32 |  |  32
//          LCD16x2  RtcDS1307 SDA  21 |  |        |  |  35
//                                  RX |  |        |  |  34
//                                  TX |  |        |  |  VN
//          LCD16x2  RtcDS1307 SCL  22 |  |        |  |  VP
//                         touchSen 23 |  |________|  |  EN
//                                 GND |______________|  2V3

// FINGERPRINT SENSOR USE 3V3 !!!!
// DS1307 use 5v/3v3
// LCD I2C use 5V
// GPIO 34 35 36 39 input only
// GPIO 2 led onboar
// GPIO 13 -> 33 normal
// Định nghĩa các thông số kết nối và thông tin của Blynk
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLwj58xTkX"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "k7uGKLnJFAMutYPjhWrSOC1hKGhz-hnm"

#define touchSen 23

#define countof(x) (sizeof(x) / sizeof((x)[0]))

// Thư viện và module được sử dụng
#include <Arduino.h>
#include <BlynkMultiClient.h>
#include <Adafruit_Fingerprint.h>
#include <RtcDS1307.h>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#include <FirebaseESP8266.h>
#endif
#include <WiFiClientSecure.h>
#include <Wire.h>  // must be included here so that Arduino library object file references work
#include <LiquidCrystal_I2C.h>
#include <ESP.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//realTime
RtcDS1307<TwoWire> Rtc(Wire);

//Khai báo Serial ảo
#define MODEM_RX 16
#define MODEM_TX 17
#define mySerial Serial2  // use for ESP32

//FINGER
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Thông tin kết nối WiFi
const char* ssid = "Chalsprovjp";
const char* pass = "khongchopass";

// Thông tin kết nối đến Google Script
const char* host = "script.google.com";
// const uint16_t httpsPort = 80;

//FIREBASE INFORMATION
#define HOST "iot23-aa61b-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define AUTH "Wu8xEJYji84Lvt3xqVzSkFzJTN2lJY7OCxgQ0GXO"
FirebaseData firebaseData;
String path = "/";

// ID của script Google Apps, dùng để gửi dữ liệu
String GAS_ID = "AKfycbyxnSvpfceB8ft3BZlOFuYi1MuHLFvViCk_qyAzm-vm1XvJCPR76saL8LeyOE7kIGLrIw";

// Khai báo đối tượng WiFiClient để sử dụng trong Blynk
static WiFiClient blynkWiFiClient;

// Biến lưu trữ giá trị
int btnAddFinger = 0, btnCheckAttend = 1, btnDeleteAll = 0, id = 0;
String terminalRead = "";
String timeReal;
bool ok1 = false, ok2 = false, ok3 = false, ok4 = false, ok5 = false;
bool in = true;
String ids[50];
String names[50];
String times[50];
String tFingerPrint[50];
uint8_t fingerTemplate[512];
int count = 0;
int idtmp = 1;
String templateFingerData = "";
uint8_t fingerTemp[50];
//Biến đếm thời gian delay
unsigned long t1 = 0;

// Khai báo đối tượng trên Blynk để hiển thị dữ liệu
WidgetLCD lcdB(V6);
WidgetTerminal terminal(V7);
WidgetLED led(V0);

// Khai báo đối tượng BlynkTimer để định thời gian gửi dữ liệu
BlynkTimer timer;



unsigned long previousMillis = 0;     // Biến lưu trữ thời điểm trước đó
const unsigned long interval = 5000;  // Khoảng thời gian mong muốn (1 giây)



// ---------------------------------------------------------------------------------------------------------

// Hàm gửi dữ liệu đến Google Script
void sendDataToGGSheet() {

  // Đối tượng WiFiClientSecure để thực hiện kết nối an toàn
  WiFiClientSecure client;
  const int httpPort = 443;  // 80 is for HTTP / 443 is for HTTPS!

  client.setInsecure();
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  delay(1000);
  // Kết nối đến host Google Script
  if (!client.connect(host, httpPort)) {

    return;
  }

  // Chuẩn bị dữ liệu và URL
  String string_temperature = "";
  String string_humidity = "";
  String url = "/macros/s/AKfycbyxnSvpfceB8ft3BZlOFuYi1MuHLFvViCk_qyAzm-vm1XvJCPR76saL8LeyOE7kIGLrIw/exec";
  Serial.print("requesting URL: ");
  Serial.println(url);

  // Gửi yêu cầu GET
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  Serial.println("Request sent");

  Serial.println("Request complete");
}



// Hàm lập lịch tự chạy
void myTimerEvent() {
  if (btnAddFinger == 1 && !ok1) addFinger();
  else if (btnDeleteAll == 1 && !ok3) deleteAll();
  else if (btnCheckAttend == 1 && !ok5 && digitalRead(touchSen)) checkAttend();
  else {
    printLCDDefault();
  }
}


//------------------------------------------------ARDUINO FUNCTION-------------------------------------------------
//ARDUINO FUNCTION
void setup() {

  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  printLCD("WAITING STARTUP", ".........");
  // Khởi tạo kết nối Serial để ghi log
  Serial.begin(115200);
  pinMode(touchSen, INPUT);
  //REALTIME

  // Kết nối đến mạng WiFi
  connectWiFi();
  printLCD("SETTING WIFI", "DONE");
  //Connect Blynk
  printLCD("SETTING BLYNK", "WAITING");
  setBlynk();
  printLCD("SETTING BLYNK", "DONE");
  printLCD("RTC SETTING", "WAITING");
  rtcSetup();
  printLCD("RTC SETTING", "DONE");
  printLCD("FIREBASE SETTING", "WAITING");
  firebaseConnect();
  printLCD("FIREBASE SETTING", "DONE");
  printLCD("FINGER SETTING", "WAITING");
  fingerSet();
  printLCD("FINGER SETTING", "DONE");
  printLCD("SYNC DATA FINGER", "WAITING");
  finger.emptyDatabase();
  syncFingerDataToSensor();
  printLCD("SYNC DATA FINGER", "DONE");
  printLCD("STARTUP DONE", ".........");
  // Thiết lập chu kỳ thời gian cho sự kiện timer < cứ sau xL milis thì hàm được gọi một lần 
  timer.setInterval(1L, myTimerEvent);
  timer.setInterval(1000L, updateTime);

}

void loop() {
  // Kiểm tra kết nối WiFi, nếu mất kết nối thì kết nối lại
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    return;
  }

  // Chạy các hoạt động của Blynk
  Blynk.run();

  // Chạy timer của Blynk
  timer.run();
}
//--------------------------------------------------END ARDUINO FUNCTION-----------------------------------------------

//-----------------------------------------MAIN function--------------------------------------------------------
//MAIN function

void syncFingerDataToSensor() {
  getFullDataFromFirebase();
  for (int i = 0; i < count; i++) {
    lcd.setCursor(10,1);
    lcd.print(String(i)+"/"+String(count));
    processingTemplateFinger(ids[i].substring(2).toInt(), tFingerPrint[i]);
    delay(1000);
  }
  memset(ids, 0, sizeof(ids) / sizeof(ids[0]));
  memset(names, 0, sizeof(names) / sizeof(names[0]));
  memset(times, 0, sizeof(times) / sizeof(times[0]));
  memset(tFingerPrint, 0, sizeof(tFingerPrint) / sizeof(tFingerPrint[0]));
}

void addFinger() {
  while (getFingerprintEnroll() > 0 && !ok1)
    ;
  Blynk.virtualWrite(V3, 0);
}

void deleteAll() {
  if (in) {
    println("\n========================================");
    println("Deleting all fingerprint templates!");
    println("Input Y to delete all. If you input any key different Y, he action of deleting all fingerprints will be cancelled");
    in = false;
    terminalRead = "0";
  }

  String check = terminalRead;
  delay(500);
  while (check == "0") {
    Blynk.run();
    check = terminalRead;
  }
  if (check == "Y") {
    finger.emptyDatabase();
    println("Now database is empty :)");


    getDataDeleteFromFirebase("fingerData");
    Serial.println(count);
    for (int i = 0; i < count; i++) {
      FirebaseJson json;
      json.add("time", times[i]);
      json.add("name", names[i]);
      json.add("id", ids[i]);
      json.add("templateFinger", tFingerPrint[i]);

      Firebase.pushJSON(firebaseData, path + "/fingerDelete", json);
    }

    // Firebase.deleteNode(firebaseData, path + "/fingerData");
    ok3 = true;

    in = true;
  } else {
    println("Do nothing :)");
    terminal.println("Do nothing  :)");
    ok3 = true;

    in = true;
  }

  return;
}

void checkAttend() {
  printLCD("CHECK ATTENDANCE", "");
  id = getFingerprintID();

  if (id > 0 && !ok5) {
    getFullDataFromFirebase();
    // int sizeData = sizeof(ids) / sizeof(ids[0]);

    for (int i = 0; i < count; i++) {
      Serial.println(ids[i].substring(2));
      if (id == ids[i].substring(2).toInt()) {
        println("WELCOME " + names[i]);

        String strtime = timeReal;
        printLCD("WELCOME " + nameSeparation(names[i]), strtime);
        saveToFirebaseAt(strtime, names[i], id);
        println("You have checked attendance successfully");
        delay(1500);
        return;
      }
    }
  } else {
    println("Your fingerprint is not registered on the database");
    printLCD("ERORR!!!!", "CHECK AGAIN!");
  }
  id = 0;
  delay(500);
}

//-----------------------------------------------END MAIN function--------------------------------------------------

