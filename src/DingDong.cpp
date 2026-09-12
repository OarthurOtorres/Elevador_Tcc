#include "DingDong.h"
#include <Arduino.h>

#define BUZZER_PIN A3

// Estados do DingDong (Campainha de Chegada)
enum EstadoDD { DD_IDLE, DD_DING, DD_DONG };
static EstadoDD estadoDD = DD_IDLE;
static unsigned long tempoDD = 0;

// Variáveis da Sirene de Emergência
static bool sireneAtiva = false;
static int freqSirene = 400;
static bool subindoSirene = true;
static unsigned long tempoSirene = 0;

void initDingDong() {
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);
}

void tocarDingDong() {
  // Dispara a campainha apenas se a emergência estiver inativa
  if (!sireneAtiva && estadoDD == DD_IDLE) {
    estadoDD = DD_DING;
    tempoDD = millis();
    tone(BUZZER_PIN, 1318); // "DING" (Mi6)
  }
}

void setSireneEmergencia(bool ativa) {
  sireneAtiva = ativa;
  if (!sireneAtiva) {
    noTone(BUZZER_PIN);
    freqSirene = 400;
    subindoSirene = true;
  } else {
    estadoDD = DD_IDLE; // Cancela o DingDong se acionar emergência
  }
}

void atualizarDingDong() {
  unsigned long agora = millis();

  // --- PRIORIDADE 1: SIRENE DE EMERGÊNCIA (Varredura de frequência sem delay) ---
  if (sireneAtiva) {
    if (agora - tempoSirene >= 5) {
      tempoSirene = agora;
      tone(BUZZER_PIN, freqSirene);

      if (subindoSirene) {
        freqSirene += 10;
        if (freqSirene >= 1200) subindoSirene = false;
      } else {
        freqSirene -= 10;
        if (freqSirene <= 400) subindoSirene = true;
      }
    }
    return;
  }

  // --- PRIORIDADE 2: CAMPAINHA DING-DONG (Chegada ao Andar) ---
  switch (estadoDD) {
    case DD_DING:
      if (agora - tempoDD >= 300) { // 300ms em 1318Hz
        estadoDD = DD_DONG;
        tempoDD = agora;
        tone(BUZZER_PIN, 1046); // "DONG" (Dó6)
      }
      break;

    case DD_DONG:
      if (agora - tempoDD >= 700) { // 700ms em 1046Hz
        estadoDD = DD_IDLE;
        noTone(BUZZER_PIN);
      }
      break;

    case DD_IDLE:
    default:
      break;
  }
}