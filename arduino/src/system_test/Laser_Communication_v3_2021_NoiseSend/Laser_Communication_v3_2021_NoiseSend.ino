/* This file was created by Cary Xiao on February 1, 2021.
 * Runs objectives of Laser_Communication_v3_2021 but with
 * automatically added error (defined by errorRate)
 */

const int ERROR_RATE = 500; // error rate = 1 bit per 'ERROR_RATE' bits
 
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

int sensor = 3; // Column analog pin (0 - 5) to determine receiving information
int subBit = 7; // sublocation for the bit currently being sent

boolean sending = false, finishedReading = false;
boolean bitSend = false; // variable used to send bits
int messageSize; // size of sending message, adds six for the header and closer to be checked
/*-----Global Variables from Manchester_Communication-----*/
boolean currentlyA = true; // used to alternate between a and b
boolean receivingMessage = false, check = false; // check = measures change every 2 cycles

boolean sendingBit; // bit to be sent by manchester encoding

int error[8], sub[8], e = 0; // arrays to notate location of error in the system. 

/*-----Extra variables for receiving information-----*/
int sig; // finds most recent bit
int place = 0;
boolean foundHeader = false;
char receive[140]; // byte array for storing message
int r = 0; // index for receive, 0 - 139

int n = 0;
int delayNum = 0; // set to 25 to give delay between sending information

void setup() {
  pinMode(LASER_PIN, OUTPUT);
 
  Serial.begin(9600); // Opening Information has been moved to Java App
  digitalWrite(LASER_PIN, HIGH);
  
  randomSeed(analogRead(A0));
}

void loop() {
  //### Change while loop to function w/ manchester encoding
  while ((now - lastTime) < TIME_DELAY / 2) // Plays loop once every TIME_DELAY
  { 
    now = millis();
  }
  lastTime = now;


  /*-----#Code for sending information-----*/
  if (delayNum <= 0) {
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
        Serial.print(bitSend);
      
        if (sub == 0) { // Cycles to next bit
          sub = 7;
          if (location == messageSize) {
            Serial.println();
            Serial.println(++n);
  
            // Resets toSend and all other variables
            digitalWrite(LASER_PIN, 1);
            location = 0;
            delayNum = 250;
          }
          else location++;
       }
       else sub--;
      }
      // toggles laser on 2nd half of the laser 
      else {
        // randomly adds error into the system
        if (random(ERROR_RATE) == 1) digitalWrite(LASER_PIN, bitSend);
        else digitalWrite(LASER_PIN, !bitSend);
      }
      
    }
  }
  else delayNum--;
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

/* Prints out received message and attempts to remove
 * any obvious errors (see filterError).
 *  @param interrupted - whether signal was completely lost
 */
void printMessage(boolean interrupted) {
  Serial.println();
  if (interrupted) Serial.print("Connection Severed. ");
  Serial.print("Message Received: " + (String) receive); 

    // uses filterError to fix any obvious errors
  int errorsFixed = 0;
  if (!interrupted) {
    for (int i = 0; i < e; i++) {
      byte test = filterError(receive[error[i]], sub[i]);
  
      if (test != 0) {
        receive[error[i]] = test;
        error[i] = -1, sub[i] = -1;
        errorsFixed++;
      }
    }
    if (errorsFixed > 0) {
      Serial.println("\nArduino-Corrected Message: " + (String) receive);
    }
    
  }
  Serial.println("\nErrors Corrected: " + (String) errorsFixed);
  if (e > 0) {
    Serial.print("Locations of Error: {");
    for (int i = 0; i < e; i++) {
      // Notates locations of bits with error
      Serial.print(error[i]);
      Serial.print(",");
      Serial.print(sub[i]);
      Serial.print(";");
    }
  Serial.println("}");

  if (e > errorsFixed) Serial.println("ML-Corrected Data: " + (String) receive);
  }


  Serial.println("-----------------------------");
  memset(receive, 0, 140);

  receivingMessage = false;
  r = 0;
  foundHeader = false;
  data = 0;
  place = 0;
  e = 0;
}

/* Filters out any errors within bytes based off ASCII table.
 * If not valid ASCII character, then eliminate and return
 * correct byte. 
 *  @param current – byte to be examined
 *  @param sub - location of error bit in byte
 *  Returns correct byte if able to remove option, 0 if not.
 */
byte filterError(byte current, int sub) {
  // calculates alternative bit; sub increases from left to right
  byte alternative = current + pow(2, 7 - sub);

  // Characters not in specified values / ranges are deemed as error
  if (!(alternative > 31 && alternative < 35      // space, !, "
          || alternative > 43 && alternative < 59   // comma, -, ., 0-9, /, :
          || alternative > 64 && alternative < 91   // A-Z
          || alternative > 96 && alternative < 123  // a-z
          || alternative == 39                      // '
          || alternative == 63)) return current;    // ?
  else if (!(current > 31 && current < 35      // space, !, "
          || current > 43 && current < 59   // comma, -, ., 0-9, /, :
          || current > 64 && current < 91   // A-Z
          || current > 96 && current < 123  // a-z
          || current == 63)) return alternative;
  else return 0;
}
