// By awfuchs, public domain
//
// Adapted from example testing sketch for DHT sensors by ladyada

#include "DHT.h"

#define TARGET_TEMP 17.5    // Maintain this temp if you can
#define NEVER_EXCEED 25     // Don’t ever adapt higher than 77*F

#define USEC_PER_SAMPLE 5000    // Five seconds per sample
#define SAMPLES_PER_PERIOD 120  // Ten minute evaluation period for duty cycle

// --- DHT definitions ---
// Set pin and type
#define DHTPIN 3            // Which pin has the sensor
#define DHTTYPE DHT22       // DHT 22  (AM2302), AM2321
// Initialize sensor
DHT dht(DHTPIN, DHTTYPE);

int numSamples=0;
bool fanOn=false;

// --- adaptive temperature, duty cycle ---
float adapTemp = TARGET_TEMP;
int onesPerPeriod[4] = { 0 }; // Duty cycle counter per period
int period = 0;               // Counts 0..3 and repeat
int samplesThisPeriod = 0;    // Counts 0..119 and repeats
int totalOnes = 0;            // Counts only samples with fan on

// --- alert definitions ---
bool restarted   = true;
bool doorOpen    = false;
bool doorClose   = false;
bool tempAdaptUp = false;
bool tempAdaptDown = false;



// =========== The Arduino setup function ===========
void setup() {
  Serial.begin(9600);

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
    onesPerPeriod[period] += 1;
  }

// ...handle duty cycle processing...

if (samplesThisPeriod >= SAMPLES_PER_PERIOD) {
  // --- end of period; now time to reckon, adapt, and wrap ---

  // reckon the overall duty cycle
  totalOnes =  onesPerPeriod[0];
  totalOnes += onesPerPeriod[1];
  totalOnes += onesPerPeriod[2];
  totalOnes += onesPerPeriod[3];

  
  // adapt temperature if needed
  if (totalOnes > SAMPLES_PER_PERIOD && adapTemp <= NEVER_EXCEED) {
    // --- More than 25% duty cycle over 4 periods, increase temp  ---
    adapTemp += 0.1;
    tempAdaptUp = true;
  }
  else if (totalOnes < SAMPLES_PER_PERIOD/2) {
    if (adapTemp > TARGET_TEMP) {
      // --- Less than 12.5% duty cycle, if above target temp, reduce temp ---
      adapTemp -= 0.1;
      tempAdaptDown = true;
    }
  }
  // wrap the duty cycle counters
  period = (period + 1) % 4;  // Increment the period 0,1,2,3,0,1,2,3...
  samplesThisPeriod = 0;
  onesPerPeriod[period] = 0;
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

  // ...adjust the fan state...
  
  if( t<=adapTemp ) { fanOn=false; digitalWrite(LED_BUILTIN, LOW); }
  else              { fanOn=true; digitalWrite(LED_BUILTIN, HIGH); }

  // ...and finally report data.

  // alerts first

  if( restarted ) {
    Serial.println("ALERT RESTARTED ");
    restarted=false;
  }

  if( tempAdaptUp ) {
    Serial.print("ALERT ADAPT_UP ");
    Serial.print(adapTemp);
    Serial.println(" is the new temperature goal");
    tempAdaptUp=false;
  }

  if( tempAdaptDown ) {
    Serial.print("ALERT ADAPT_DN ");
    Serial.print(adapTemp);
    Serial.println(" is the new temperature goal");
    tempAdaptDown=false;
  }

  // Now the reporting data

  // fields 0/1

  if( fanOn==true ) { Serial.print("OPERATING -- "); }
  else              { Serial.print("IDLE -- "); }
  
  Serial.print(t);          // Field 2
  Serial.print(" *C ");     // Field 3
  //Serial.print(f);
  //Serial.print(" *F ");
  Serial.print(h);          // Field 4
  Serial.print(" %  ");     // Field 5
  Serial.print(adapTemp);   // Field 6
  Serial.print(" *C goal");
  Serial.println(" ");
}
