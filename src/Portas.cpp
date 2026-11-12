#include "Portas.h"
#include "DingDong.h"
#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>

#define SENSOR_IV_PIN 8   
#define SERVO_P1 A0       
#define SERVO_P2 A1       
#define SERVO_P3 A2       

#define EEPROM_ADDR_TEMPO 0 // Endereço 0 da EEPROM para o tempo da porta

const int ANGULO_FECHADO_P1 = 5;  const int ANGULO_ABERTO_P1 = 95;
const int ANGULO_FECHADO_P2 = 0;  const int ANGULO_ABERTO_P2 = 90;
const int ANGULO_FECHADO_P3 = 5;  const int ANGULO_ABERTO_P3 = 95;

const unsigned long TEMPO_MOVIMENTO_TOTAL = 1500; 
unsigned long tempoPortaAbertaMs = 6000; // Valor padrão 6s

enum EstadoPorta { PORTA_FECHADA, PORTA_ABRINDO, PORTA_ABERTA, PORTA_FECHANDO, PORTA_EMERGENCIA };

static EstadoPorta estadoAtualPorta = PORTA_FECHADA;
static int andarAtualPorta = 1;
static unsigned long tempoInicioEstado = 0;
static unsigned long tempoDuracaoEstado = 1500;
static unsigned long tempoInicioObstrucao = 0;

static float anguloOrigemMovimento = 0;
static float anguloDestinoMovimento = 0;
static float anguloAtualServo = 0;

Servo servoP1, servoP2, servoP3;

Servo* getServoAndar(int andar) {
  if (andar == 2) return &servoP2;
  if (andar == 3) return &servoP3;
  return &servoP1;
}

int getAnguloFechado(int andar) {
  if (andar == 2) return ANGULO_FECHADO_P2;
  if (andar == 3) return ANGULO_FECHADO_P3;
  return ANGULO_FECHADO_P1;
}

int getAnguloAberto(int andar) {
  if (andar == 2) return ANGULO_ABERTO_P2;
  if (andar == 3) return ANGULO_ABERTO_P3;
  return ANGULO_ABERTO_P1;
}

bool sensorObstaculoAtivo() { return (digitalRead(SENSOR_IV_PIN) == LOW); }
bool lerSensorIR() { return sensorObstaculoAtivo(); }

void salvarTempoPorta(unsigned long tempoMs) {
  tempoPortaAbertaMs = tempoMs;
  EEPROM.put(EEPROM_ADDR_TEMPO, tempoPortaAbertaMs);
}

void inicializarPortas() {
  pinMode(SENSOR_IV_PIN, INPUT_PULLUP);

  // Lê da EEPROM o tempo salvo
  EEPROM.get(EEPROM_ADDR_TEMPO, tempoPortaAbertaMs);
  if (tempoPortaAbertaMs < 2000 || tempoPortaAbertaMs > 20000) {
    tempoPortaAbertaMs = 6000; // Restaura 6s se o valor da EEPROM for inválido
  }

  servoP1.attach(SERVO_P1, 500, 2500);
  servoP2.attach(SERVO_P2, 500, 2500);
  servoP3.attach(SERVO_P3, 500, 2500);

  servoP1.write(ANGULO_FECHADO_P1);
  servoP2.write(ANGULO_FECHADO_P2);
  servoP3.write(ANGULO_FECHADO_P3);

  anguloAtualServo = ANGULO_FECHADO_P1;
  estadoAtualPorta = PORTA_FECHADA;
}

void iniciarMovimentoPorta(int andar, float angDestino) {
  andarAtualPorta = andar;
  float cursoTotal = abs(getAnguloAberto(andar) - getAnguloFechado(andar));

  anguloOrigemMovimento = anguloAtualServo;
  anguloDestinoMovimento = angDestino;
  
  float distancia = abs(anguloDestinoMovimento - anguloOrigemMovimento);
  tempoDuracaoEstado = (cursoTotal > 0) ? (distancia / cursoTotal) * TEMPO_MOVIMENTO_TOTAL : TEMPO_MOVIMENTO_TOTAL;
  if (tempoDuracaoEstado < 50) tempoDuracaoEstado = 50;

  tempoInicioEstado = millis();
  Servo* s = getServoAndar(andarAtualPorta);
  if (!s->attached()) s->attach((andar == 1 ? SERVO_P1 : (andar == 2 ? SERVO_P2 : SERVO_P3)), 500, 2500);
}

void comandarAberturaPorta(int andar) {
  if (estadoAtualPorta != PORTA_ABRINDO && estadoAtualPorta != PORTA_ABERTA) {
    if (estadoAtualPorta == PORTA_FECHADA) anguloAtualServo = getAnguloFechado(andar);
    iniciarMovimentoPorta(andar, getAnguloAberto(andar));
    estadoAtualPorta = PORTA_ABRINDO;
    tempoInicioObstrucao = 0;
  }
}

// Controle individual de qualquer porta no Modo Manual
void comandarTogglePortaEspecifica(int andar) {
  andarAtualPorta = andar;
  if (estadoAtualPorta == PORTA_FECHADA) {
    anguloAtualServo = getAnguloFechado(andar);
    iniciarMovimentoPorta(andar, getAnguloAberto(andar));
    estadoAtualPorta = PORTA_ABRINDO;
  } else {
    iniciarMovimentoPorta(andar, getAnguloFechado(andar));
    estadoAtualPorta = PORTA_FECHANDO;
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
        anguloAtualServo = anguloDestinoMovimento;
        estadoAtualPorta = PORTA_ABERTA;
        tempoInicioEstado = agora;
      } else {
        anguloAtualServo = anguloOrigemMovimento + (anguloDestinoMovimento - anguloOrigemMovimento) * progresso;
      }
      s->write((int)anguloAtualServo);
      break;
    }

    case PORTA_ABERTA:
      if (haObstaculo) {
        tempoInicioEstado = agora;
        if (tempoInicioObstrucao == 0) tempoInicioObstrucao = agora;
        if (agora - tempoInicioObstrucao >= 4000) setAlertaObstrucao(true);
      } else {
        tempoInicioObstrucao = 0;
        setAlertaObstrucao(false);
        if (agora - tempoInicioEstado >= tempoPortaAbertaMs) { // Usa o tempo configurado!
          iniciarMovimentoPorta(andarAtualPorta, getAnguloFechado(andarAtualPorta));
          estadoAtualPorta = PORTA_FECHANDO;
          tempoInicioEstado = agora;
        }
      }
      break;

    case PORTA_FECHANDO: {
      if (haObstaculo) {
        setAlertaObstrucao(false);
        iniciarMovimentoPorta(andarAtualPorta, getAnguloAberto(andarAtualPorta));
        estadoAtualPorta = PORTA_ABRINDO;
        break;
      }
      float progresso = (float)(agora - tempoInicioEstado) / (float)tempoDuracaoEstado;
      if (progresso >= 1.0f) {
        anguloAtualServo = anguloDestinoMovimento;
        estadoAtualPorta = PORTA_FECHADA;
      } else {
        anguloAtualServo = anguloOrigemMovimento + (anguloDestinoMovimento - anguloOrigemMovimento) * progresso;
      }
      s->write((int)anguloAtualServo);
      break;
    }

    case PORTA_EMERGENCIA: {
      float progresso = (float)(agora - tempoInicioEstado) / (float)tempoDuracaoEstado;
      if (progresso >= 1.0f) anguloAtualServo = anguloDestinoMovimento;
      else anguloAtualServo = anguloOrigemMovimento + (anguloDestinoMovimento - anguloOrigemMovimento) * progresso;
      s->write((int)anguloAtualServo);
      break;
    }
  }
}

bool portaEstaTotalmenteFechada() { return (estadoAtualPorta == PORTA_FECHADA); }
bool portaEstaAberta() { return (estadoAtualPorta != PORTA_FECHADA); }
void ativarPortaEmergencia(int andar) { setAlertaObstrucao(false); iniciarMovimentoPorta(andar, getAnguloAberto(andar)); estadoAtualPorta = PORTA_EMERGENCIA; }
void restaurarPortasAposEmergencia(int andar) { setAlertaObstrucao(false); iniciarMovimentoPorta(andar, getAnguloFechado(andar)); estadoAtualPorta = PORTA_FECHANDO; }