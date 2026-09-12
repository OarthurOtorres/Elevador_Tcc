#include "Portas.h"
#include "DingDong.h"
#include <Arduino.h>
#include <Servo.h>

// Definir o pino do Sensor IR (Módulo FC-51 / TCRT5000)
#define SENSOR_IV_PIN 8

// Pinos dos Servos das Portas (Ajuste se seus pinos forem diferentes)
#define SERVO_P1 11
#define SERVO_P2 12
#define SERVO_P3 13

// Ângulos dos Servos
#define ANGULO_FECHADO 0
#define ANGULO_ABERTO  90

// Estados da Porta
enum EstadoPorta {
  PORTA_FECHADA,
  PORTA_ABRINDO,
  PORTA_ABERTA,
  PORTA_FECHANDO
};

static EstadoPorta estadoAtualPorta = PORTA_FECHADA;
static int andarAtualPorta = 1;
static unsigned long tempoInicioEstado = 0;
static unsigned long tempoInicioObstrucao = 0;
static bool obstruindoAnterior = false;

Servo servoP1;
Servo servoP2;
Servo servoP3;

// Módulo FC-51: Saída LOW (0) significa OBSTÁCULO DETECTADO
bool sensorObstaculoAtivo() {
  return (digitalRead(SENSOR_IV_PIN) == LOW);
}

Servo* getServoAndar(int andar) {
  if (andar == 1) return &servoP1;
  if (andar == 2) return &servoP2;
  if (andar == 3) return &servoP3;
  return &servoP1;
}

void inicializarPortas() {
  pinMode(SENSOR_IV_PIN, INPUT);

  servoP1.attach(SERVO_P1);
  servoP2.attach(SERVO_P2);
  servoP3.attach(SERVO_P3);

  servoP1.write(ANGULO_FECHADO);
  servoP2.write(ANGULO_FECHADO);
  servoP3.write(ANGULO_FECHADO);

  estadoAtualPorta = PORTA_FECHADA;
}

void comandarAberturaPorta(int andar) {
  andarAtualPorta = andar;
  estadoAtualPorta = PORTA_ABRINDO;
  tempoInicioEstado = millis();

  Servo* s = getServoAndar(andarAtualPorta);
  if (!s->attached()) s->attach(andar == 1 ? SERVO_P1 : (andar == 2 ? SERVO_P2 : SERVO_P3));
  s->write(ANGULO_ABERTO);
}

void gerenciarMaquinaPortas() {
  unsigned long agora = millis();
  bool haObstaculo = sensorObstaculoAtivo();

  switch (estadoAtualPorta) {
    
    case PORTA_FECHADA:
      setAlertaObstrucao(false);
      break;

    case PORTA_ABRINDO:
      // Aguarda tempo do movimento do servo (1 segundo para abrir)
      if (agora - tempoInicioEstado >= 1000) {
        estadoAtualPorta = PORTA_ABERTA;
        tempoInicioEstado = agora;
        tempoInicioObstrucao = agora;
      }
      break;

    case PORTA_ABERTA:
      if (haObstaculo) {
        // Reinicia o tempo de espera de fechamento enquanto alguém estiver na porta
        tempoInicioEstado = agora; 

        // Se o obstáculo persistir por mais de 4 segundos, dispara alerta sonoro
        if (agora - tempoInicioObstrucao >= 4000) {
          setAlertaObstrucao(true);
        }
      } else {
        // Sem obstáculo: reseta o tempo de obstrução e desliga alerta
        tempoInicioObstrucao = agora;
        setAlertaObstrucao(false);

        // Após 3 segundos sem ninguém na porta, começa a fechar
        if (agora - tempoInicioEstado >= 3000) {
          estadoAtualPorta = PORTA_FECHANDO;
          tempoInicioEstado = agora;

          Servo* s = getServoAndar(andarAtualPorta);
          s->write(ANGULO_FECHADO);
        }
      }
      break;

    case PORTA_FECHANDO:
      // REABERTURA INSTANTÂNEA ANTI-ESMAGAMENTO:
      if (haObstaculo) {
        // Alguém colocou a mão/corpo durante o fechamento!
        // Inverte a porta imediatamente para ABRINDO
        estadoAtualPorta = PORTA_ABRINDO;
        tempoInicioEstado = agora;
        tempoInicioObstrucao = agora;

        Servo* s = getServoAndar(andarAtualPorta);
        s->write(ANGULO_ABERTO);
        break;
      }

      // Conclui o fechamento após 1 segundo
      if (agora - tempoInicioEstado >= 1000) {
        estadoAtualPorta = PORTA_FECHADA;
        setAlertaObstrucao(false);
      }
      break;
  }
}

bool portaEstaTotalmenteFechada() {
  return (estadoAtualPorta == PORTA_FECHADA);
}

void ativarPortaEmergencia(int andar) {
  setAlertaObstrucao(false);
  Servo* s = getServoAndar(andar);
  s->write(ANGULO_ABERTO); // Deixa a porta aberta na emergência para evacuação
  delay(500);
  s->detach(); // Desativa o servo para poder abrir manualmente se preciso
}

void restaurarPortasAposEmergencia(int andar) {
  Servo* s = getServoAndar(andar);
  s->attach(andar == 1 ? SERVO_P1 : (andar == 2 ? SERVO_P2 : SERVO_P3));
  s->write(ANGULO_FECHADO);
  estadoAtualPorta = PORTA_FECHADA;
}