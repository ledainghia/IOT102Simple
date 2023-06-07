// ------------------------------------------------SETUP---------------------------------------------------------
// SETUP
void setBlynk() {
  // Thêm kết nối Blynk Client với tên "WiFi", sử dụng WiFiClient blynkWiFiClient và cổng 80
  Blynk.addClient("WiFi", blynkWiFiClient, 80);

  // Cấu hình thông tin xác thực Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);
}
void fingerSet() {
  //Khởi chạy finger
  finger.begin(57600);

  delay(50);
  //check finger sensor
  if (finger.verifyPassword()) {
    println("Found fingerprint sensor!");
  } else {
    println("Did not find fingerprint sensor :(");
    println("Check sensor then reset :(");
  }

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    terminal.println();
  } else {
    println("Waiting for valid finger...");
    print("Sensor contains ");
    print(String(finger.templateCount));
    println(" templates");
  }
}
// Hàm kết nối WiFi
void connectWiFi() {
  print("Connecting to ");
  println(ssid);

  // Kết nối WiFi với SSID và Password
  if (pass && strlen(pass)) {
    WiFi.begin((char*)ssid, (char*)pass);
  } else {
    WiFi.begin((char*)ssid);
  }

  // Chờ cho đến khi kết nối thành công
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    print(".");
  }
}
void firebaseConnect() {
  //connect firebase
  Firebase.begin(HOST, AUTH);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path)) {
    println("REASON: " + firebaseData.errorReason());
    println();
  }
}
// -------------------------------------------------END SETUP--------------------------------------------------------
