#include "Emergencia.h"
#include "Display.h"
#include "Motor.h"
#include "DingDong.h"
#include <Arduino.h>

#define BtEmergencia 2
#define BtReset 3

volatile bool emergenciaAtivada = false;

volatile unsigned long ultimoTempoEmergencia = 0;
volatile unsigned long ultimoTempoReset = 0;
const unsigned long TEMPO_DEBOUNCE = 150;

void EmergenciaInit() {
  pinMode(BtEmergencia, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BtEmergencia), ParadaEmergencia, FALLING);
  
  pinMode(BtReset, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BtReset), ResetEmergencia, FALLING);
}

void ParadaEmergencia() { 
  unsigned long tempoAtual = millis();
  if (tempoAtual - ultimoTempoEmergencia > TEMPO_DEBOUNCE) {
    if (digitalRead(BtEmergencia) == LOW) { 
      emergenciaAtivada = true; 
      ultimoTempoEmergencia = tempoAtual;
    }
  }
}

bool tentarResetarSistema() {
  if (digitalRead(BtEmergencia) == LOW) { 
    return false;
  }
  
  emergenciaAtivada = false;
  Serial.println("E:0");
  return true;
}

void ResetEmergencia() {
  unsigned long tempoAtual = millis();
  if (tempoAtual - ultimoTempoReset > TEMPO_DEBOUNCE) {
    if (digitalRead(BtReset) == LOW) { 
      tentarResetarSistema();
      ultimoTempoReset = tempoAtual;
    }
  }
}

void rotinaSeguranca() {
  pararMotor();              
  setSireneEmergencia(true);  
  lcdEmergencia();           
  Serial.println("E:1"); 
}