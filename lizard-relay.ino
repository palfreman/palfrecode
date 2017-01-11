#include <DHT.h>
#define DHTPIN 8
#define DHTTYPE DHT22
//const int t_sense = 8; // Temperature sensor pin
const int cold_r = 9; // Pin of relay for cold light, e.g. UVB
const int hot_r = 10; // Pin of relay for hot relay, e.g. heater
const int read_delay = 15; // Seconds delay between temp readings
const int too_hot = 27; // Temp *C that is too hot
const int too_cold = 24; // Temp *C that is too cold
const int fence = 2; // Amount *C temp must move before changing. To stop flicker.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
// put your setup code here, to run once:
  delay(5000); // Open serial monitor
  dht.begin();
  Serial.begin(9600);
  pinMode(cold_r,OUTPUT);
  pinMode(hot_r,OUTPUT);
  Serial.println("Test, Going LOW");
  digitalWrite(cold_r,LOW);
  digitalWrite(hot_r,LOW);
  delay(2000);
  Serial.println("Test, Going HIGH");
  digitalWrite(cold_r,HIGH);
  digitalWrite(hot_r,HIGH);
  delay(2000);
}

void loop() {
// put your main code here, to run repeatedly:
  float t = dht.readTemperature();

// Check if any temp reads failed and retry indefintely. This is a safety feature.
//if (isnan(h) || isnan(t) || isnan(f)) {
  int fail_count = 0;
  while (isnan(t)) {
    fail_count++;
    Serial.println("Failed to read from DHT sensor!");
    // Happens occasionally, so give it opportunity to retry before giving up.
    if (fail_count > 10) {
      // Do the default, which is of off state for both, so go HIGH
      // So the cold relay will be NO=>on, and the hot relay NC=>off
      // which is the strip light on and the heater off. We can loop here forever if need be.
      Serial.println("Going HIGH");
      digitalWrite(cold_r,HIGH);      // strip light on
      digitalWrite(hot_r,HIGH);       // supplimentary heater off
    }
    delay(5000);
    t = dht.readTemperature();
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

  // Temperature is over maximum
  if (t > too_hot) {
    digitalWrite(cold_r,LOW);       // Strip light off
    digitalWrite(hot_r,HIGH);       // Heater off
    Serial.println("Set cold relay LOW (strip light off) and hot relay HIGH (heater off)");
  }
  // Temperature is under minium
  if (t < too_cold) {
    digitalWrite(cold_r,HIGH);     // Strip light on
    digitalWrite(hot_r,LOW);       // Heater on
    Serial.println("Set cold relay HIGH (strip light on) and hot relay LOW (heater on)");
  }
  // Temp is normal and more than a fixed amount from trigger point so it doesn't flicker
  if ((t < (too_hot - fence)) || (t > (too_cold + fence))) {
    digitalWrite(cold_r,HIGH);      // Strip light on
    digitalWrite(hot_r,HIGH);       // Heater off
    Serial.println("Temp normal. Set cold relay HIGH (strip light on) and hot relay HIGH (heater off)");
  }
delay(read_delay*1000);
}
