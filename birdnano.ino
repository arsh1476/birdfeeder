#include <HX711.h>
#include <Servo.h>

#define DOUT  2  // HX711 data pin
#define CLK   3  // HX711 clock pin

HX711 scale;
Servo servo;

int weightThreshold = 10;  // Minimum weight threshold to detect bird presence
int weight = 0;            // Variable to store the weight reading
unsigned long lastActionTime = 0;  // To track the 20-second delay
bool isDispensed = false;  // Flag to track if food is dispensed

void setup() {
  Serial.begin(9600);

  // Initialize HX711 load cell
  scale.begin(DOUT, CLK);
  scale.set_scale(2280.f);  // Calibration factor (adjust based on your load cell)
  scale.tare();             // Zero the scale

  // Initialize Servo
  servo.attach(10);          // Servo connected to pin 9
  servo.write(0);           // Initial position (0 degrees)

  // Give time for the servo and sensor to settle
  delay(1000);
}

void loop() {
  // Read weight from load cell
  weight = scale.get_units(5);

  // Print weight for debugging purposes
  Serial.print("Weight: ");
  Serial.println(weight);

  // Check if weight exceeds threshold (indicating bird is present)
  if (weight > weightThreshold && !isDispensed) {
    
    Serial.println("Bird detected, dispensing food according to the species...");
    delay(5000);
    // Rotate the servo to 180 degrees
    servo.write(180);  // Dispense position (180 degrees)
    lastActionTime = millis();  // Record the time when servo was rotated
    isDispensed = true;  // Mark that the food has been dispensed

  }

  // After 20 seconds, return servo to initial position
  if (isDispensed && millis() - lastActionTime >= 10000) {
    Serial.println("Returning to initial position...");
    #include <WiFiNINA.h>
#include <PubSubClient.h>
#include <HX711.h>
#include <Servo.h>

// Wi-Fi and MQTT settings
const char* ssid = "yourSSID";
const char* password = "yourPASSWORD";
const char* mqtt_server = "yourPiIP";

// HX711 Load Cell connections
#define DOUT  2  // Data pin (DT) to Arduino Pin 2
#define CLK   3  // Clock pin (SCK) to Arduino Pin 3

WiFiClient wifiClient;
PubSubClient client(wifiClient);
Servo feederServo;

// Weight Sensor and Threshold settings
HX711 scale;
const int thresholdWeight = 50;  // Define the weight threshold in grams (adjustable)
long weightDetected = 0;

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi
  connectWiFi();

  // Configure MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Connect to MQTT
  connectMQTT();

  // Attach the servo
  feederServo.attach(9);  // Use an appropriate PWM pin
  feederServo.write(0);   // Initial position of the servo (0 degrees)

  // Initialize the load cell
  scale.begin(DOUT, CLK);
  scale.set_scale();  // Set your calibration factor here
  scale.tare();       // Tare the scale to set initial reading to zero
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  // Read weight from the load cell
  weightDetected = scale.get_units(10);  // Read the average of 10 values from the load cell
  Serial.print("Weight Detected: ");
  Serial.println(weightDetected);

  // Check if the weight exceeds the threshold
  if (weightDetected > thresholdWeight) {
    Serial.println("Weight detected. Requesting species identification...");
    client.publish("birdFeeder/requestSpecies", "1");  // Request species identification from Raspberry Pi
    delay(10000);  // Wait for 10 seconds before rechecking weight (you can adjust this delay)
  }

  delay(500);  // Small delay to stabilize the readings
}

// Wi-Fi connection
void connectWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    delay(5000);
  }
  Serial.println("Connected to WiFi!");
}

// MQTT connection
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect("ArduinoClient")) {
      Serial.println("Connected to MQTT broker!");
      client.subscribe("birdFeeder/speciesDetected");  // Subscribe to species detection messages
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

// Callback to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "birdFeeder/speciesDetected") {
    Serial.println("Species detected. Rotating servo for feeding...");
    feederServo.write(180);   // Rotate servo to 180 degrees to release food
    delay(5000);             // Wait for 1 minute
    feederServo.write(0);     // Return servo to 0 degrees (original position)
  }
}

    // Rotate the servo back to 0 degrees
    servo.write(0);  // Return position (0 degrees)
    isDispensed = false;  // Reset flag after returning
  }

  delay(100);  // Small delay to prevent overwhelming the serial output
}
