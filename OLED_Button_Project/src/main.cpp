#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int BUTTON_PIN = 18;
const unsigned long DEBOUNCE_MS = 50;

Adafruit_SSD1306 display(128, 64, &Wire, -1);

int lastRawButtonState = HIGH;
int stableButtonState = HIGH;
unsigned long lastRawChangeTime = 0;
unsigned int pressCount = 0;

void showScreen()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("ESP32 Button Demo");

  display.setCursor(0, 20);
  display.print("Button: ");
  display.println(stableButtonState == LOW ? "PRESSED" : "RELEASED");

  display.setCursor(0, 40);
  display.print("Press count: ");
  display.println(pressCount);

  display.display();
}

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Start the OLED using the usual lab I2C pins:
  // SDA = GPIO 21, SCL = GPIO 22.
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED not found. Check its wiring and I2C address.");
    while (true)
    {
      // Stop here if the display could not be started.
    }
  }

  // Read the button's starting state so startup does not count as a press.
  lastRawButtonState = digitalRead(BUTTON_PIN);
  stableButtonState = lastRawButtonState;
  showScreen();

  Serial.println("Project started. Press the button to increase the count.");
}

void loop()
{
  int rawButtonState = digitalRead(BUTTON_PIN);

  // Remember when the electrical reading changes.
  if (rawButtonState != lastRawButtonState)
  {
    lastRawChangeTime = millis();
    lastRawButtonState = rawButtonState;
  }

  // Accept the new state only after it stays stable for 50 ms.
  if ((millis() - lastRawChangeTime >= DEBOUNCE_MS) &&
      (rawButtonState != stableButtonState))
  {
    stableButtonState = rawButtonState;

    // INPUT_PULLUP means a pressed button reads LOW.
    if (stableButtonState == LOW)
    {
      pressCount++;
      Serial.print("Accepted press. Count: ");
      Serial.println(pressCount);
    }

    showScreen();
  }
}