/* This file was created by Cary Xiao on January 5th, 2021.
 * Streams / recieves constant 0's through manchester encoding, using A0 to receive.
 * Also notifies when connection has been terminated.
 */
const int TIME_DELAY = 50, LASER_PIN = 50;
boolean data = false, reverse = false; // data = bit to be repeated by manchester encoding

int a, b; // used for comparisons to find bit
boolean currentlyA = true; // boolean used to alternate
boolean receivingMessage = false, check = false; 
// check used to measure change every 2 cycles

long now = 0, lastTime = 0; // both enable loop to run once per TIME_DELAY

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
  if (reverse) {
    digitalWrite(LASER_PIN, !data);
    if (data) reverse = false;
  }
  else {
    digitalWrite(LASER_PIN, data);
    if (data) reverse = true;
  }
  data = !data;
  
  
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
        Serial.print(!sig);
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
