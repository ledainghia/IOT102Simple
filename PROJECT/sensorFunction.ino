//Các hàm xử lý, đọc và ghi khi giao tiếp với cảm biến vân tay
//=======================================================================================
void write_template_data_to_sensor(int idt) {
  int template_buf_size = 512;  //usually hobby grade sensors have 512 byte template data, watch datasheet to know the info

  uint8_t tFingerTemplate[512];  //this is where you need to store your template data

  Serial.println("Ready to write template to sensor...");

  Serial.print("Writing template against ID #");
  Serial.println(idt);
  for (int j = 0; j < 512; j++) {
    tFingerTemplate[j] = fingerTemplate[j];
  }
  for (int j = 0; j < 512; j++) {
    Serial.print(tFingerTemplate[j]);
    Serial.print(":");
  }
  Serial.println("");
  delay(3000);
  if (finger.write_template_to_sensor(template_buf_size, tFingerTemplate)) {  //telling the sensor to download the template data to it's char buffer from upper computer (this microcontroller's "fingerTemplate" buffer)
    Serial.println("now writing to sensor...");
  } else {
    Serial.println("writing to sensor failed");
    return;
  }

  Serial.print("ID ");
  Serial.println(idt);
  if (finger.storeModel(idt) == FINGERPRINT_OK) {  //saving the template against the ID you entered or manually set
    Serial.print("Successfully stored against ID#");
    Serial.println(idt);
  } else {
    Serial.println("Storing error");
    return;
  }
}


// Lấy id của vân tay đã được lưu trong cảm biến
int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

// Thêm vân tay mới
uint8_t getFingerprintEnroll() {
  printLCD("ADD NEW", "FINGER READY !");
  getFullDataFromFirebase();
  if (in) {
    println("==============================");
    println("Ready to enroll a fingerprint!");
    print("Your ID: ");
    id = idtmp;
    println(String(id));
    printLCD("YOUR ID: ID" + String(id), "");
    delay(1500);
    println("Please put your fingerprint on the sensor!!!");
    printLCD("PLS PUT FINGER", "ON THE SENSOR!!!");
    in = false;
  }
  if (id != 0) in = true;
  int p = -1;

  print("Waiting for valid finger to enroll as #");
  println(String(id));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        println("Image taken");
        break;
      default:
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      println("Could not find fingerprint features");
      return p;
    default:
      println("Unknown error");
      return p;
  }
  printLCD("PLEASE REMOVE ", "FINGERS");
  println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  print("ID ");
  println(String(id));
  p = -1;
  printLCD("PLACE SAME", "FINGER AGAIN");
  println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        println("Imaging error");
        break;
      default:
        println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      println("Could not find fingerprint features");
      return p;
    default:
      println("Unknown error");
      return p;
  }

  // OK converted!
  print("Creating model for #");
  println(String(id));

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    println("Fingerprints did not match");
    return p;
  } else {
    println("Unknown error");
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    println("Error writing to flash");
    return p;
  } else {
    println("Unknown error");
    return p;
  }

  delay(100);
  terminalRead = "0";


  println("Please type name for ID" + String(id));

  String name = terminalRead;

  while (name == "0") {
    Blynk.run();
    name = terminalRead;
  }

  String strtime = timeReal;
  while ((templateFingerData = getFingerprint(id)) == String(FINGERPRINT_PACKETRECIEVEERR))
    ;
  saveToFirebase(strtime, name, id, templateFingerData);
  printLCD("SUCCESS!!!", "WELCOME " + nameSeparation(name));
  delay(1000);
  templateFingerData = "";
  terminalRead = "0";
  id = 0;
  ok1 = true;
  return id;
}

// Lấy template vân tay từ cảm biến trả về một mảng chuỗi 512 phần tử
String getFingerprint(uint8_t id) {
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #");
  Serial.println(id);
  uint8_t p = finger.loadModel(id);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template ");
      Serial.print(id);
      Serial.println(" loaded");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return String(p);
    default:
      Serial.print("Unknown error ");
      Serial.println(p);
      return String(p);
  }

  // OK success!

  Serial.print("Attempting to get #");
  Serial.println(id);
  p = finger.getModel();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template ");
      Serial.print(id);
      Serial.println(" transferring:");
      break;
    default:
      Serial.print("Unknown error ");
      Serial.println(p);
      return String(p);
  }


  uint8_t f_buf[512];
  String templateF = "";
  if (finger.get_template_buffer(512, f_buf) == FINGERPRINT_OK) {
    for (int k = 0; k < 4; k++) {
      for (int l = 0; l < 128; l++) {
        templateF += String(f_buf[(k * 128) + l]) + ":";
      }
    }
  }
  return templateF;
}