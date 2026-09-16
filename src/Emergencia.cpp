#include "Emergencia.h"
#include "Display.h"
#include "Motor.h"
#include "DingDong.h"
#include <Arduino.h>

#define BtEmergencia 2
#define BtReset 3

volatile bool emergenciaAtivada = false;

// Variáveis para filtro de ruído (Debounce) dentro das interrupções
volatile unsigned long ultimoTempoEmergencia = 0;
volatile unsigned long ultimoTempoReset = 0;
const unsigned long TEMPO_DEBOUNCE = 150; // Tempo mínimo em ms entre disparos

void EmergenciaInit() {
  pinMode(BtEmergencia, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BtEmergencia), ParadaEmergencia, FALLING);
  
  pinMode(BtReset, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BtReset), ResetEmergencia, FALLING);
}

void ParadaEmergencia() { 
  unsigned long tempoAtual = millis();
  
  // FILTRO ANTI-RUÍDO DO MOTOR:
  // Só aceita o disparo se passaram mais de 150ms desde o último ruído/interrupção
  if (tempoAtual - ultimoTempoEmergencia > TEMPO_DEBOUNCE) {
    emergenciaAtivada = true; 
    ultimoTempoEmergencia = tempoAtual;
  }
}

void ResetEmergencia() {
  unsigned long tempoAtual = millis();
  
  if (tempoAtual - ultimoTempoReset > TEMPO_DEBOUNCE) {
    // Confirma se o botão de emergência REALMENTE não está pressionado (HIGH = Solto)
    if (digitalRead(BtEmergencia) == HIGH) { 
      emergenciaAtivada = false;
      Serial.println("E:0"); // Avisa o supervisório que voltou ao Normal
    }
    ultimoTempoReset = tempoAtual;
  }
}

void rotinaSeguranca() {
  pararMotor();              // Para o motor imediatamente
  setSireneEmergencia(true);  // Dispara a sirene do buzzer
  lcdEmergencia();           // Atualiza o display para mostrar emergência
  
  // AVISA O SUPERVISÓRIO WEB QUE ENTROU EM EMERGÊNCIA
  Serial.println("E:1"); 
}