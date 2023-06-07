// -------------------------------------------------FIREBASE Function--------------------------------------------------------
//FIREBASE Function

void saveToFirebase(String time, String name, int id, String templateFinger) {

  // Gửi dữ liệu lên Firebase
  Firebase.setString(firebaseData, path + "/fingerData" + "/" + String(id - 1) + "/time", time);
  Firebase.setString(firebaseData, path + "/fingerData" + "/" + String(id - 1) + "/name", name);
  Firebase.setString(firebaseData, path + "/fingerData" + "/" + String(id - 1) + "/id", "ID" + String(id));
  Firebase.setString(firebaseData, path + "/fingerData" + "/" + String(id - 1) + "/templateFinger", templateFinger);
  println("========================");
  println("ID" + String(id));
  println("Name: " + name);
  println("Time: " + time);
  println("Template Fingerprint" + templateFinger);
  println("Save to Firebase success");
  println("========================");
  sendDataToGGSheet();
}
void saveToFirebaseAt(String time, String name, int id) {
  // Gửi dữ liệu lên Firebase
  FirebaseJson json;

  json.add("time", time);
  json.add("name", name);
  json.add("id", "ID" + String(id));
  Firebase.pushJSON(firebaseData, path + "/attendance", json);

  println("========================");
  println("ID" + String(id));
  println("Name: " + name);
  println("Time: " + time);
  println("Save to Firebase success");
  println("========================");

  sendDataToGGSheet();
}
// void deleteToFirebase(int id) {
//   String str = "";
//   if (Firebase.getString(firebaseData, path + "/fingerData" + "/" + String(id))) {
//     str = firebaseData.stringData();
//   }
//   Firebase.pushString(firebaseData, path + "/fingerDelete" + "/" + String(id), str);
//   Firebase.deleteNode(firebaseData, path + "/fingerData" + "/" + String(id));
//   println("==========================");
//   println("Data deleted successfully.");
//   println("==========================");
// }
bool checkNodeFirebase(String nodePath) {
  if (Firebase.getString(firebaseData, nodePath)) {
    return true;
  } else {
    println(firebaseData.errorReason());
    return false;
  }
}

void getFullDataFromFirebase() {


  if (Firebase.getString(firebaseData, path + "/fingerData")) {

    parseJSON(firebaseData.stringData());
    idtmp = 1;
    for (int i = 0; i < count; i++) {
      if (idtmp == ids[i].substring(2).toInt()) {
        idtmp++;
      } else break;
    }
  } else {
    Serial.println(firebaseData.errorReason());
  }
}


// void getDataDeleteFromFirebase(String str) {


//   if (Firebase.getString(firebaseData, path + "/" + str)) {

//     parseJSON(firebaseData.stringData());
//   } else {
//     Serial.println(firebaseData.errorReason());
//   }
// }


// -----------------------------------------------END FIREBASE Function----------------------------------------------------------
