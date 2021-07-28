/* This file was created by Cary Xiao on December 28th, 2020.
 * Combines 2021 Position Finder code with updated version of 
 * last year's communication system. 
 * ---------------------------------------------------------
 * Things to add: Change init values after message ends
 * or if data consistently reads 0's or 1's 
 */

const int input[12] = {A0, A1, A2, A3, A4, A5,     // Used to declare analog inputs
                    A6, A7, A8, A9, A10, A11};     // 0-5 = Columns, 6-11 = Rows
int reference[12]; // used to determine 1 vs. 0; analog values when laser isn't hitting

const byte HEADER = 1; // openings and closings for each packet to be
const byte FOOTER = 0; // sent and received. 
const int LASER_PIN = 50;

const int TIME_DELAY = 2; // millisecond interval for each bit
long now = 0, lastTime = 0; // both enable loop to run once per TIME_DELAY

String message = "";
byte data = 0; // byte for determining the character being sent

byte toSend[146] = {HEADER, HEADER, HEADER}; // resulting incoming/receiving message

int sensor = 0; // Column analog pin (0 - 5) to determine receiving information
int subBit = 7; // sublocation for the bit currently being sent

boolean sending = false;
boolean finishedReading = false;
int messageSize; // size of sending message, adds six for the header and closer to be checked

void setup() {
  pinMode(LASER_PIN, OUTPUT);
  
  // Opening Information
  Serial.begin(9600);
  Serial.println("Welcome to the Full Duplex String Transfer!"); 
  Serial.println("This program can only transfer up to 140 characters, so please shorten your messages.");
  Serial.println();
  Serial.println("Also, due to one serial monitor on Arduino Unos, this program will only show that you");
  Serial.println("sent a message after your recieved message has been decoded and vise versa.");
  Serial.println("Anyways, happy messaging!");
  Serial.println("____________________________________________________________________________________");
  // Set initial reference values
  for (int i = 0; i < 12; i++) {
    reference[i] = analogRead(input[i]);
    
  }
  Serial.print(reference[0]);
}

void loop() {
  while ((now - lastTime) < TIME_DELAY) // Plays loop once every TIME_DELAY
  { 
    now = millis();
  }
  lastTime = now;
  

  /*-----#Code for receiving information-----*/
  int sensorValue = analogRead(input[sensor]);

  // updates data byte based off photodiode reading
  data = data << 1;
  if (sensorValue - reference[sensor] >= 25) {
    //###note think of changing 0 to column w/ center of laser
    data++;
    // updates analog pin if laser position has moved
    sensor = brightestDiff(0, 6);
  }
  else {
    // Rudimentary Adaptive Filter
    // Updates calibration for reference array while laser isn't shooting
    //for (int i = 0; i < 12; i++) {
    //  reference[i] = (3 * reference[i] + analogRead(input[i])) / 4;
    //}
  }
  
  static int headerTimes = 0; // static variables for receiving information
  static int place = 0;
  
  // Finds 1st header
  if (headerTimes == 0) {
    headerTimes += checkSentinel(data, HEADER, headerTimes);
    place = 0;
  }
  // Reads and certifies data past 1st header
    else if (place == 7) {
      if (headerTimes < 3) {
          headerTimes += checkSentinel(data, HEADER, headerTimes);
      } 

      // Code for reading and ending package
      else if (headerTimes == 3) {
        static char receive[140]; // byte array for storing message
        static int r = 0; // index for receive, 0 - 139

        static int enderTimes = 0;

/*        if (data == 0) { 
          if (enderTimes == 0) {
            enderTimes++;
          }
          
        }
        else if (enderTimes > 0) {
          enderTimes = 0;
        }*/
        // checks for consecutive footers
        enderTimes += checkSentinel(data, FOOTER, enderTimes);
        
        // records byte
        if (enderTimes == 0 && r < 140) {
          receive[r] = data;
          r++;  
        }
        
        // displays message in serial monitor once packet is delivered
        else if (enderTimes == 3) {
          Serial.println();
          Serial.println("Message Recieved: " + (String) receive);
          Serial.println();
          Serial.println("-----------------------------");
          memset(receive, 0, 140);
          
          r = 0;
          headerTimes = 0;
          enderTimes = 0;
        }
      }
      place = 0;
  }
  else place++;


  /*-----#Code for sending information-----*/
  if (!sending) sending = findCharacters(false);
  else if (!finishedReading) findCharacters(true);
  else {
    static int sub = 7;
    static int location = 0;
    digitalWrite(LASER_PIN, bitRead(toSend[location], sub));
    
    if (sub == 0) { // Cycles to next bit
      sub = 7;
      if (location == messageSize) {
        Serial.println("Sent!");
        Serial.println();
        Serial.println("-----------------------------");

        // Resets toSend and all other variables
        memset(toSend,0,sizeof(toSend));
        sending = false;
        finishedReading = false;
        location = 0;
        messageSize = 0;

      }
      else location++;
    }
    else sub--;
  }
}


/*  Finds focal point of laser between analogInputs start and finish.
 *   (Modified from function in Position Finder v1_2021)
 *   Precondition – start and finish positive, <= length 
 *     of input, start < finish 
 *   @param start – first desired analogInput port
 *   @param finish – first undesired 
 *   Postcondition – returns distance of highest value's 
 *     index from start
 */
int brightestDiff(int start, int finish) {
  int brightestVal = 0, bValIndex = 0;
  
  // Determines highest value
  for (int i = start; i < finish; i++) {
      if (analogRead(input[i]) - reference[i] > brightestVal) {
      brightestVal = analogRead(input[i]);
      bValIndex = i;
    }
  }
  return bValIndex - start; // inputs from start
}

/* Reads any user input; places into toSend byte array
 * & enables Arduino to begin sending information.
 *  @param isReplay – true if part of unsent message is already in toSend
 *  Returns false if no buffered user information.
 */
boolean findCharacters(boolean isReplay) {
  static int i = 0;  
  
  if (Serial.available() > 0) {
    if (!isReplay) {
      for (i = 0; i < 3; i++) toSend[i] = 1;
      Serial.println();
      Serial.print("Sending Message: ");
      while (i < 140 && Serial.available() > 0) {
        toSend[i] = Serial.read();
        Serial.print((char) toSend[i]);
        i++;
      }
    }
    else {
      while (i < 140 && Serial.available() > 0) {
        toSend[i] = Serial.read();
        Serial.print((char) toSend[i]);
        i++;
      }
    }
     messageSize = i;
     return true;
  }
  else if (isReplay) {
    finishedReading = true;
    i = 0;
    return false;
  }
  
  else return false;

  // ###note to add condition for realigning lasers
}

/* Compares test to sentinel. Primarially for
 * header and ender of packet.
 *  @param test – first byte to be compared.
 *  @param sentinel - second byte to be compared.
 *  @param place - value of original object.
 *  Returns 1 for equal, -1 * self for not equal
 */
int checkSentinel(byte test, byte sentinel, int self) {
  if (test == sentinel) {
    return 1;
  }
  else return (-1 * self);
}
