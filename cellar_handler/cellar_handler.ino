// By awfuchs, public domain
//
// Adapted from example testing sketch for DHT sensors by ladyada

#include "DHT.h"
#include <EEPROM.h>

// --- Master definitions ---
#define FW_VERSION 5
#define TARGET_TEMP 18      // Maintain this temp if you can
#define NEVER_EXCEED 25     // Don’t ever adapt higher than 77*F
#define MAX_DUTY_CYCLE 0.3  // Adapt temp up if exceeds this
#define MIN_DUTY_CYCLE 0.2  // Adapt down if less loaded than this

// --- DHT definitions ---
// Set pin and type
#define DHTPIN 3            // Which pin has the sensor
#define DHTTYPE DHT22       // DHT 22  (AM2302), AM2321
// Initialize sensor
DHT dht(DHTPIN, DHTTYPE);

// --- general config values ---

#define USEC_PER_SAMPLE 5000    // Five seconds per sample
#define SAMPLES_PER_PERIOD 120  // Ten minute evaluation period for duty cycle
int numSamples=0;
bool fanOn=false;

// --- adaptive temperature, duty cycle ---
float adapTemp = TARGET_TEMP;
int onesPerPeriod[4] = { 0 }; // Duty cycle counter per period
int period = 0;               // Counts 0..3 and repeat
int samplesThisPeriod = 0;    // Counts 0..119 and repeats
int totalOnes = 0;            // Counts only samples with fan on

// --- EEPROM locations ---
int EE_adapTemp = 0;
int EE_nextFree = EE_adapTemp+sizeof(adapTemp);

// --- alert definitions ---
bool restarted   = true;
bool restored    = false;
bool doorOpen    = false;
bool doorClose   = false;
bool tempAdaptUp = false;
bool tempAdaptDown = false;


// ===== Utility Functions =====

// restoreGoalTemp() attempts to read the goal temperature that was saved
// to EEPROM. Returns: true if successful. A false return means missing or
// incompatible (future idea, e.g. keyed by version) data.
//
bool restoreGoalTemp() {
  float t;
  EEPROM.get(EE_adapTemp, t);
  if( t>0 ){
    adapTemp = t;
    return true;
  }
  else {
    return false;
  }
}

// saveGoalTemp() just writes the goal temp (future: general status structure?)
// to EEPROM. Just assume that it worked.
//
void saveGoalTemp() {
  EEPROM.put(EE_adapTemp, adapTemp);
}

// initializeDcCounters() puts a nominal mid-value fake history into the
// counter array.
//
void initializeDcCounters() {
  float fakeDc   = (MAX_DUTY_CYCLE + MIN_DUTY_CYCLE) / 2;
  int fakeFanOns = SAMPLES_PER_PERIOD * fakeDc;
  onesPerPeriod[0] = fakeFanOns;
  onesPerPeriod[1] = fakeFanOns;
  onesPerPeriod[2] = fakeFanOns;
  onesPerPeriod[3] = fakeFanOns;
}

// =========== The Arduino setup function ===========
//
void setup() {
  // initialize serial, led, and sensors.
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  dht.begin();

  restored = restoreGoalTemp();
  initializeDcCounters();
}


// =========== The Arduino loop ==============
//
void loop() {
  
  float dutyCycle;
  
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
  dutyCycle = float(totalOnes) / float(SAMPLES_PER_PERIOD * 4);
  
  // adapt temperature if needed
  if ( (dutyCycle > MAX_DUTY_CYCLE) && (adapTemp <= NEVER_EXCEED) ) {
    // --- Over max duty cycle, increase goal temp a bit ---
    adapTemp += 0.1;
    tempAdaptUp = true;
    saveGoalTemp();
  }
  else if ( (dutyCycle < MIN_DUTY_CYCLE) && (adapTemp > TARGET_TEMP) ) {
    // --- Duty cycle below relaxed threshold, reduce goal temp a bit ---
    adapTemp -= 0.1;
    tempAdaptDown = true;
    saveGoalTemp();
  }
  // wrap the duty cycle counters
  period = (period + 1) % 4;  // Increment the period 0,1,2,3,0,1,2,3...
  samplesThisPeriod = 0;
  onesPerPeriod[period] = 0;
}

  //...then take readings...
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("ALERT: Failed to read from DHT sensor!");
    return;
  }

  // ...adjust the fan state...
  
  if( t<=adapTemp ) { fanOn=false; digitalWrite(LED_BUILTIN, LOW); }
  else              { fanOn=true; digitalWrite(LED_BUILTIN, HIGH); }

  // ...and finally report data.

  // alerts first

  if( restarted ) {
    Serial.print("ALERT RESTARTED FW_ver ");
    Serial.print(FW_VERSION);                       // field 3
    if( restored ) Serial.print(" restored ");      // field 4
    else Serial.print(" default ");
    Serial.print(adapTemp);                         // field 5
    Serial.println("");
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
