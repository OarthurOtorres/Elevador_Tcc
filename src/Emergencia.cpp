#include "Emergencia.h"
#include "Display.h"
#include "Motor.h"
#include <Arduino.h>

// Pino do botão de emergência (INPUT_PULLUP)
#define BtEmergencia 2
#define BtReset 3

// Variável para armazenar o estado do botão de emergência
volatile bool emergenciaAtivada = false;

// PinMode do Botão de Emergência e reset falha
void EmergenciaInit() {
  pinMode(BtEmergencia, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BtEmergencia), ParadaEmergencia, FALLING);
  pinMode(BtReset, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BtReset), ResetEmergencia, FALLING);
}

// Funcão chamada quando o botão de emergência é pressionado, ativando a variável de emergência
void ParadaEmergencia() { emergenciaAtivada = true; }

// Funcão chamada quando o botão de reset é pressionado, desativando a variável de emergência
void ResetEmergencia() {
    if(digitalRead(BtEmergencia)) { // Verifica se o botão de emergência ainda está pressionado, pra poder resetar a emergência
        emergenciaAtivada = false;
    }
    
}

// Rotina de segurança que é chamada quando a emergência é ativada
void rotinaSeguranca() {
  pararMotor();    // Para o motor imediatamente
  // adicionar a função que faz o servo ficar solto (Servo.detach(pino))
  lcdEmergencia(); // Atualiza o display para mostrar que a emergência está ativa
}
