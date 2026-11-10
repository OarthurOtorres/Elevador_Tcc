#include "Portas.h"
#include "DingDong.h"
#include <Arduino.h>
#include <Servo.h>

#define SENSOR_IV_PIN 8   // Pino do Sensor IR de Presença

#define SERVO_P1 A0       // Servo Porta 1º Andar
#define SERVO_P2 A1       // Servo Porta 2º Andar
#define SERVO_P3 A2       // Servo Porta 3º Andar

// CONFIGURAÇÃO DOS ÂNGULOS INDIVIDUAIS DE CADA PORTA
const int ANGULO_FECHADO_P1 = 5;
const int ANGULO_ABERTO_P1  = 95;

const int ANGULO_FECHADO_P2 = 0;   // Ajustado para 0, 90
const int ANGULO_ABERTO_P2  = 90;

const int ANGULO_FECHADO_P3 = 5;
const int ANGULO_ABERTO_P3  = 95;

const unsigned long TEMPO_MOVIMENTO_TOTAL = 1500; // Tempo total de percurso completo (ms)
const unsigned long TEMPO_PORTA_ABERTA     = 6000; // Tempo da porta aberta (ms)

enum EstadoPorta {
  PORTA_FECHADA,
  PORTA_ABRINDO,
  PORTA_ABERTA,
  PORTA_FECHANDO,
  PORTA_EMERGENCIA
};

static EstadoPorta estadoAtualPorta = PORTA_FECHADA;
static int andarAtualPorta = 1;
static unsigned long tempoInicioEstado = 0;
static unsigned long tempoDuracaoEstado = 1500;
static unsigned long tempoInicioObstrucao = 0;

static float anguloOrigemMovimento = 0;
static float anguloDestinoMovimento = 0;
static float anguloAtualServo = 0;

Servo servoP1;
Servo servoP2;
Servo servoP3;

Servo* getServoAndar(int andar) {
  if (andar == 1) return &servoP1;
  if (andar == 2) return &servoP2;
  if (andar == 3) return &servoP3;
  return &servoP1;
}

int getAnguloFechado(int andar) {
  if (andar == 1) return ANGULO_FECHADO_P1;
  if (andar == 2) return ANGULO_FECHADO_P2;
  if (andar == 3) return ANGULO_FECHADO_P3;
  return ANGULO_FECHADO_P1;
}

int getAnguloAberto(int andar) {
  if (andar == 1) return ANGULO_ABERTO_P1;
  if (andar == 2) return ANGULO_ABERTO_P2;
  if (andar == 3) return ANGULO_ABERTO_P3;
  return ANGULO_ABERTO_P1;
}

bool sensorObstaculoAtivo() {
  return (digitalRead(SENSOR_IV_PIN) == LOW);
}

bool lerSensorIR() {
  return sensorObstaculoAtivo();
}

void inicializarPortas() {
  pinMode(SENSOR_IV_PIN, INPUT_PULLUP);

  servoP1.attach(SERVO_P1, 500, 2500);
  servoP2.attach(SERVO_P2, 500, 2500);
  servoP3.attach(SERVO_P3, 500, 2500);

  servoP1.write(ANGULO_FECHADO_P1);
  servoP2.write(ANGULO_FECHADO_P2);
  servoP3.write(ANGULO_FECHADO_P3);

  anguloAtualServo = ANGULO_FECHADO_P1;
  estadoAtualPorta = PORTA_FECHADA;
}

// Inicia um movimento suave sem saltos a partir da posição atual do servo
void iniciarMovimentoPorta(int andar, float angDestino) {
  andarAtualPorta = andar;
  float angFechado = getAnguloFechado(andar);
  float angAberto = getAnguloAberto(andar);
  float cursoTotal = abs(angAberto - angFechado);

  anguloOrigemMovimento = anguloAtualServo;
  anguloDestinoMovimento = angDestino;
  
  float distancia = abs(anguloDestinoMovimento - anguloOrigemMovimento);
  if (cursoTotal > 0) {
    tempoDuracaoEstado = (distancia / cursoTotal) * TEMPO_MOVIMENTO_TOTAL;
  } else {
    tempoDuracaoEstado = TEMPO_MOVIMENTO_TOTAL;
  }
  if (tempoDuracaoEstado < 50) tempoDuracaoEstado = 50;

  tempoInicioEstado = millis();

  Servo* s = getServoAndar(andarAtualPorta);
  if (!s->attached()) {
    int pino = (andar == 1 ? SERVO_P1 : (andar == 2 ? SERVO_P2 : SERVO_P3));
    s->attach(pino, 500, 2500);
  }
}

void comandarAberturaPorta(int andar) {
  if (estadoAtualPorta != PORTA_ABRINDO && estadoAtualPorta != PORTA_ABERTA) {
    if (estadoAtualPorta == PORTA_FECHADA) {
      anguloAtualServo = getAnguloFechado(andar);
    }
    iniciarMovimentoPorta(andar, getAnguloAberto(andar));
    estadoAtualPorta = PORTA_ABRINDO;
    tempoInicioObstrucao = 0;
  }
}

void gerenciarMaquinaPortas() {
  unsigned long agora = millis();
  bool haObstaculo = sensorObstaculoAtivo();
  Servo* s = getServoAndar(andarAtualPorta);

  switch (estadoAtualPorta) {
    
    case PORTA_FECHADA:
      setAlertaObstrucao(false);
      tempoInicioObstrucao = 0;
      break;

    case PORTA_ABRINDO: {
      float progresso = (float)(agora - tempoInicioEstado) / (float)tempoDuracaoEstado;
      if (progresso >= 1.0f) {
        progresso = 1.0f;
        anguloAtualServo = anguloDestinoMovimento;
        estadoAtualPorta = PORTA_ABERTA;
        tempoInicioEstado = agora;
        tempoInicioObstrucao = 0;
      } else {
        anguloAtualServo = anguloOrigemMovimento + (anguloDestinoMovimento - anguloOrigemMovimento) * progresso;
      }
      s->write((int)anguloAtualServo);
      break;
    }

    case PORTA_ABERTA:
      if (haObstaculo) {
        tempoInicioEstado = agora; // Reinicia temporizador da porta aberta

        if (tempoInicioObstrucao == 0) {
          tempoInicioObstrucao = agora;
        }

        if (agora - tempoInicioObstrucao >= 4000) {
          setAlertaObstrucao(true);
        }
      } else {
        tempoInicioObstrucao = 0;
        setAlertaObstrucao(false);

        if (agora - tempoInicioEstado >= TEMPO_PORTA_ABERTA) {
          iniciarMovimentoPorta(andarAtualPorta, getAnguloFechado(andarAtualPorta));
          estadoAtualPorta = PORTA_FECHANDO;
          tempoInicioEstado = agora;
        }
      }
      break;

    case PORTA_FECHANDO: {
      if (haObstaculo) { // Obstáculo detectado durante fechamento: Reabre suavemente sem dar tranco
        setAlertaObstrucao(false);
        iniciarMovimentoPorta(andarAtualPorta, getAnguloAberto(andarAtualPorta));
        estadoAtualPorta = PORTA_ABRINDO;
        tempoInicioObstrucao = agora;
        break;
      }

      float progresso = (float)(agora - tempoInicioEstado) / (float)tempoDuracaoEstado;
      if (progresso >= 1.0f) {
        progresso = 1.0f;
        anguloAtualServo = anguloDestinoMovimento;
        estadoAtualPorta = PORTA_FECHADA;
        setAlertaObstrucao(false);
        tempoInicioObstrucao = 0;
      } else {
        anguloAtualServo = anguloOrigemMovimento + (anguloDestinoMovimento - anguloOrigemMovimento) * progresso;
      }
      s->write((int)anguloAtualServo);
      break;
    }

    case PORTA_EMERGENCIA: {
      // Abre suavemente até a posição aberta e se mantém aberta até o Reset
      float progresso = (float)(agora - tempoInicioEstado) / (float)tempoDuracaoEstado;
      if (progresso >= 1.0f) {
        progresso = 1.0f;
        anguloAtualServo = anguloDestinoMovimento;
      } else {
        anguloAtualServo = anguloOrigemMovimento + (anguloDestinoMovimento - anguloOrigemMovimento) * progresso;
      }
      s->write((int)anguloAtualServo);
      break;
    }
  }
}

bool portaEstaTotalmenteFechada() {
  return (estadoAtualPorta == PORTA_FECHADA);
}

bool portaEstaAberta() {
  return (estadoAtualPorta != PORTA_FECHADA);
}

void ativarPortaEmergencia(int andar) {
  setAlertaObstrucao(false);
  iniciarMovimentoPorta(andar, getAnguloAberto(andar));
  estadoAtualPorta = PORTA_EMERGENCIA;
}

void restaurarPortasAposEmergencia(int andar) {
  setAlertaObstrucao(false);
  iniciarMovimentoPorta(andar, getAnguloFechado(andar));
  estadoAtualPorta = PORTA_FECHANDO;
}