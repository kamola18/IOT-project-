#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// WiFi credentials
const char* ssid = "Baribir topolmisan";
const char* password = "Ssssssss";

// Django server URL
const char* serverURL = "http://192.168.105.220:8000/api/esp32/";

// Pin definitions
#define BUZZER_PIN 23
#define SDA_PIN 21
#define SCL_PIN 22

// BMP280 sensor
Adafruit_BMP280 bmp;

// Timing variables
unsigned long lastReading = 0;
const unsigned long readingInterval = 10000; // 10 seconds

// ESP32 ID
const String esp32Id = "ESP32_001";

void setup() {
  Serial.begin(9600);
  
  // Initialize buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Initialize BMP280
  if (!bmp.begin(0x76)) { // Try address 0x76 first
    if (!bmp.begin(0x77)) { // Try address 0x77
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      while (1) {
        // Buzz error pattern
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        delay(100);
      }
    }
  }
  
  // Configure BMP280
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode
                  Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling
                  Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
                  Adafruit_BMP280::FILTER_X16,      // Filtering
                  Adafruit_BMP280::STANDBY_MS_500); // Standby time
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Startup beep
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  
  Serial.println("ESP32 Temperature Monitor Ready!");
}
void sendDataToServer(float temperature, float pressure);
void activateBuzzer();


void loop() {
  unsigned long currentTime = millis();
  
  // Take reading every readingInterval
  if (currentTime - lastReading >= readingInterval) {
    lastReading = currentTime;
    
    // Read sensor data
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0F; // Convert Pa to hPa
    
    Serial.printf("Temperature: %.2f°C, Pressure: %.2f hPa\n", temperature, pressure);
    
    // Send data to Django server
    if (WiFi.status() == WL_CONNECTED) {
      sendDataToServer(temperature, pressure);
    } else {
      Serial.println("WiFi not connected, attempting to reconnect...");
      WiFi.reconnect();
    }
  }
  
  delay(1000); // Small delay to prevent excessive looping
}

void sendDataToServer(float temperature, float pressure) {
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON payload
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["pressure"] = pressure;
  doc["esp32_id"] = esp32Id;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.println("Sending data: " + jsonString);
  
  // Send POST request
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response: " + response);
    
    // Parse response to check if buzzer should be activated
    StaticJsonDocument<300> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);
    
    if (!error) {
      bool shouldBuzz = responseDoc["should_buzz"];
      
      if (shouldBuzz) {
        Serial.println("Temperature out of range! Activating buzzer...");
        activateBuzzer();
      } else {
        Serial.println("Temperature within normal range.");
      }
      
      // Print threshold info
      float thresholdMin = responseDoc["threshold_min"];
      float thresholdMax = responseDoc["threshold_max"];
      Serial.printf("Current thresholds: %.1f°C - %.1f°C\n", thresholdMin, thresholdMax);
    }
  } else {
    Serial.println("Error in HTTP request: " + String(httpResponseCode));
    
    // Error buzz pattern
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);
    }
  }
  
  http.end();
}

void activateBuzzer() {
  // Alert buzz pattern - 3 long beeps
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(300);
  }
}

// Function to handle WiFi reconnection
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    WiFi.reconnect();
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi reconnected!");
    } else {
      Serial.println("\nFailed to reconnect to WiFi");
    }
  }
}
