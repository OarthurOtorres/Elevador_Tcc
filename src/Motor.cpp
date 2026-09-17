#include "Motor.h"
#include <Arduino.h>

#define RPWM_Subir  10
#define LPWM_Descer 9

int sentidoMotor = 0; // Estado inicial: Parado

void inicializarMotor() {
  pinMode(RPWM_Subir, OUTPUT);
  pinMode(LPWM_Descer, OUTPUT);

  analogWrite(RPWM_Subir, 0);
  analogWrite(LPWM_Descer, 0);
  sentidoMotor = 0;
}

void ligarMotorSubir() {
  analogWrite(LPWM_Descer, 0);
  analogWrite(RPWM_Subir, VELOCIDADE_MOTOR);
  sentidoMotor = 1; // Subindo
}

void ligarMotorDescer() {
  analogWrite(RPWM_Subir, 0);
  analogWrite(LPWM_Descer, VELOCIDADE_MOTOR);
  sentidoMotor = 2; // Descendo
}

void pararMotor() {
  analogWrite(RPWM_Subir, 0);
  analogWrite(LPWM_Descer, 0);
  sentidoMotor = 0; // Parado
}

void atualizarRampaMotor() {
  // Controle de rampa se necessário
}