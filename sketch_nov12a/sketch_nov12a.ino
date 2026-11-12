const int buttonPin = 2;
const int RLedPin = 9;
const int GLedPin = 10;
const int BLedPin = 11;

const unsigned long longPressInterval = 2000;
const unsigned long debounceInterval = 25;
const unsigned long blinkInterval = 500;
const unsigned long fadeInterval = 10;
const int fadeAmount = 2;

// 依照課堂的 RGB 模組：0 最亮，255 熄滅
const byte colors[5][3] = {
  {0,   0,   0},    // 白
  {0,   255, 255},  // 紅
  {255, 0,   255},  // 綠
  {255, 255, 0},    // 藍
  {0,   0,   255}   // 黃
};

int lightNum = 0;
int currentMode = 0;  // 0 恆亮、1 閃爍、2 呼吸

int lastReading = LOW;
int buttonState = LOW;
unsigned long readingChangedAt = 0;
unsigned long pressingTime = 0;

unsigned long blinkTimer = 0;
bool blinkOn = true;

unsigned long fadeTimer = 0;
int fadeValue = 0;
int fadeDirection = 1;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(RLedPin, OUTPUT);
  pinMode(GLedPin, OUTPUT);
  pinMode(BLedPin, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  checkButton(now);
  updateLEDColor(now);
}

void checkButton(unsigned long now) {
  int reading = digitalRead(buttonPin);

  if (reading != lastReading) {
    lastReading = reading;
    readingChangedAt = now;
  }

  // 等讀值穩定，避免按一次卻被算成好幾次
  if (now - readingChangedAt < debounceInterval ||
      reading == buttonState) {
    return;
  }

  buttonState = reading;

  if (buttonState == HIGH) {
    pressingTime = now;
  } else if (now - pressingTime < longPressInterval) {
    changeLEDColor();
  } else {
    changeMode(now);
  }
}

void changeLEDColor() {
  lightNum = (lightNum + 1) % 5;
}

void changeMode(unsigned long now) {
  currentMode = (currentMode + 1) % 3;

  blinkOn = true;
  blinkTimer = now;

  fadeValue = 0;
  fadeDirection = 1;
  fadeTimer = now;
}

void updateLEDColor(unsigned long now) {
  int r = colors[lightNum][0];
  int g = colors[lightNum][1];
  int b = colors[lightNum][2];

  if (currentMode == 1) {
    if (now - blinkTimer >= blinkInterval) {
      blinkTimer = now;
      blinkOn = !blinkOn;
    }

    if (!blinkOn) {
      r = g = b = 255;
    }
  } else if (currentMode == 2) {
    if (now - fadeTimer >= fadeInterval) {
      fadeTimer = now;
      fadeValue += fadeDirection * fadeAmount;

      if (fadeValue >= 255) {
        fadeValue = 255;
        fadeDirection = -1;
      } else if (fadeValue <= 0) {
        fadeValue = 0;
        fadeDirection = 1;
      }
    }

    // 只調暗目前顏色會用到的燈，其他燈維持熄滅
    if (r == 0) r = fadeValue;
    if (g == 0) g = fadeValue;
    if (b == 0) b = fadeValue;
  }

  setRGBLEDColor(r, g, b);
}

void setRGBLEDColor(int r, int g, int b) {
  analogWrite(RLedPin, r);
  analogWrite(GLedPin, g);
  analogWrite(BLedPin, b);
}