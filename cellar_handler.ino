// By awfuchs, public domain
//
// Adapted from example testing sketch for DHT sensors by ladyada

#include "DHT.h"

#define DHTPIN 3     // what digital pin we're connected to
#define TARGET_TEMP 18.5
#define NEVER_EXCEED 25         // Don’t ever adapt higher than 77*F

#define USEC_PER_SAMPLE 5000    // Five seconds per sample

// Set the DHT type: uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

int numSamples=0;
bool fanOn=false;

// vars for adaptive temperature and other duty-cycle driven things
#define SAMPLES_PER_PERIOD 120              // Ten minute evaluation period
#define FULL_LOAD_DC SAMPLES_PER_PERIOD     // 25% duty cycle is full load (assumes 4 periods)
#define LITE_LOAD_DC SAMPLES_PER_PERIOD*0.6 // 15% duty cycle is light load (assumes 4 periods)

float adapTemp = TARGET_TEMP;
int fanOnCount = [0, 0, 0, 0]; // Duty cycle counter per period
int period = 0;                   // Counts 0..3 and repeat
int samplesThisPeriod = 0;        // Counts 0..119 and repeats


// =========== The Arduino setup function ===========
void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);


  dht.begin();
}

// =========== The Arduino loop ==============
void loop() {
  
  // Wait for next sample interval, then bump counters...
  
  delay(USEC_PER_SAMPLE);
  numSamples += 1;
  samplesThisPeriod +=1;
  if (fanOn == true) {
    fanOnCount[period] += 1;
  }

// ...handle duty cycle processing...

if (samplesThisPeriod >= SAMPLES_PER_PERIOD) {
  // --- end of period, time to reckon, adapt, and wrap ---

  // reckon
  totalOnes =  fanOnCount[0];
  totalOnes += fanOnCount[1];
  totalOnes += fanOnCount[2];
  totalOnes += fanOnCount[3];

  
  // adapt
  if (totalOnes > FULL_LOAD_DC && adapTemp <= NEVER_EXCEED) {
    // --- More than approved duty cycle: increase temp  ---
    adapTemp += 0.1;
  }
  else if (totalOnes < LITE_LOAD_DC) {
    if (adapTemp > TARGET_TEMP) {
      // --- Under light duty cycle level: reduce temp ---
      adapTemp -= 0.1;
    }
  }
  // wrap
  period = (period + 1) % 4;  // Increment the period 0,1,2,3,0,1,2,3...
  samplesThisPeriod = 0;
  fanOnCount[period] = 0;
}

  //...then take readings...
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // ...adjust the cooling...
  
  if( t<=adapTemp ) { fanOn=false; digitalWrite(LED_BUILTIN, LOW); }
  else              { fanOn=true; digitalWrite(LED_BUILTIN, HIGH); }

  // ...and finally report data.
  
  if( fanOn==true ) { Serial.print("OPERATING -- "); }
  else              { Serial.print("IDLE -- "); }
  
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F");
  Serial.print(h);
  Serial.println("%");
}
