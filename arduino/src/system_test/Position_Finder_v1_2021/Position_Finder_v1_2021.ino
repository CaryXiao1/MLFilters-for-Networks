/* This file was created by Cary Xiao on December 28th, 2020.
 * Identifies the position the laser is hitting the photodiode array.
 */

const int input[12] = {A0, A1, A2, A3, A4, A5,     // Used to declare analog inputs
                   A6, A7, A8, A9, A10, A11};      // 0-5 = Columns, 6-11 = Rows


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Position of laser – Column (0 = topmost, 5 = bottommost), Row (0 = rightmost, 5 = leftmost)");
  Serial.println("-------------------------------------------------------------------------------------------"); 

}

void loop() {
  
    // prints out analog inputs
  // for (int i = 0; i < 12; i++) {
    // Serial.print(analogRead(input[i]));
    // if (i < 11) Serial.print(", ");}

  // determines brightest column
  Serial.print(brightest(0, 6));
  Serial.print(", ");
  
  // determines brightest row
  Serial.print(brightest(6, 12));
  
  Serial.println();
  delay(500);
}


/*  Finds highest value of analogInput betweem start and finish.
 *   Precondition – start and finish positive, <= length 
 *     of input, start < finish 
 *   @param start – first desired analogInput port
 *   @param finish – first undesired 
 *   Postcondition – returns distance of highest value's 
 *     index from start
 */
int brightest(int start, int finish) {
  int brightestVal = 0, bValIndex = 0;
  
  // Determines brightest column or row
  for (int i = start; i < finish; i++) {
      if (analogRead(input[i]) > brightestVal) {
      brightestVal = analogRead(input[i]);
      bValIndex = i;
    }
  }
  return bValIndex - start; // inputs from start
}
