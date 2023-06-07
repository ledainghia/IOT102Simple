//------------------------------------------------Blynk function-------------------------------------------------
//Blynk function
// BLYNK_WRITE(Vpin) là hàm ngắt. Khi có data từ Blynk gửi về Vpin thì toàn bộ chương trình tạm dừng để nhận dữ liệu của Blynk

BLYNK_WRITE(V3) {
  btnAddFinger = param.asInt();
  Serial.println("btnAddFinger " + String(btnAddFinger));
  ok1 = false;
}

BLYNK_WRITE(V5) {
  btnDeleteAll = param.asInt();
  Serial.println("btnDeleteAll " + String(btnDeleteAll));
  ok3 = false;
}

BLYNK_WRITE(V7) {
  terminalRead = param.asStr();
  Serial.println(terminalRead);
}

BLYNK_WRITE(V8) {
  btnCheckAttend = param.asInt();
  Serial.println("btnCheckAttend " + String(btnCheckAttend));
}
//------------------------------------------END Blynk function-------------------------------------------------------
