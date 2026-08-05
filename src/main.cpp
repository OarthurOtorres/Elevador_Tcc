#include "Display.h"
#include "Emergencia.h"
#include "Logica.h"
#include "Motor.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Variável para guardar o tempo em que o elevador parou
unsigned long tempoParada = 0;

void setup() {
  inicializarMotor();
  initBtsESensores();
  lcdInit();
  EmergenciaInit();
}

void loop() {
  if (emergenciaAtivada == false) { // Se a emergência não estiver ativada, o elevador funciona normalmente
    
    if (telaEmergenciaEscrita == true) { 
      telaEmergenciaEscrita = false; // Reseta a flag para a próxima emergência
      lcd.backlight();               // Força o backlight a ficar permanentemente ligado
      lcd.clear();                   // Limpa o texto "EMERGENCIA ATIVA!" da tela
    }

    // 1. Faz a leitura contínua dos botões de chamada (NUNCA TRAVA!)
    lerBotoes();

    // 2. Máquina de Estados do Elevador
    if (estado == 0) { // --------- ESTADO 0: PARADO ---------

      // Mostra o status parado de forma contínua e limpa
      lcdParado();

      // Algoritmo SCAN decide para onde ir com base nas chamadas
      andarDestino = escolherProximoAndar();

      // Se houver alguma chamada pendente para outro andar, altera o estado
      // para se mover
      if (andarDestino != andarAtual) {
        estado = 1;
        lcd.clear(); // Limpa a tela para a transição de movimento
      }
    } else if (estado == 1) { // --------- ESTADO 1: MOVENDO ---------

      // CONTROLE DE MOVIMENTO E ATUALIZAÇÃO DE POSIÇÃO
      if (andarDestino > andarAtual) {
        ligarMotorSubir();
        lcdSubindo(); // Atualiza o display informando que está subindo

        if (sensorAtivo(andarAtual + 1)) {
          andarAtual++;
          delay(600); // Pausa curta física apenas para alinhar no sensor
        }
      } else if (andarDestino < andarAtual) {
        ligarMotorDescer();
        lcdDescendo(); // Atualiza o display informando que está descendo

        if (sensorAtivo(andarAtual - 1)) {
          andarAtual--;
          delay(600); // Pausa curta física apenas para alinhar no sensor
        }
      }

      // CONDIÇÃO DE PARADA INTELIGENTE
      if ((andarAtual == andarDestino && sensorAtivo(andarDestino)) ||
          (chamada[andarAtual] && sensorAtivo(andarAtual))) {

        pararMotor();
        chamada[andarAtual] = false; // Limpa a chamada deste andar

        lcd.clear();
        lcdChegou(); // Mostra a mensagem de "Chegou / Porta Aberta"

        tempoParada = millis(); // 🕒 Tira um "print" do tempo atual do relógio
        estado = 2;             // Muda para o estado de espera da porta aberta
      }
    } else if (estado == 2) { // --------- ESTADO 2: PORTA ABERTA (ESPERANDO) ---------

      // Enquanto o elevador espera a porta "fechar", ele CONTINUA lendo botões!
      // Se passar 2500 milissegundos (2.5 segundos)...
      if (millis() - tempoParada >= 2500) {
        lcd.clear();
        estado = 0; // Devolve o elevador para o modo PARADO, liberando para a próxima viagem
      }
    }
  } else { // Se a emergência estiver ativada, o elevador para e faz a rotina de segurança
    rotinaSeguranca();
  }
}