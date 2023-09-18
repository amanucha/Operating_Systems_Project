#include <Arduino.h>
#include <DHT.h>
#include <pt.h>

// Sensor pins
#define DHTPIN 3
#define DHTTYPE DHT11
#define LIGHT_SENSOR A6
#define SOUND_SENSOR A2
#define LED_PIN 4
#define BUZZER_PIN 5

// Sensor objects
DHT dht(DHTPIN, DHTTYPE);
struct pt pt_dht, pt_light, pt_sound;

// Sync variable
volatile int sync_counter = 0;

// Protothread for DHT11 sensor
static int read_dht(struct pt *pt) {
  PT_BEGIN(pt);

  while (1) {
    PT_WAIT_UNTIL(pt, sync_counter % 3 == 0);
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C" );
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    sync_counter++;
    PT_YIELD(pt);
  }

  PT_END(pt);
}

// Protothread for light sensor
static int read_light(struct pt *pt) {
  PT_BEGIN(pt);

  while (1) {
    PT_WAIT_UNTIL(pt, sync_counter % 3 == 1);
    int light_value = analogRead(LIGHT_SENSOR);
    Serial.print("Light level: ");
    Serial.println(light_value);
    sync_counter++;
    PT_YIELD(pt);
  }

  PT_END(pt);
}

// Protothread for sound sensor
static int read_sound(struct pt *pt) {
  PT_BEGIN(pt);

  while (1) {
    PT_WAIT_UNTIL(pt, sync_counter % 3 == 2);
    int sound_value = analogRead(SOUND_SENSOR);
    Serial.print("Sound level: ");
    Serial.println(sound_value);
    sync_counter++;
    PT_YIELD(pt);
  }

  PT_END(pt);
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  PT_INIT(&pt_dht);
  PT_INIT(&pt_light);
  PT_INIT(&pt_sound);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  read_dht(&pt_dht);
  read_light(&pt_light);
  read_sound(&pt_sound);

  // Check if a command has been received
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove any trailing whitespace
    if (command == "LED_ON") {
      digitalWrite(LED_PIN, HIGH);
    } else if (command == "LED_OFF") {
      digitalWrite(LED_PIN, LOW);
    } else if (command == "BUZZER_ON") {
      tone(BUZZER_PIN, 1000); // generate a tone of 1000Hz
    } else if (command == "BUZZER_OFF") {
      noTone(BUZZER_PIN); // stop generating the tone
    }
}
}
