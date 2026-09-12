#include "Emergencia.h"
#include "Display.h"
#include "Motor.h"
#include "DingDong.h"
#include <Arduino.h>

#define BtEmergencia 2
#define BtReset 3

volatile bool emergenciaAtivada = false;

void EmergenciaInit() {
  pinMode(BtEmergencia, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BtEmergencia), ParadaEmergencia, FALLING);
  pinMode(BtReset, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BtReset), ResetEmergencia, FALLING);
}

void ParadaEmergencia() { 
  emergenciaAtivada = true; 
}

void ResetEmergencia() {
  if (digitalRead(BtEmergencia)) { 
    emergenciaAtivada = false;
  }
}

void rotinaSeguranca() {
  pararMotor();             // Para o motor imediatamente
  setSireneEmergencia(true); // Dispara a sirene do buzzer
  lcdEmergencia();          // Atualiza o display para mostrar emergência
}