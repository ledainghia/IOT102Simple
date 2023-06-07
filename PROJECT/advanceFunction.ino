// processing fingerprint template to write to the sensor
void processingTemplateFinger(int idt, String str) {

  int i = 0;
  int startPos = 0;
  int commaPos = str.indexOf(':', startPos);

  while (commaPos != -1) {
    String token = str.substring(startPos, commaPos);

    fingerTemplate[i] = token.toInt();
    startPos = commaPos + 1;
    commaPos = str.indexOf(':', startPos);
    i++;
  }
  // Xử lý phần tử cuối cùng trong chuỗi
  String lastToken = str.substring(startPos);
  fingerTemplate[i] = lastToken.toInt();
  Serial.println(str.substring(startPos));

  write_template_data_to_sensor(idt);
}

// Tách họ và tên để chỉ lấy tên
String nameSeparation(String fullName) {

  Serial.println(fullName);
  // Tách họ và tên
  fullName.trim();
  String name = "";
  String middleName = "";
  int firstSpaceIndex = fullName.indexOf(' ');
  int lastSpaceIndex = fullName.lastIndexOf(' ');

  if (firstSpaceIndex != -1 && lastSpaceIndex != -1 && firstSpaceIndex != lastSpaceIndex) {
    middleName = fullName.substring(firstSpaceIndex + 1, lastSpaceIndex);
    Serial.println("midname " + middleName);
    name = fullName.substring(lastSpaceIndex + 1);
  } else {
    // Trường hợp không tìm thấy họ và tên đệm, chỉ có tên
    middleName = "";
    name = fullName;
  }
  return name;
}

//Tách chuỗi JSON
void parseJSON(const String& jsonString) {

  //Serial.println(jsonString);
  //   // Bỏ qua ký tự đầu '[' và cuối ']'
  String trimmedJson = jsonString.substring(1, jsonString.length() - 1);

  // Lưu trữ số lượng đối tượng JSON
  count = 0;

  memset(ids, 0, 50);
  memset(names, 0, 50);
  memset(times, 0, 50);
  memset(tFingerPrint, 0, 50);
  Serial.println(" haha");
  // Tìm các dấu '{' trong chuỗi JSON để đếm số lượng đối tượng
  for (int i = 0; i < trimmedJson.length(); i++) {
    if (trimmedJson.charAt(i) == '{') {
      count++;
    }
  }

  // Khởi tạo mảng các biến để lưu trữ giá trị từ chuỗi JSON



  // Lưu trữ chỉ mục hiện tại trong mảng
  int currentIndex = 0;
  Serial.println(" deptrai");
  // Bỏ qua phần tử đầu tiên nếu là null
  if (trimmedJson.startsWith("null")) {
    int startIndex = trimmedJson.indexOf('{');
    if (startIndex >= 0) {
      trimmedJson = trimmedJson.substring(startIndex);
    } else {
      Serial.println("Không tìm thấy chuỗi JSON sau null.");
      return;
    }
  }

  // Tìm các đối tượng JSON và trích xuất giá trị
  int startIndex = trimmedJson.indexOf('{');
  int endIndex = trimmedJson.indexOf('}');
  while (startIndex >= 0 && endIndex >= 0) {
    String jsonObject = trimmedJson.substring(startIndex, endIndex + 1);

    // Trích xuất giá trị của trường 'id'
    int idStart = jsonObject.indexOf("\"id\":\"") + 6;
    int idEnd = jsonObject.indexOf("\",", idStart);
    ids[currentIndex] = jsonObject.substring(idStart, idEnd);

    // Trích xuất giá trị của trường 'name'
    int nameStart = jsonObject.indexOf("\"name\":\"") + 8;
    int nameEnd = jsonObject.indexOf("\",", nameStart);
    names[currentIndex] = jsonObject.substring(nameStart, nameEnd);


    // Trích xuất giá trị của trường 'templateFingerData'
    int templateFingerDataStart = jsonObject.indexOf("\"templateFinger\":\"") + 18;
    Serial.println(templateFingerDataStart);
    int templateFingerDataEnd = jsonObject.indexOf("\",", templateFingerDataStart);
    tFingerPrint[currentIndex] = jsonObject.substring(templateFingerDataStart, templateFingerDataEnd);

    // Trích xuất giá trị của trường 'time'
    int timeStart = jsonObject.indexOf("\"time\":\"") + 8;
    int timeEnd = jsonObject.indexOf("\"}", timeStart);
    times[currentIndex] = jsonObject.substring(timeStart, timeEnd);

    // Di chuyển đến vị trí bắt đầu của đối tượng JSON tiếp theo
    startIndex = trimmedJson.indexOf('{', endIndex);
    if (startIndex >= 0) {
      endIndex = trimmedJson.indexOf('}', startIndex);
      currentIndex++;
      Serial.println("lozmabug6");
    } else {
      // Kết thúc vòng lặp nếu không còn đối tượng JSON
      break;
    }
  }
}