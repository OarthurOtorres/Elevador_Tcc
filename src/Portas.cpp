#include "Portas.h"

// Pinos dos Servos (Ajuste se no seu circuito estiver diferente)
#define PIN_SERVO1 A0
#define PIN_SERVO2 A1
#define PIN_SERVO3 A2

Servo servo1;
Servo servo2;
Servo servo3;

// Estados: 0=Fechada, 1=Abrindo, 2=Aberta, 3=Fechando
int estadoPorta = 0; 
unsigned long tempoInicioEstado = 0;
int andarServoAtivo = 0;
bool cicloPortaOcupado = false;

void inicializarPortas() {
  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);
  servo3.attach(PIN_SERVO3);

  // Garante portas fechadas no início
  servo1.write(ANGULO_FECHADO);
  servo2.write(ANGULO_FECHADO);
  servo3.write(ANGULO_FECHADO);
  
  estadoPorta = 0;
  cicloPortaOcupado = false;
}

void comandarAberturaPorta(int andar) {
  andarServoAtivo = andar;
  estadoPorta = 1; // Inicia estado 'Abrindo'
  cicloPortaOcupado = true;
  tempoInicioEstado = millis();

  // ENVIA O COMANDO DIRETO PRO SERVO GIRAR AGORA!
  if (andarServoAtivo == 1)      servo1.write(ANGULO_ABERTO);
  else if (andarServoAtivo == 2) servo2.write(ANGULO_ABERTO);
  else if (andarServoAtivo == 3) servo3.write(ANGULO_ABERTO);
}

void gerenciarMaquinaPortas() {
  if (estadoPorta == 0) return; // Se está fechada, não faz nada

  unsigned long tempoAtual = millis();

  // Estado 1: Abrindo (espera 1s pro servo físico girar até 90 graus)
  if (estadoPorta == 1) {
    if (tempoAtual - tempoInicioEstado >= 1000) {
      estadoPorta = 2; // Passa pro estado de esperar aberta
      tempoInicioEstado = tempoAtual;
    }
  }
  // Estado 2: Espera com a porta aberta (aguarda os 3s da constante)
  else if (estadoPorta == 2) {
    if (tempoAtual - tempoInicioEstado >= TEMPO_PORTA_ABERTA) {
      estadoPorta = 3; // Inicia o fechamento
      tempoInicioEstado = tempoAtual;

      // COMANDO DIRETO PRO SERVO FECHAR!
      if (andarServoAtivo == 1)      servo1.write(ANGULO_FECHADO);
      else if (andarServoAtivo == 2) servo2.write(ANGULO_FECHADO);
      else if (andarServoAtivo == 3) servo3.write(ANGULO_FECHADO);
    }
  }
  // Estado 3: Fechando (espera 1s pro servo físico voltar pra 0 graus)
  else if (estadoPorta == 3) {
    if (tempoAtual - tempoInicioEstado >= 1000) {
      estadoPorta = 0; // Terminou todo o ciclo!
    }
  }
}

// Retorna 'true' para o main.cpp apenas quando O CICLO INTEIRO terminar
bool portaEstaTotalmenteFechada() {
  if (cicloPortaOcupado && estadoPorta == 0) {
    cicloPortaOcupado = false; // Libera para a próxima viagem
    return true;
  }
  return false;
}