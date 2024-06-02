#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define  SCREEN_WIDTH 128 // ширина OLED дисплея, в пикселях
#define  SCREEN_HEIGHT 64 // высота OLED дисплея, в пикселях
#define  OLED_RESET -1 // контакт сброса # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const int sampleWindow = 50;
unsigned int sample;

#define SENSOR_PIN 35
#define PIN_QUIET 5
#define PIN_MODERATE 18
#define PIN_LOUD 19

void setup(){
  pinMode(SENSOR_PIN, INPUT); 
  pinMode(PIN_QUIET, OUTPUT);
  pinMode(PIN_MODERATE, OUTPUT);
  pinMode(PIN_LOUD, OUTPUT);
  digitalWrite(PIN_QUIET, LOW);
  digitalWrite(PIN_MODERATE, LOW);
  digitalWrite(PIN_LOUD, LOW);
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); 
 }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.display();
}

void loop()
{
  unsigned long startMillis = millis();
  float peakToPeak = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  
  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(SENSOR_PIN);
    if (sample < 1024)
      if (sample > signalMax)
        signalMax = sample;
      else if (sample < signalMin)
        signalMin = sample;
  }

  peakToPeak = signalMax - signalMin;
  // масштабирует peakToPeak. peakToPeak находится в диапазоне 0-900. А функция возвращает отображённое значение из диапазона 49-90
  int db = map(peakToPeak, 0, 900, 49, 90);
  
  Serial.print("\t");
  Serial.println(db);

  digitalWrite(PIN_QUIET, LOW);
  digitalWrite(PIN_MODERATE, LOW);
  digitalWrite(PIN_LOUD, LOW);
  
  display.clearDisplay();
  display.setCursor(0, 1);

  if (db <= 55)
  {
    display.print("Level:Quite");
    digitalWrite(PIN_QUIET, HIGH);
  }
  else if (db > 60 && db < 85)
  {
    display.print("Level:Moderate");
    digitalWrite(PIN_MODERATE, HIGH);
    }
  else if (db >= 85 && db <= 90)
  {
    display.print("Level:High");
    digitalWrite(PIN_LOUD, HIGH);
  }
  else
  {
    display.setCursor(0, 0);
    display.print("Loudness: ");
    display.print(db);
    display.print("dB");
  }

  display.display();
  delay(200);
}