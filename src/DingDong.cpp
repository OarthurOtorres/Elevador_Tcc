#include "DingDong.h"
#include <Arduino.h>

#define BUZZER_PIN A3

enum EstadoDD { DD_IDLE, DD_DING, DD_DONG };
static EstadoDD estadoDD = DD_IDLE;
static unsigned long tempoDD = 0;

static bool sireneAtiva = false;
static int freqSirene = 400;
static bool subindoSirene = true;
static unsigned long tempoSirene = 0;

// Variáveis do Alerta de Obstrução da Porta
static bool alertaObstrucao = false;
static unsigned long tempoAlerta = 0;
static bool bipAlertaLigado = false;

void initDingDong() {
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);
}

void tocarDingDong() {
  if (!sireneAtiva && estadoDD == DD_IDLE) {
    estadoDD = DD_DING;
    tempoDD = millis();
    tone(BUZZER_PIN, 1318);
  }
}

void setSireneEmergencia(bool ativa) {
  sireneAtiva = ativa;
  if (!sireneAtiva) {
    noTone(BUZZER_PIN);
    freqSirene = 400;
    subindoSirene = true;
  } else {
    estadoDD = DD_IDLE;
  }
}

void setAlertaObstrucao(bool ativo) {
  if (alertaObstrucao != ativo) {
    alertaObstrucao = ativo;
    if (!alertaObstrucao && !sireneAtiva && estadoDD == DD_IDLE) {
      noTone(BUZZER_PIN);
      bipAlertaLigado = false;
    }
  }
}

void atualizarDingDong() {
  unsigned long agora = millis();

  // --- PRIORIDADE 1: SIRENE DE EMERGÊNCIA ---
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

  // --- PRIORIDADE 2: ALERTA DE OBSTRUÇÃO DA PORTA (Bip Intermitente) ---
  if (alertaObstrucao) {
    if (agora - tempoAlerta >= 250) { // Alterna a cada 250ms
      tempoAlerta = agora;
      bipAlertaLigado = !bipAlertaLigado;

      if (bipAlertaLigado) {
        tone(BUZZER_PIN, 1800); // Tom agudo de alerta
      } else {
        noTone(BUZZER_PIN);
      }
    }
    return;
  }

  // --- PRIORIDADE 3: CAMPAINHA DING-DONG ---
  switch (estadoDD) {
    case DD_DING:
      if (agora - tempoDD >= 300) {
        estadoDD = DD_DONG;
        tempoDD = agora;
        tone(BUZZER_PIN, 1046);
      }
      break;

    case DD_DONG:
      if (agora - tempoDD >= 700) {
        estadoDD = DD_IDLE;
        noTone(BUZZER_PIN);
      }
      break;

    case DD_IDLE:
    default:
      break;
  }
}