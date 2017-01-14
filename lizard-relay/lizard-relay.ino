#include <DHT.h>
#define DHTPIN 8
#define DHTTYPE DHT22

const int cold_r = 9; // Pin of relay for cold light, e.g. UVB
const int hot_r = 10; // Pin of relay for hot relay, e.g. heater
const unsigned long read_delay = 60; // Seconds delay between temp readings
const int too_hot = 30; // Temp *C that is too hot
const int too_cold = 18; // Temp *C that is too cold
const int fence = 2; // Amount *C temp must move before changing. To stop flicker.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    delay(5000); // Open serial monitor
    dht.begin();
    Serial.begin(9600);
    Serial.println();
    Serial.print("Thresholds: too_cold=");
    Serial.print(too_cold);
    Serial.print("C too_hot=");
    Serial.print(too_hot);
    Serial.print("C fence=");
    Serial.print(fence);
    Serial.print("C read_delay=");
    Serial.print(read_delay);
    Serial.println("s");
    pinMode(cold_r,OUTPUT);
    pinMode(hot_r,OUTPUT);
    pinMode(DHTPIN,INPUT_PULLUP);
    Serial.println("Test, Going LOW");
    digitalWrite(cold_r,LOW);           // Strip light off
    digitalWrite(hot_r,LOW);            // Heater on
    delay(5000);
    Serial.println("Test, Going HIGH"); // The default state when unpowered or broken
    digitalWrite(cold_r,HIGH);          // Strip light on
    digitalWrite(hot_r,HIGH);           // Heater off
    delay(5000);
    }

void loop() {
    float t = get_temp_wait();
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println("C ");

    // Temperature is over maximum
    if (t > too_hot) {
        digitalWrite(cold_r,LOW); // Strip light off
        digitalWrite(hot_r,HIGH); // Heater off
        Serial.println("Too hot. Set cold relay LOW (strip light off) and hot relay HIGH (heater off)");

        // Wait until the temperature falls below threshold
        while ((t + fence) > too_hot) {
            Serial.print("Temp is ");
            Serial.print(t);
            Serial.print("C. Waiting for temp to fall below ");
            Serial.print(too_hot - fence);
            Serial.print("C. ");
            delay(read_delay*1000);
            t = get_temp_wait();
            // get_temp_wait can turn the strip light on if it gets over ten bad readings. Make sure it is off.
            Serial.println("Set cold_r LOW (strip light off)");
            digitalWrite(cold_r,LOW);
        }
        Serial.print("Temp no longer too hot @ ");
        Serial.print(t);
        Serial.println("C. Turning strip light back on");
        digitalWrite(cold_r,HIGH);
    }

    // Temperature is under minium
    if (t < too_cold) {
        digitalWrite(cold_r,HIGH); // Strip light on
        digitalWrite(hot_r,LOW); // Heater on
        Serial.print(t);
        Serial.println("C is too cold. Set cold relay HIGH (strip light on) and hot relay LOW (heater on)");

        // Wait until it warms up
        while ((t - fence) < too_cold) {
            Serial.print("Temp is ");
            Serial.print(t);
            Serial.print("C. Waiting for temp to rise above ");
            Serial.print(too_cold + fence);
            Serial.print("C. ");
            delay(read_delay*1000);
            t = get_temp_wait();
            //  get_temp_wait can turn the heater off after ten bad readings. Make sure it is on.
            Serial.println("Set hot_r LOW (heater on)");
            digitalWrite(hot_r,LOW);
        }
        Serial.print("Temp normal @ ");
        Serial.print(t);
        Serial.println("C. Turning supplimentary heater off");
        digitalWrite(hot_r,HIGH);

        // Skip the long delay. We know it is no longer too cold, but it might be now too hot. Back the beginning to make sure.
        delay(5000);
        return;
    }
    delay(read_delay*1000);
}

// Function to always get a temperature reading. The sensor occasionally fails and returns nan, which should be retried until it works
// If the sensor fails completely, the relays should go into their unpowered state of HIGH, which is the UV light being NO=>on and the 
// supplementary heater being NC=>off.
float get_temp_wait() {
    float my_temp = dht.readTemperature();
    int my_fail_count = 0;
    while (isnan(my_temp)) {
        my_fail_count++;
        Serial.println("Failed to read from DHT sensor!");
        // Happens occasionally, so give it opportunity to retry before taking action.
        if (my_fail_count > 10) {
            // Do the default, which is HIGH state for both.
            // So the cold relay will be NO=>on, and the hot relay NC=>off
            // which is the strip light on and the heater off. We can loop here forever if need be.
            Serial.println("Going HIGH, strip light on and heater off");
            digitalWrite(cold_r,HIGH); // strip light on
            digitalWrite(hot_r,HIGH); // supplimentary heater off
        }
        delay(5000); // Minimum read interval is 2 seconds, so wait 5.
        my_temp = dht.readTemperature();
    }
    return my_temp;
}
