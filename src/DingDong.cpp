#include "DingDong.h"
#include <Arduino.h>

#define BUZZER_PIN A3

enum EstadoDD { DD_IDLE, DD_DING, DD_DONG };
static EstadoDD estadoDD = DD_IDLE;
static unsigned long tempoDD = 0;

static bool sireneAtiva = false;
static unsigned long tempoSirene = 0;
static bool alternadorTomSirene = false;

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
    alternadorTomSirene = false;
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

  // PRIORIDADE 1: SIRENE DE EMERGÊNCIA (ALARME DE INCÊNDIO INDUSTIAL - TOM DUPLO INTERMITENTE)
  if (sireneAtiva) {
    if (agora - tempoSirene >= 120) {
      tempoSirene = agora;
      alternadorTomSirene = !alternadorTomSirene;
      if (alternadorTomSirene) {
        tone(BUZZER_PIN, 1000); // Frequência Alta
      } else {
        tone(BUZZER_PIN, 850);  // Frequência Baixa
      }
    }
    return;
  }

  // PRIORIDADE 2: ALERTA DE OBSTRUÇÃO DA PORTA
  if (alertaObstrucao) {
    if (agora - tempoAlerta >= 250) {
      tempoAlerta = agora;
      bipAlertaLigado = !bipAlertaLigado;

      if (bipAlertaLigado) {
        tone(BUZZER_PIN, 1800);
      } else {
        noTone(BUZZER_PIN);
      }
    }
    return;
  }

  // PRIORIDADE 3: CAMPAINHA DING-DONG
  switch (estadoDD) {
    case DD_DING:
      if (agora - tempoDD >= 350) {
        estadoDD = DD_DONG;
        tempoDD = agora;
        tone(BUZZER_PIN, 1046);
      }
      break;

    case DD_DONG:
      if (agora - tempoDD >= 650) {
        estadoDD = DD_IDLE;
        noTone(BUZZER_PIN);
      }
      break;

    case DD_IDLE:
    default:
      break;
  }
}