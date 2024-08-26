#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Blynk Auth Token
char auth[] = "Your_Blynk_Auth_Token";

// WiFi credentials
char ssid[] = "Your_SSID";
char pass[] = "Your_PASSWORD";

// Pin Definitions
#define DHTPIN 4          // DHT22 sensor pin
#define DHTTYPE DHT22     // DHT 22 (AM2302)
#define RELAY_PIN 16      // Relay pin to control appliances
#define CURRENT_SENSOR_PIN A0  // Current sensor pin (ACS712)

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Variables
float currentReading = 0.0;
float temperature = 0.0;
float humidity = 0.0;

BlynkTimer timer;

// Function to read current sensor
float getCurrentReading() {
  int analogValue = analogRead(CURRENT_SENSOR_PIN);
  float voltage = analogValue * (5.0 / 1023.0);  // Convert to voltage
  float current = (voltage - 2.5) / 0.066;       // Calculate current (ACS712 30A)
  return abs(current);
}

// Function to read DHT sensor
void readDHTSensor() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send readings to Blynk
  Blynk.virtualWrite(V5, temperature);
  Blynk.virtualWrite(V6, humidity);
  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
}

// Function to control appliances via Blynk
BLYNK_WRITE(V1) {
  int relayState = param.asInt();
  digitalWrite(RELAY_PIN, relayState);
}

// Function to monitor energy usage
void monitorEnergyUsage() {
  currentReading = getCurrentReading();
  
  // Send current reading to Blynk
  Blynk.virtualWrite(V7, currentReading);
  
  Serial.print("Current Reading: ");
  Serial.print(currentReading);
  Serial.println(" A");
}

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Initialize Relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Start with relay off

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Setup Timers
  timer.setInterval(2000L, readDHTSensor);       // Read DHT sensor every 2 seconds
  timer.setInterval(1000L, monitorEnergyUsage);  // Monitor energy usage every 1 second
}

void loop() {
  Blynk.run();  // Run Blynk
  timer.run();  // Run timer
}
