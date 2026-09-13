#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <DHT.h>


LiquidCrystal_I2C lcd1(0x27, 16, 2);
LiquidCrystal_I2C lcd2(0x26, 16, 2);


#define DHTTYPE DHT11
#define DHTPIN PA0
DHT dht(DHTPIN, DHTTYPE);


#define SOIL_SENSOR PA6
#define LIGHT_SENSOR PA5
#define RELAY_FAN PA4
#define RELAY_LIGHT PA3
#define RELAY_PUMP PA2


#define BTN_EDIT PB12
#define BTN_RST PB13
#define BTN_SAVE PB14
#define BTN_UP PB15
#define BTN_DOWN PA8
#define BTN_NEXT PA9
#define BTN_PREV PA10


#define BTN_TOGGLE_PUMP PB3
#define BTN_TOGGLE_LIGHT PB4
#define BTN_TOGGLE_FAN PB5


uint8_t thresholds[4] = {40, 35, 85, 30};
int curParam = 0;


bool isEditing = false;
bool isPumpOn = false;
bool isLightOn = false;
bool isFanOn = false;

#define DEBOUNCE_DELAY 50


void saveThresholdsToEEPROM() {
  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, thresholds[i]);
  }
}

void loadThresholdsFromEEPROM() {
  for (int i = 0; i < 4; i++) {
    thresholds[i] = EEPROM.read(i);

    if (thresholds[i] > 100)
    {
      thresholds[i] = (i == 0) ? 40 : (i == 1) ? 35 : (i == 2) ? 85 : 30;
    }
  }
}

void setup() {

  dht.begin();

  lcd1.init();
  lcd1.backlight();
  lcd1.setCursor(0, 0);
  lcd1.print("He thong");
  lcd1.setCursor(0, 1);
  lcd1.print("Tuoi cay");
  delay(3000);
  lcd1.clear();

  lcd2.init();
  lcd2.backlight();
  lcd2.setCursor(0, 0);
  lcd2.print("Cai dat");
  lcd2.setCursor(0, 0);
  delay(3000);
  lcd2.clear();

  lcd2.print("SM<");
  lcd2.print(thresholds[0]);
  lcd2.print(" T>");
  lcd2.print(thresholds[1]);
  lcd2.setCursor(0, 1);
  lcd2.print("Hu>");
  lcd2.print(thresholds[2]);
  lcd2.print(" L<");
  lcd2.print(thresholds[3]);


  pinMode(SOIL_SENSOR, INPUT);
  pinMode(LIGHT_SENSOR, INPUT);
  pinMode(RELAY_PUMP, OUTPUT);
  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_SAVE, INPUT_PULLUP);
  pinMode(BTN_RST, INPUT_PULLUP);
  pinMode(BTN_EDIT, INPUT_PULLUP);
  pinMode(BTN_TOGGLE_PUMP, INPUT_PULLUP);
  pinMode(BTN_TOGGLE_LIGHT, INPUT_PULLUP);
  pinMode(BTN_TOGGLE_FAN, INPUT_PULLUP);


  loadThresholdsFromEEPROM();

}


bool debounce(int buttonIndex, int pin)
{
  unsigned long lastButtonPressTime[10] = {0};
  unsigned long currentTime = millis();
  if (digitalRead(pin) == LOW && currentTime - lastButtonPressTime[buttonIndex] > DEBOUNCE_DELAY)
   {

      lastButtonPressTime[buttonIndex] = currentTime;
      return true;

  }
  return false;
}

void loop() {
  int soilMoisture = map(analogRead(SOIL_SENSOR), 0, 4095, 100, 0);
  int lightLevel = map(analogRead(LIGHT_SENSOR), 0, 4095, 100, 0);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();


  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("SM:");
  lcd1.print(soilMoisture);
  lcd1.print("% L:");
  lcd1.print(lightLevel);
  lcd1.print("%");

  lcd1.setCursor(0, 1);
  lcd1.print("T:");
  lcd1.print(temperature);
  lcd1.print("C Hu:");
  lcd1.print(humidity);
  lcd1.print("%");


  if (debounce(0, BTN_EDIT))
  {
    isEditing = true;
    curParam = 0;
  }

    if (debounce(1, BTN_UP)) {
      thresholds[curParam]++;
    }
    if (debounce(2, BTN_DOWN)) {
      thresholds[curParam]--;
    }
    if (debounce(3, BTN_NEXT)) {
      curParam = (curParam + 1) % 4;
    }
    if (debounce(4, BTN_PREV)) {
      curParam = (curParam - 1 + 4) % 4;
    }
    if (debounce(5, BTN_SAVE)) {
      saveThresholdsToEEPROM();
      isEditing = false;
      lcd2.clear();
      lcd2.print("Luu cai dat...");
      delay(1000);

      if (isEditing) {
    lcd2.clear();
    switch (curParam) {
      case 0:
        lcd2.print("SM<");
        lcd2.print(thresholds[0]);
        break;
      case 1:
        lcd2.print("T>");
        lcd2.print(thresholds[1]);
        break;
      case 2:
        lcd2.print("Hu>");
        lcd2.print(thresholds[2]);
        break;
      case 3:
        lcd2.print("L<");
        lcd2.print(thresholds[3]);
        break;
    }


      lcd2.clear();
      lcd2.setCursor(0, 0);
      lcd2.print("SM<");
      lcd2.print(thresholds[0]);
      lcd2.print(" T>");
      lcd2.print(thresholds[1]);
      lcd2.setCursor(0, 1);
      lcd2.print("Hu>");
      lcd2.print(thresholds[2]);
      lcd2.print(" L<");
      lcd2.print(thresholds[3]);
      delay(3000);
    }
  }


  if (debounce(6, BTN_RST)) {
    thresholds[0] = 40;
    thresholds[1] = 35;
    thresholds[2] = 85;
    thresholds[3] = 30;

    saveThresholdsToEEPROM();

    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print("Da cai dat ve");
    lcd2.setCursor(0, 1);
    lcd2.print("che do mac dinh");
    delay(1000);


    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print("SM<");
    lcd2.print(thresholds[0]);
    lcd2.print(" T>");
    lcd2.print(thresholds[1]);
    lcd2.setCursor(0, 1);
    lcd2.print("Hu>");
    lcd2.print(thresholds[2]);
    lcd2.print(" L<");
    lcd2.print(thresholds[3]);
    delay(3000);
  }


  if (debounce(7, BTN_TOGGLE_PUMP)) {
    isPumpOn = !isPumpOn;
    digitalWrite(RELAY_PUMP, isPumpOn ? HIGH : LOW);
  }


  if (debounce(8, BTN_TOGGLE_LIGHT)) {
    isLightOn = !isLightOn;
    digitalWrite(RELAY_LIGHT, isLightOn ? HIGH : LOW);
  }


    if (debounce(9, BTN_TOGGLE_FAN)) {
    isFanOn = !isFanOn;
    digitalWrite(RELAY_FAN, isFanOn ? HIGH : LOW);
  }


  if (!isPumpOn) {
    if (soilMoisture <= thresholds[0]) digitalWrite(RELAY_PUMP, HIGH);
    else digitalWrite(RELAY_PUMP, LOW);
  }

  if (!isLightOn) {
    if (lightLevel < thresholds[3]) digitalWrite(RELAY_LIGHT, HIGH);
    else digitalWrite(RELAY_LIGHT, LOW);
  }

  if (!isFanOn) {
    if (temperature > thresholds[1] || humidity > thresholds[2]) digitalWrite(RELAY_FAN, HIGH);
    else digitalWrite(RELAY_FAN, LOW);
  }

  delay(500 );
}
