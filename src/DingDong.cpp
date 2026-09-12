#include "DingDong.h"
#include <Arduino.h>

#define buzzerPin A3 // Pino do buzzer


void DingDong() {
  // "DING" - Agudo e impactante (Mi6 / 1318 Hz)
  tone(buzzerPin, 1318);
  delay(300);

  // "DONG" - Grave e prolongado (Dó6 / 1046 Hz)
  tone(buzzerPin, 1046);
  delay(700);

  // Desliga para encerrar o som
  noTone(buzzerPin);
}
