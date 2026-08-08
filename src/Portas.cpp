#include "Portas.h"
#include "Logica.h" // Incluído para reconhecer a função sensorAtivo()

// Pinos dos Servos nas entradas analógicas A0, A1 e A2
#define PIN_SERVO1 A0
#define PIN_SERVO2 A1
#define PIN_SERVO3 A2

Servo servo1;
Servo servo2;
Servo servo3;

int estadoPorta = 0; // 0=Fechada, 1=Abrindo, 2=Aberta, 3=Fechando
unsigned long tempoInicioEstado = 0;
int andarServoAtivo = 0;
bool cicloPortaOcupado = false;

void inicializarPortas() {
  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);
  servo3.attach(PIN_SERVO3);

  servo1.write(ANGULO_FECHADO);
  servo2.write(ANGULO_FECHADO);
  servo3.write(ANGULO_FECHADO);
  
  estadoPorta = 0;
  cicloPortaOcupado = false;
}

void comandarAberturaPorta(int andar) {
  andarServoAtivo = andar;
  estadoPorta = 1;
  cicloPortaOcupado = true;
  tempoInicioEstado = millis();

  if (andarServoAtivo == 1) {
    servo1.attach(PIN_SERVO1);
    servo1.write(ANGULO_ABERTO);
  } else if (andarServoAtivo == 2) {
    servo2.attach(PIN_SERVO2);
    servo2.write(ANGULO_ABERTO);
  } else if (andarServoAtivo == 3) {
    servo3.attach(PIN_SERVO3);
    servo3.write(ANGULO_ABERTO);
  }
}

void gerenciarMaquinaPortas() {
  if (estadoPorta == 0) return;

  unsigned long tempoAtual = millis();

  // Estado 1: Abrindo
  if (estadoPorta == 1) {
    if (tempoAtual - tempoInicioEstado >= 1000) {
      estadoPorta = 2;
      tempoInicioEstado = tempoAtual;
    }
  }
  // Estado 2: Aberta
  else if (estadoPorta == 2) {
    if (tempoAtual - tempoInicioEstado >= TEMPO_PORTA_ABERTA) {
      estadoPorta = 3;
      tempoInicioEstado = tempoAtual;

      if (andarServoAtivo == 1)      servo1.write(ANGULO_FECHADO);
      else if (andarServoAtivo == 2) servo2.write(ANGULO_FECHADO);
      else if (andarServoAtivo == 3) servo3.write(ANGULO_FECHADO);
    }
  }
  // Estado 3: Fechando
  else if (estadoPorta == 3) {
    if (tempoAtual - tempoInicioEstado >= 1000) {
      estadoPorta = 0;
    }
  }
}

bool portaEstaTotalmenteFechada() {
  if (cicloPortaOcupado && estadoPorta == 0) {
    cicloPortaOcupado = false;
    return true;
  }
  return false;
}

// ---------------- FUNÇÕES DE EMERGÊNCIA ----------------

// Função de emergência com verificação do sensor do andar
void ativarPortaEmergencia(int andarAtual) {
  
  // Só abre a porta se o elevador estiver realmente parado no sensor do andar
  if (sensorAtivo(andarAtual)) {
    if (andarAtual == 1) {
      servo1.attach(PIN_SERVO1);
      servo1.write(ANGULO_ABERTO);
    } else if (andarAtual == 2) {
      servo2.attach(PIN_SERVO2);
      servo2.write(ANGULO_ABERTO);
    } else if (andarAtual == 3) {
      servo3.attach(PIN_SERVO3);
      servo3.write(ANGULO_ABERTO);
    }
    
    delay(500); // Dá tempo físico para o motor mover
  } 
  else {
    // Se parou no meio do caminho (sem sensor ativo): MANTÉM TODAS FECHADAS!
    servo1.attach(PIN_SERVO1);
    servo2.attach(PIN_SERVO2);
    servo3.attach(PIN_SERVO3);

    servo1.write(ANGULO_FECHADO);
    servo2.write(ANGULO_FECHADO);
    servo3.write(ANGULO_FECHADO);

    delay(500);
  }

  // Corta o sinal elétrico dos servos
  servo1.detach();
  servo2.detach();
  servo3.detach();

  estadoPorta = 0;
  cicloPortaOcupado = false;
}

// Fecha todas as portas com segurança ao restaurar do modo de emergência
void restaurarPortasAposEmergencia(int andarAtual) {
  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);
  servo3.attach(PIN_SERVO3);

  servo1.write(ANGULO_FECHADO);
  servo2.write(ANGULO_FECHADO);
  servo3.write(ANGULO_FECHADO);

  delay(1000); // Aguarda o movimento físico de fechamento

  estadoPorta = 0;
  cicloPortaOcupado = false;
}