/* This file was created by Cary Xiao on January 6th, 2021.
 * Adds sending Serial prompt ability (like in Laser_Communication) 
 * to the previous code made in Manchester_Communication_v1.
 * ---------------------------------------------------------
 * Idea for sending bits – set boolean to sending bit every TIME_DELAY,
 * use two if statements to either send a 0 then 1 for a 1 
 * or 1 then 0 for a 0.
 */
const int TIME_DELAY =50, LASER_PIN = 50;
boolean data; // data = bit to be sent by manchester encoding

int a, b; // used for comparisons to find bit
boolean currentlyA = true; // boolean used to alternate
boolean receivingMessage = false, check = false; 
// check used to measure change every 2 cycles

long now = 0, lastTime = 0; // both enable loop to run once per TIME_DELAY

/*-----Variables from Laser_Communication-----*/
byte toSend[146] = {255, 255, 255}; // resulting incoming/receiving message
int messageSize;
boolean sending = false;
boolean finishedReading = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LASER_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  while ((now - lastTime) < TIME_DELAY / 2) // Plays loop once every TIME_DELAY
  { 
    now = millis();
  }
  lastTime = now;
  
  /*-----#Code for sending information-----*/
  if (!sending) sending = findCharacters(false);
  else if (!finishedReading) findCharacters(true);
  else {
    static int sub = 7;
    static int location = 0;
    int subBit = now % TIME_DELAY;
    
    // digitalWrite(LASER_PIN, bitRead(toSend[location], sub));
    //###finish coding below – tests if ready to record new bit
    if (subBit < 3) {
      
      data = bitRead(toSend[location], sub);
      digitalWrite(LASER_PIN, !data);
      Serial.print(data);
    
      if (sub == 0) { // Cycles to next bit
        sub = 7;
        if (location == messageSize) {
          Serial.println("Sent!");
          Serial.println();
          Serial.println("-----------------------------");

          // Resets toSend and all other variables
          digitalWrite(LASER_PIN, 0);
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
      digitalWrite(LASER_PIN, data);
    }
  }
  
  
  /*-----#Code for receiving information-----*/
  // SET a and b – currentlyA switched at end of loop
  if (currentlyA) a = analogRead(A0);
  else b = analogRead(A0);
  
  // Find the most recent bit
  if (!receivingMessage || check == currentlyA) {
    int sig;
    if (currentlyA) sig = determineBit(a, b);
    else sig = determineBit(b, a);

    if (sig >= 0) {
// Inverts and sets comparison for Manchester if 1st bit in message
      if (!receivingMessage) {
        receivingMessage = true;
        Serial.print(sig);
        check = currentlyA;
      }
      else Serial.print(sig);
      
    }
    // End of message or message was interrupted.
    else if (receivingMessage) {
      receivingMessage = false;
      Serial.println();
      Serial.println("Connection disconnected.");
    }
  }
  currentlyA = !currentlyA;
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
      for (i = 0; i < 3; i++) toSend[i] = 255;
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
