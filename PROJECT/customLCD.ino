void printLCD(String str1, String str2) {

  lcdB.clear();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str1);
  lcdB.print(0, 0, str1);
  lcd.setCursor(0, 1);
  lcd.print(str2);
  lcdB.print(0, 1, str2);
  delay(1500);
}

void printLCDDefault() {
  lcdB.clear();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO");
  lcdB.print(0, 0, "WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("ATTENDANCE CHECK");
  lcdB.print(0, 1, "ATTENDANCE CHECK");
  delay(1500);
}