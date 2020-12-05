// Raumklima Checker powered by RS Calberlah and DLR_School_Lab Braunschweig
// Verwendete Bibliotheken
#include <Wire.h> // Ist eine Bibliothek für die Ansteuerung des Displays
#include <Adafruit_SSD1306.h> // Bibliothek für den Display
#include <SimpleDHT.h> // Bibliothek für den Umweltsensor 

#define DHT11_PIN 2  // Der ausgewählte Pin für den Umweltsensor

SimpleDHT11 dht11(DHT11_PIN); // Initializierung des Umweltsensors

#define SCREEN_WIDTH 128 // OLED display weite in Pixeln
#define SCREEN_HEIGHT 32 // OLED display höhe in Pixeln

#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Initializierung den Displays

// Initializierung der Sensordaten
float airQuality;
byte temperature = 0;
byte humidity = 0;

// Die Nachricht welches im Display angezeigt wird
char message[] = "powered by RS Calberlah & DLR_School_Lab_BS";
int width = 12;
int  x, minX;

bool alreadyInitialized = false;
int steps = 0;

// Programm initializierung
void setup() {
  // Validierung des Displays
  checkIfDisplayAllocationIsValid();
  Serial.begin(115200);
  // Anzeige des Displays
  display.display();
  Serial.println("The gas sensor is warming up..."); // Ausgabe in den seriellen Arduino-Monitor für Testzwecke
  delay(3000); // Verzögerung von 3 Sekunden
  x = display.width();
  minX = -12 * strlen(message);
  pinMode(9, OUTPUT); // Initializierung des LED-Pins
  pinMode(8, OUTPUT); // Initializierung des LED-Pins
  pinMode(7, OUTPUT); // Initializierung des LED-Pins
}

// Validierung des Displays
void checkIfDisplayAllocationIsValid() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed")); // Ausgabe in den seriellen Arduino-Monitor für Testzwecke
    for (;;);
  }
}

void loop() {
    // Verzögerung der Sensorausgabe um eine sinnvolle Darstellung zu ermöglichen
    if (steps == 0 || steps == 100) {
      if (steps == 0 && !alreadyInitialized || steps != 0 && alreadyInitialized) {
        // Schalter für Holung der Sensordaten und Schaltung der LEDs
        airQuality = analogRead(0);
        temperature = digitalRead(2);
        humidity = digitalRead(2);
        if (airQuality <= 90) { // Luftqualität ist gut
           digitalWrite(7, HIGH); // Schaltet grüne LED an
           digitalWrite(8, LOW); // Schaltet gelbe LED aus
           digitalWrite(9, LOW); // Schaltet rote LED aus
        } else if (airQuality > 90 && airQuality < 180) { // Luftqualität ist schlecht - Lüften nötig
           digitalWrite(7, LOW); // Schaltet grüne LED aus
           digitalWrite(8, HIGH); // Schaltet gelbe LED an
           digitalWrite(9, LOW); // Schaltet rote LED aus
        } else { // Luftqualität ist ganz schlecht - Es muss gelüftet werden
           digitalWrite(7, LOW); // Schaltet grüne LED aus
           digitalWrite(8, LOW); // Schaltet gelbe LED aus
           digitalWrite(9, HIGH); // Schaltet rote LED an
        }
        int errorCode = SimpleDHTErrSuccess;
        // Validierung des Temperatur- & Luftfeuchtigkeitssensor 
        if ((errorCode = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
          Serial.println("Read DHT11 failed, errorCode=" + String(errorCode)); // Ausgabe in den seriellen Arduino-Monitor für Testzwecke
          delay(2000); // Verzögerung von zwei Sekunden
          return;
        }
        // Zurücksetzung der Schritte 
        if (steps == 100) {
          steps = -1;
        }
    }
  }
  // Display Konfiguration
  preconfigureDisplay();
  // Ausgabe des Titels
  drawSensorTitle();
  // Ausgabe der Sensorinformationen
  drawSensorInformation(
    airQuality,
    temperature,
    humidity);
  // Anzeige der Informationen auf dem Display  
  display.display();
  if (!alreadyInitialized && steps == 0) {
    alreadyInitialized = true;
  }
  // Erhöhung des Stufenwerts
  steps++;
}

// Konfigurierung des Displays
void preconfigureDisplay() {
  display.clearDisplay(); // Löschen des Displays beim Programmstart
  display.setTextSize(1); // Setzen der Textgröße
  display.setTextWrap(false); // Verhindern einer Leerzeile
  display.setTextColor(WHITE); // Setzen der Textfarbe
  display.setCursor(0, 0); // Positionierung des Corsors
}

// Ausgabe des Programmtitels
void drawSensorTitle() {
  display.println("Raumklima-Check");
  display.setCursor(x, 7); // Positionierung des Titels
  display.println(message); // Ausgabe des Titels
  // Laufschriftkonfiguration 
  x = x - 2;
  if (--x < minX) {
    x = display.width() - 4;
  }
}

// Ausgabe der Sensor Informationen
void drawSensorInformation(
  float airQuality,
  byte temperature,
  byte humidity) {
  String airQualityMessage = "Air-Quality: ";
  String airQualityValue;
  if (airQuality <= 90) { // Luftqualität ist gut
    airQualityValue = "Gut";
  } else if (airQuality > 90 && airQuality < 180) { // Luftqualität ist schlecht - Lüften nötig
    airQualityValue = "Lueften"; 
  } else { // Luftqualität ist ganz schlecht - Es muss gelüftet werden
    airQualityValue = "Lueften!";
  }
  airQualityMessage += airQualityValue;
  // Ausgabe der Luftqualität Nachricht
  display.println(airQualityMessage);
  String roomMessage = "Temp: ";
  roomMessage += String((int)temperature);
  roomMessage += + "*C | Hum ";
  roomMessage += String((int)humidity);
  roomMessage += "%";
  // Ausgabe der Temperatur- & Luftfeuchtigkeitsnachricht
  display.println(roomMessage);
}

// Idea: Steffen Jauch, Programming: Manuel Kollus, Supported by DLR_School_Lab Brunswick
