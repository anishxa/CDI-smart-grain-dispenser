#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Pin configuration for ultrasonic sensor
const int trigPin = D1;
const int echoPin = D2;

// Pin configuration for LCD display
const int lcdColumns = 16;
const int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Pin configuration for potentiometer
const int potPin = A0;

// Constants for distance calculation
const float soundSpeed = 343.2;  // Speed of sound in m/s
const int containerHeight = 30;  // Height of the container in cm

// WiFi credentials
char ssid[] = "YourWiFiSSID";
char password[] = "YourWiFiPassword";

// Blynk authentication token
char auth[] = "YourAuthToken";

// Phone number for SMS alert
char phoneNumber[] = "+1234567890";

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.begin(lcdColumns, lcdRows);
  lcd.clear();
  lcd.print("Status:");

  Serial.begin(9600);

  Blynk.begin(auth, ssid, password);
}

void loop() {
  Blynk.run();
}

BLYNK_WRITE(V1) {
  // Read the potentiometer value
  int potValue = analogRead(potPin);

  // Map the potentiometer value to the container height
  int targetHeight = map(potValue, 0, 1023, 0, containerHeight);

  // Measure the distance using the ultrasonic sensor
  float distance = measureDistance();

  // Determine the status based on the measured distance
  String status;
  if (distance >= targetHeight) {
    status = "Full";
  } else {
    status = "Empty";
  }

  // Display the status on the LCD
  lcd.setCursor(0, 1);
  lcd.print("          ");  // Clear the previous status
  lcd.setCursor(0, 1);
  lcd.print(status);

  // Update the Blynk virtual pin with the status
  Blynk.virtualWrite(V1, status);

  // Send SMS alert
  String smsMessage = "Grain dispenser is " + status;
  Blynk.notify(smsMessage);
  Blynk.sms(phoneNumber, smsMessage);

  // Send email alert
  String emailSubject = "Grain Dispenser Alert";
  String emailMessage = "The grain dispenser is " + status + ".";
  Blynk.email("YourEmail@gmail.com", emailSubject, emailMessage);
}

float measureDistance() {
  // Send a 10us pulse to trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo duration and calculate the distance
  float duration = pulseIn(echoPin, HIGH);
  float distance = (duration / 2) * (soundSpeed / 10000);  // Convert to cm

  return distance;
}
