#include <DHT.h>
#define DHTPIN 8
#define DHTTYPE DHT22

const int cold_r = 9; // Pin of relay for cold light, e.g. UVB
const int hot_r = 10; // Pin of relay for hot relay, e.g. heater
const int read_delay = 300; // Seconds delay between temp readings
const int too_hot = 30; // Temp *C that is too hot
const int too_cold = 18; // Temp *C that is too cold
const int fence = 2; // Amount *C temp must move before changing. To stop flicker.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    delay(5000); // Open serial monitor
    dht.begin();
    Serial.begin(9600);
    pinMode(cold_r,OUTPUT);
    pinMode(hot_r,OUTPUT);
    pinMode(DHTPIN,INPUT_PULLUP);
    Serial.println("Test, Going LOW");
    digitalWrite(cold_r,LOW);
    digitalWrite(hot_r,LOW);
    delay(5000);
    Serial.println("Test, Going HIGH");
    digitalWrite(cold_r,HIGH);
    digitalWrite(hot_r,HIGH);
    delay(5000);
    }

void loop() {
    float t = dht.readTemperature();

    // Check if any temp reads failed and retry indefintely. This is a safety feature.
    int fail_count = 0;
    while (isnan(t)) {
        fail_count++;
        Serial.println("Failed to read from DHT sensor!");
        // Happens occasionally, so give it opportunity to retry before taking action.
        if (fail_count > 10) {
            // Do the default, which is HIGH state for both.
            // So the cold relay will be NO=>on, and the hot relay NC=>off
            // which is the strip light on and the heater off. We can loop here forever if need be.
            Serial.println("Going HIGH");
            digitalWrite(cold_r,HIGH); // strip light on
            digitalWrite(hot_r,HIGH); // supplimentary heater off
        }
        delay(5000);
        t = dht.readTemperature();
    }

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println("C ");

    // Temperature is over maximum
    if (t > too_hot) {
        digitalWrite(cold_r,LOW); // Strip light off
        digitalWrite(hot_r,HIGH); // Heater off
        Serial.println("Set cold relay LOW (strip light off) and hot relay HIGH (heater off)");

        // Wait until the temperature falls below threshold
        while ((t + fence) > too_hot) {
            Serial.print("Temp is ");
            Serial.print(t);
            Serial.print("C. Waiting for temp to fall below ");
            Serial.print(too_hot - fence);
            Serial.println("C");
            delay(read_delay*1000);
            t = dht.readTemperature();
        }
        Serial.print(" Temp normal @ ");
        Serial.print(t);
        Serial.println("C. Turning light back on");
        digitalWrite(cold_r,HIGH); // Strip light on
    }

    // Temperature is under minium
    if (t < too_cold) {
        digitalWrite(cold_r,HIGH); // Strip light on
        digitalWrite(hot_r,LOW); // Heater on
        Serial.print(t);
        Serial.println("C. Too cold. Set cold relay HIGH (strip light on) and hot relay LOW (heater on)");

        // Wait until it warms up
        while ((t - fence) < too_cold) {
            Serial.print("Temp is ");
            Serial.print(t);
            Serial.print("C. Waiting for temp to rise above ");
            Serial.print(too_cold + fence);
            Serial.println("C");
            delay(read_delay*1000);
            t = dht.readTemperature();
        }
        Serial.print(" Temp normal @ ");
        Serial.print(t);
        Serial.println("C. Turning supplimentary heater off");
        digitalWrite(hot_r,HIGH);
    }
    delay(read_delay*1000);
}
