#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;
DateTime now;

int button = 9;
int pirPin = 7;
int buzzer = 11;

// Alarm time
int set_hour = 7;
int set_minute = 0;

void setup() {
  Wire.begin();
  rtc.begin();

  pinMode(button, INPUT_PULLUP);
  pinMode(pirPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //rtc.adjust(DateTime(2025, 6, 5, 22, 12, 0)); // Uncomment this ONCE to set RTC time
}

void alarm(int minute_start) {
  bool button_pressed = false;
  bool awake = false;

  // Sound buzzer until button is pressed
  while (!button_pressed) {
    for (int i = 0; i < 4; i++) {
      tone(buzzer, 523, 50);
      delay(50);
      tone(buzzer, 784, 50);
      delay(50);

      for (int j = 0; j < 20; j++) {
        if (digitalRead(button) == LOW) {
          button_pressed = true;
          awake = true;
          now = rtc.now();
          minute_start = now.minute();
        }
        delay(65);
      }
    }
  }

  while (awake) {
    bool motion = false;
    for (int i = 0; i < 30; i++) {
      if (digitalRead(pirPin)) motion = true;
      delay(100);
    }

    digitalWrite(LED_BUILTIN, motion);

    if (!motion) {
      delay(5000);
      for (int i = 0; i < 40; i++) {
        if (digitalRead(pirPin)) motion = true;
        delay(100);
      }

      if (!motion) {
        alarm(minute_start); // reset alarm if no motion
        return;
      }
    }

    now = rtc.now();
    if (abs(now.minute() - minute_start) >= 1) {
      tone(buzzer, 698, 50);
      delay(100);
      tone(buzzer, 698, 50);
      digitalWrite(LED_BUILTIN, LOW);
      awake = false;
    }
  }
}

void loop() {
  now = rtc.now();
  if (now.hour() == set_hour && now.minute() == set_minute) {
    alarm(now.minute());
    delay(60000); // prevent multiple triggers
  }

  delay(10000);
}
