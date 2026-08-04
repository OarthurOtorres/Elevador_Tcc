#include "Motor.h"
#include <Arduino.h>

// Pinos que controlam os dois relés (Subir e Descer)
#define ReleSubir 12
#define ReleDescer 13

void inicializarMotor() {
  pinMode(ReleSubir, OUTPUT);
  pinMode(ReleDescer, OUTPUT);

  // Como é Low Level Trigger, HIGH mantém o relé DESLIGADO
  digitalWrite(ReleSubir, HIGH);
  digitalWrite(ReleDescer, HIGH);
}

void ligarMotorSubir() {
  digitalWrite(ReleDescer, HIGH); // Garante que o de descida está desligado
  digitalWrite(ReleSubir, LOW);   // LOW liga o relé de subida
}

void ligarMotorDescer() {
  digitalWrite(ReleSubir, HIGH); // Garante que o de subida está desligado
  digitalWrite(ReleDescer, LOW); // LOW liga o relé de descida
}

void pararMotor() {
  // Desliga ambos os relés enviando HIGH
  digitalWrite(ReleSubir, HIGH);
  digitalWrite(ReleDescer, HIGH);
}