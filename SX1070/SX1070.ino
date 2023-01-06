/****************************************
 * Include Libraries
 ****************************************/


/****************************************
 * Define Transmissive Photo Interrupter, Phototransistor Constants
 ****************************************/

unsigned long previousMillis = 60000; // timeout comparitor
unsigned long currentMillis = 0;      // timeout check
unsigned long timeout = 0;            // could be large 999 days!!!! but unsigned long is still only 92 days
unsigned long startMillis = 60000;    // bubble width comparitor
unsigned long interval = 0;           // The time interval from start to end of a bubble, which appears as two interruptions
unsigned long count = 0;              // Assume a very large number of bubbles per minute
int theshold = 0;                     // max value is 1024
bool bubble = false;                  // indicate start of the bubble 
  
/****************************************
 * Main Functions
 ****************************************/

void setup() {

  Serial.begin(9600);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.

  while(!Serial) { }

  // initialise analogue pin A0 as input
  pinMode(A0, INPUT);
  pinMode(D0,WAKEUP_PULLUP);
  Serial.println("Started");

  // Set up the timers

  timeout =60000;   // every minute
  //timeout =10000;   // every 10 Seconds
  interval = 400;   // milliseconds for the bubble to be considered separate
  theshold = 300;   // amount of background light before the bubble disturbance is noticed 
  count = 0;        //
  bubble = false;   //
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage HIGH
  
}
 
// the loop function runs over and over again forever
void loop() {

  currentMillis = millis();
  if (currentMillis - previousMillis >= timeout) {

    Serial.println("reset");
    Serial.print("count=");
    Serial.println(count);
    count = 0;
    bubble = false;
    
    previousMillis = currentMillis;
  }
  else {

    // Only count bubbles if there is a transition from high to low and then back to high
     
    float raw = 0;
    raw = analogRead(A0);
    if (raw < theshold) {
      if (bubble == false) {
          Serial.println("start");
          digitalWrite(LED_BUILTIN, LOW);    // turn the LED on by making the voltage LOW
          bubble = true;
          startMillis = currentMillis;
      }
    }
    else {
      if (bubble == true) {
        // Potentailly this is the end of the bubble
        // but actually probably just the surface of the
        // bubble either at the start or the end
        // could just halve the counts assuming this
        // check if all bubble counts are even - no not always even

        // Check if still in the bubble

        if (currentMillis - startMillis >= interval) {
          Serial.println("outside");
          bubble = false;
          count++;
          Serial.print("count=");
          Serial.println(count);
          Serial.println("end");
          digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage HIGH
        }
      }
      else
      {
        digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage HIGH
      }
    }
  }  
}
