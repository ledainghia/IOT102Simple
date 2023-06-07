// --------------------------------------------Custom print for SERIAL and TERMINAL-------------------------------------------------------------
//Custom print for SERIAL and TERMINAL
void print(String str) {
  Serial.print(str);
  terminal.print(str);
  terminal.flush();
}

void println(String str) {
  Serial.println(str);
  terminal.println(str);
  terminal.flush();
}

void println() {
  Serial.println("\n");
  terminal.println("\n");
  terminal.flush();
}
// ---------------------------------------END Custom print for SERIAL and TERMINAL------------------------------------------------------------------
