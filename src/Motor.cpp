#include "Motor.h"
#include <Arduino.h>

#define RPWM_Subir  10
#define LPWM_Descer 9

void inicializarMotor() {
  pinMode(RPWM_Subir, OUTPUT);
  pinMode(LPWM_Descer, OUTPUT);

  analogWrite(RPWM_Subir, 0);
  analogWrite(LPWM_Descer, 0);
}

void ligarMotorSubir() {
  // Desliga o sentido oposto na hora e aplica 100% no sentido correto
  analogWrite(LPWM_Descer, 0);
  analogWrite(RPWM_Subir, VELOCIDADE_MOTOR);
}

void ligarMotorDescer() {
  // Desliga o sentido oposto na hora e aplica 100% no sentido correto
  analogWrite(RPWM_Subir, 0);
  analogWrite(LPWM_Descer, VELOCIDADE_MOTOR);
}

void pararMotor() {
  // Zera ambos instantaneamente
  analogWrite(RPWM_Subir, 0);
  analogWrite(LPWM_Descer, 0);
}

void atualizarRampaMotor() {
  // Função vazia: mantida apenas para evitar erro de compilação caso esteja no seu main.cpp
}