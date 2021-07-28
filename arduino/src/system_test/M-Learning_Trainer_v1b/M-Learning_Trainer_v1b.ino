/* This file was created by Cary Xiao on January 14th, 2021.
 * v1b = receiving side: inputs & formats analog port data 
 * for data .arff file in Weka.
 */
/*-----Global Variables from Laser_Communication-----*/
const int input[12] = {A0, A1, A2, A3, A4, A5,     // Used to declare analog inputs
                    A6, A7, A8, A9, A10, A11};     // 0-5 = Columns, 6-11 = Rows

const int TIME_DELAY = 10; // millisecond interval for each bit
long now = 0, lastTime = 0; // both enable loop to run once per TIME_DELAY

int sensor = 0; // Column analog pin (0 - 5) to determine 1st bit

int messageSize; // size of sending message, adds six for the header and closer to be checked
/*-----Global Variables from Manchester_Communication-----*/
boolean newBit; // used to alternate between a and b
int start = false; // check = measures change every 2 cycles

int expectedResult[3] = {0, 1}; // pattern set in signal
int e = 0; // e = index for expectedResult
int expectedLaserPos = 0; // 

static int previous;
static int current;
boolean firstTime = true;

void setup() {

  // Opening Information
  Serial.begin(19200);
  Serial.println("Machine Learning Trainer v1b: Receiver"); 
  Serial.println("Will start recording once first bit is sent."); 
  Serial.println("____________________________________________________________________________________");
}

void loop() {
  //### Change while loop to function w/ manchester encoding
  while ((now - lastTime) < TIME_DELAY / 2) // Plays loop once every TIME_DELAY
  { 
    now = millis();
  }
  lastTime = now;

  /*-----#Code for receiving information-----*/
  if (!start) {
    if (firstTime) {
      previous = analogRead(input[sensor]);
      current = analogRead(input[sensor]);
      firstTime = false;
    }
    else {
      previous = current;
      current = analogRead(input[sensor]);
    }

    if ((current - 7) > previous)  {
      start = true;
      newBit = true;
    }

  }

  if (start) {
    if (newBit) {
      for (int i = 0; i < 12; i++) {
        Serial.print(analogRead(input[i]));
        Serial.print(",");
      }  
    }
    else {
      for (int i = 0; i < 12; i++) {
        Serial.print(analogRead(input[i]));
        Serial.print(",");
      }
      Serial.print(expectedResult[e]);
      Serial.print(",");
      Serial.println(expectedLaserPos);
      e++;
      if (e == 2) e = 0;
    }
    newBit = !newBit;
  }
  
}
