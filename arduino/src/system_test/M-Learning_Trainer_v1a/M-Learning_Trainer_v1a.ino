/* This file was created by Cary Xiao on January 14th, 2021.
 * v1a = sending component, sends repeating values for 0, 1, 
 * and not connected (-1)
 */
/*-----Global Variables from Laser_Communication-----*/
const int LASER_PIN = 50;

const int TIME_DELAY = 10; // millisecond interval for each bit
long now = 0, lastTime = 0; // both enable loop to run once per TIME_DELAY

byte toSend[3] = {170, 85}; // 170 = 10101010 in binary, 85 = 01010101 

int subBit = 7; // sublocation for the bit currently being sent

boolean start = false; // used to start sending info once info is inputted into serial monitor
boolean bitSend = false; // variable used to send bits

int i = 0, sub = 7;

void setup() {
  pinMode(LASER_PIN, OUTPUT);

  // Opening Information
  Serial.begin(9600);
  Serial.println("Machine Learning Trainer v1a: Sender"); 
  Serial.println("Enter any key to start sending information."); 
  Serial.println("____________________________________________________________________________________");
  digitalWrite(LASER_PIN, HIGH);
}

void loop() {
  //### Change while loop to function w/ manchester encoding
  while ((now - lastTime) < TIME_DELAY / 2) // Plays loop once every TIME_DELAY
  { 
    now = millis();
  }
  lastTime = now;

  /*-----#Code for sending information-----*/
  if (start) {
    bitSend = bitRead(toSend[i], sub);
    digitalWrite(LASER_PIN, !bitSend);
    Serial.print(bitSend);
        
    if (sub == 6) { // Cycles to next bit
      sub = 7;
      Serial.print(" ");
      i++;
      if (i == 2) i = 0;
    }
    else sub--;
  }
  else if (Serial.available() > 0) start = true;
}
