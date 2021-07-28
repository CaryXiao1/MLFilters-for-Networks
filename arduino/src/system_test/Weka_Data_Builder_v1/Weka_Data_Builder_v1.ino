/* This file was created by Cary Xiao on January 18th, 2021.
 * Creates all possible 1-bit errors with a given sentence, giving it a 
 * ---------------------------------------------------------
 * Things to add: Change init values after message ends
 * or if data consistently reads 0's or 1's 
 */
 
char input[140]; 
int inputSize = 0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
  boolean calculate = findCharacters();

  if (calculate) {
    // transverses bytes
    for (int i = 0; i < inputSize - 1; i++) {
      // transverses bits
      for (int j = 0; j < 8; j++) {
          byte temp = input[i];
          String init = (String) input;
          byte alternative; 
          
        if (bitRead(input[i], j) == 0) {
          // Creates data where ML should return 0 – alternative is incorrect.
          alternative = input[i] + pow(2, 7 - j);
          if (alternative > 31 && alternative < 35  // space, !, "
          || alternative > 43 && alternative < 59   // comma, -, ., 0-9, /, :
          || alternative > 64 && alternative < 91   // A-Z
          || alternative > 96 && alternative < 123  // a-z
          || alternative == 63) {                   // ?
          
            byte temp = input[i];
            String init = (String) input;
            // print case when bit shouldn't be reversed
            Serial.print("'");
            for (int k = 0; k < inputSize - 1; k++) Serial.print(input[k]);
            Serial.print("','");
            Serial.print((char) alternative);
            Serial.print("',");
            Serial.println(0);

            /* print case when bit should be reversed
            input[i] = alternative;
            alternative = temp;
            init = (String) input;
            Serial.print("'");
            for (int k = 0; k < inputSize - 1; k++) Serial.print(input[k]);
            Serial.print("','");
            Serial.print((char) alternative);
            Serial.print("',");
            Serial.println(1);
            input[i] = temp;*/
          }
        }
        else if (bitRead(input[i], j) == 1) {
          // Creates data where ML should return 1 – original is incorrect.
          // Create alternative (w/ bit = 0)
          alternative = input[i] - pow(2, 7 - j);

          if (alternative > 31 && alternative < 35  // space, !, "
          || alternative > 43 && alternative < 59   // comma, -, ., 0-9, /, :
          || alternative > 64 && alternative < 91   // A-Z
          || alternative > 96 && alternative < 123  // a-z
          || alternative == 63) {                   // ?

            // switch alternative with original byte
            byte temp = input[i];
            input[i] = alternative;
            alternative = temp;
            
            Serial.print("'");
            for (int k = 0; k < inputSize - 1; k++) Serial.print(input[k]);
            Serial.print("','");
            Serial.print((char) alternative);
            Serial.print("',");
            Serial.println(1);
            input[i] = temp;
          }
        }
      }

    }
    memset(input, 0, sizeof(input));
    calculate = false;
  }
}

boolean findCharacters() {
  int i = 0;
  if (Serial.available() > 0) {
    delay(500);
    while (Serial.available() > 0 && i < 140) {
      input[i] = Serial.read();
      i++;
    }
    inputSize = i;
    return true;
  }
  else return false;
}
