//#define DEBUGMODE

/****************************************
 * Include Libraries
 ****************************************/
#include "UbidotsESPMQTT.h"   //

/****************************************
 * Define Constants
 ****************************************/
 
#define TOKEN "[Token]" // Your Ubidots TOKEN
#define WIFINAME "[SSID]" //Your SSID
#define WIFIPASS "[Password]" // Your Wifi Password

Ubidots client(TOKEN);

// Loop parameters

unsigned long previousMillis = 60000; // timeout comparitor
unsigned long currentMillis = 0;      // timeout check
unsigned long timeout = 0;            // could be large 999 days!!!! but unsigned long is still only 92 days
unsigned long startMillis = 60000;    // bubble width comparitor
unsigned long interval = 0;           // The time interval from start to end of a bubble, which appears as two interruptions

// Specific to the interrupter

int threshold = 512;                  // Default to 512 out if 1024
int total = 0;                        // A count of events
bool trigger = false;                 // Start of interruption teigger
unsigned long count = 0;              // Assume a very large number of bubbles per minute
int theshold = 0;                     // max value is 1024
bool bubble = false;                  // indicate start of the bubble 

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup() {

  Serial.begin(9600);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.

  while(!Serial) { }

  // initialise analogue pin A0 as input for voltage measurement
  pinMode(A0, INPUT);
  pinMode(D0,WAKEUP_PULLUP);
  Serial.println("Started");
  client.setDebug(true); // Pass a true or false bool value to activate debug messages
  setup_wifi(WIFINAME, WIFIPASS);
  Serial.println("Setup");
  client.begin(callback);
  
  // Set up the timers

  timeout =60000;   // every minute
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

  if(!client.connected()){
    Serial.println("Not connected");
    client.reconnect();
  }
  
  currentMillis = millis();
  if (currentMillis - previousMillis >= timeout) {
	  Serial.println("Reset");
    Serial.print("count=");
    Serial.println(count);

    client.add("count", count);           //Insert your variable Labels and the value to be sent
    client.ubidotsPublish("ibubble");     //
    client.loop();

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
          Serial.print("Count=");
          Serial.println(count);
		  
          Serial.println("end");
          digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage HIGH
        }
      }
      else
      {

        digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage HIGH
        delay(5);
      }
    }
  }  
}

void setup_wifi(char* ssid, char* pswd) {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pswd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

