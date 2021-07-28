/* Created by Cary Xiao on January 21, 2021
 *  Sends information placed into Serial monitor back to Computer.
 *  Used to test Machine Learning
 * 
 */

String toSend = String();

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available() > 0) {
    toSend += (char) Serial.read();
    delay(4);
  }
  if (toSend != "") {
  //Serial.println("ML-Corrected Data: " + toSend);
  Serial.println("Locations of Error: " + toSend);
  toSend = "";
  }
}
