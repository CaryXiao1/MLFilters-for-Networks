/* This file was created by Cary Xiao on January 5th, 2021.
 * Runs objectives of Laser_Communication_v1_2021 but with Manchester encoding
 * ---------------------------------------------------------
 * Things to add: Change init values after message ends
 * or if data consistently reads 0's or 1's 
 */
/*-----Global Variables from Laser_Communication-----*/
const int input[12] = {A0, A1, A2, A3, A4, A5,     // Used to declare analog inputs
                    A6, A7, A8, A9, A10, A11};     // 0-5 = Columns, 6-11 = Rows
int a[12]; // used to determine 1 vs. 0; analog values when laser isn't hitting
int b[12]; // alternates with a; a and b = modified from Manchester

const byte HEADER = 255; // openings and closings for each packet to be
const byte FOOTER = 0; // sent and received. Note: Header MUST have largest bit as 1
const int LASER_PIN = 50;

const int TIME_DELAY = 4; // millisecond interval for each bit
long now = 0, lastTime = 0; // both enable loop to run once per TIME_DELAY

String message = "";
byte data = 0; // byte for determining the character being sent

byte toSend[142] = {HEADER}; // resulting incoming/receiving message

int sensor = 0; // Column analog pin (0 - 5) to determine receiving information
int subBit = 7; // sublocation for the bit currently being sent

boolean sending = false, finishedReading = false;
boolean bitSend = false; // variable used to send bits
int messageSize; // size of sending message, adds six for the header and closer to be checked
/*-----Global Variables from Manchester_Communication-----*/
boolean currentlyA = true; // used to alternate between a and b
boolean receivingMessage = false, check = false; // check = measures change every 2 cycles

boolean sendingBit; // bit to be sent by manchester encoding


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
  digitalWrite(LASER_PIN, HIGH);
}

void loop() {
  //### Change while loop to function w/ manchester encoding
  while ((now - lastTime) < TIME_DELAY / 2) // Plays loop once every TIME_DELAY
  { 
    now = millis();
  }
  lastTime = now;

  /*-----#Code for receiving information-----*/
  if (currentlyA) {
    for (int i = 0; i < 12; i++) a[i] = analogRead(input[i]);
  }
  else {
    for (int i = 0; i < 12; i++) b[i] = analogRead(input[i]);
  }

  if (!receivingMessage || check == currentlyA) {
    // updates data byte based off photodiode reading
    data = data << 1;

    int sig; // finds most recent bit
    static int place = 0;
    static boolean foundHeader = false;
    static char receive[140]; // byte array for storing message
    static int r = 0; // index for receive, 0 - 139
    
    if (currentlyA) sig = determineBit(a[sensor], b[sensor]);
    else sig = determineBit(b[sensor], a[sensor]);

    if (sig >= 0) {
    // Inverts and sets comparison for Manchester if 1st bit in message
      if (!receivingMessage) {
        receivingMessage = true;
        Serial.print(sig);
        check = currentlyA;
        place = 1;
        data = 1; // sig will always start off as 1
      }
      else {
        Serial.print(sig);
        data += sig;
        

        if (place == 7) {
          Serial.print(" ");
          place = 0;
          if (!foundHeader && data == HEADER) foundHeader = true;
          else if (foundHeader) {


          if (data != FOOTER) {
            if (r < 140) {
              receive[r] = data;
              r++;  
            }
          }
          /*else {
          Serial.println();
          Serial.println("Message Received: " + (String) receive);
          Serial.println();
          Serial.println("-----------------------------");
          memset(receive, 0, 140);
          
          r = 0;
          foundHeader = false;
          data = 0;
          }*/
        }
      }
      else place++;
      }
    }
    // End of message or message was interrupted.
    else if (receivingMessage) {
      int test = data << 1; // equals 0 if last 7 bits were 0

      if (place == 7 && test == 0) {
          place = 0;
          receivingMessage = false;
          Serial.println();
          Serial.println("Message Received: " + (String) receive);
          Serial.println();
          Serial.println("-----------------------------");
          memset(receive, 0, 140);
          
          r = 0;
          foundHeader = false;
          data = 0;
      }
      else {
      receivingMessage = false;
      Serial.println();
      Serial.println("Connection severed. Data Received: " + (String) receive);
      Serial.println();
      Serial.println("-----------------------------");
      memset(receive, 0, 140);
          
      r = 0;
      foundHeader = false;
      data = 0;
      place = 0;
      }
    }
  }

  currentlyA = !currentlyA;


  /*-----#Code for sending information-----*/
  if (!sending) sending = findCharacters(false);
  else if (!finishedReading) findCharacters(true);
  else {
    static int sub = 7;
    static int location = 0;
    int subBit = now % TIME_DELAY;
    
    // digitalWrite(LASER_PIN, bitRead(toSend[location], sub));
    //###finish coding below – tests if ready to record new bit
    
    if (subBit < TIME_DELAY / 2) {
      
      bitSend = bitRead(toSend[location], sub);
      digitalWrite(LASER_PIN, bitSend);
    
      if (sub == 0) { // Cycles to next bit
        sub = 7;
        if (location == messageSize) {
          Serial.println();
          Serial.println("Sent!");
          Serial.println();
          Serial.println("-----------------------------");

          // Resets toSend and all other variables
          digitalWrite(LASER_PIN, 1);
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
    // toggles laser on 2nd half of the laser 
    else {
      digitalWrite(LASER_PIN, !bitSend);
    }
    
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
}*/

/* Reads any user input; places into toSend byte array
 * & enables Arduino to begin sending information.
 *  @param isReplay – true if part of unsent message is already in toSend
 *  Returns false if no buffered user information.
 */
boolean findCharacters(boolean isReplay) {
  static int i = 0;  
  
  if (Serial.available() > 0 && i < 140) {
    if (!isReplay) {
      toSend[0] = HEADER;
      i++; 
      
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

     return true;
  }
  else if (isReplay) {
    finishedReading = true;
    messageSize = i;
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

/* Determines whether and which bit was sent through
 * Manchester Encoding.
 *  @param current – the most recent value for A0.
 *  @param prev – the second most recent value for A0.
 *  Returns -1 if connection has been ended. 
 */
int determineBit(int current, int prev) {
  int diff = current - prev;
  if (abs(diff) >= 15) { // Either 0 or 1 was sent
    if (diff > 0) return 1;
    else return 0;
  }
  else return -1;
}
