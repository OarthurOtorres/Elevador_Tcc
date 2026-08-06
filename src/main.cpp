#include "Display.h"
#include "Emergencia.h"
#include "Logica.h"
#include "Motor.h"
#include "Portas.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

unsigned long tempoParada = 0;
bool comandoPortaEnviado = false;

void setup() {
  inicializarMotor();
  initBtsESensores();
  lcdInit();
  EmergenciaInit();
  inicializarPortas();
}

void loop() {
  // Executa continuamente a máquina de estados das portas
  gerenciarMaquinaPortas(); 

  if (emergenciaAtivada == false) { 
    
    // Se saiu do modo de emergência, restaura o backlight e limpa o visor
    if (telaEmergenciaEscrita == true) { 
      telaEmergenciaEscrita = false; 
      lcd.backlight();               
      lcd.clear();                   
    }

    // Leitura contínua dos botões
    lerBotoes();

    // ------------------ MÁQUINA DE ESTADOS DO ELEVADOR ------------------
    if (estado == 0) { // --------- ESTADO 0: PARADO ---------

      // Atualiza o display com o status PARADO
      lcdParado();

      // 1. Checa se o botão do próprio andar onde está parado foi acionado
      if (chamada[andarAtual] == true) {
        chamada[andarAtual] = false; // Limpa a chamada imediatamente
        comandoPortaEnviado = false; // Reseta a flag de abertura da porta
        estado = 2;                  // Pula direto para Estado 2 (Porta)
      } 
      // 2. Consulta o algoritmo SCAN para ir a outros andares
      else {
        andarDestino = escolherProximoAndar();

        if (andarDestino != andarAtual) {
          estado = 1; // Vai para Estado 1 (Movimentação)
        }
      }

    } else if (estado == 1) { // --------- ESTADO 1: MOVENDO ---------

      if (andarDestino > andarAtual) {
        ligarMotorSubir();
        lcdSubindo(); // Exibe "STATUS: SUBINDO" e a rota dinâmica na linha 2

        if (sensorAtivo(andarAtual + 1)) {
          andarAtual++;
          delay(600); // Pausa física para alinhamento no sensor
        }
      } else if (andarDestino < andarAtual) {
        ligarMotorDescer();
        lcdDescendo(); // Exibe "STATUS: DESCENDO" e a rota dinâmica na linha 2

        if (sensorAtivo(andarAtual - 1)) {
          andarAtual--;
          delay(600); 
        }
      }

      // CONDIÇÃO DE PARADA INTELIGENTE (Para no destino final OU se houver chamada no caminho)
      if ((andarAtual == andarDestino && sensorAtivo(andarDestino)) ||
          (chamada[andarAtual] && sensorAtivo(andarAtual))) {

        pararMotor();
        chamada[andarAtual] = false; 
        comandoPortaEnviado = false; 
        estado = 2;                  
      }

    } else if (estado == 2) { // --------- ESTADO 2: PORTA ABERTA ---------

      // Limpa chamadas do andar atual para o botão não prender em loop
      chamada[andarAtual] = false; 

      // Atualiza o display com o status PORTAS
      lcdChegou();

      if (!comandoPortaEnviado) {
        comandarAberturaPorta(andarAtual); 
        comandoPortaEnviado = true;
      }

      // Quando o módulo 'Portas' confirmar que a porta FECHOU 100%:
      if (portaEstaTotalmenteFechada()) {
        chamada[andarAtual] = false; 
        estado = 0; // Volta para o modo PARADO para reavaliar se há mais viagens
      }
    }

  } else { 
    // Em emergência, chama a rotina de hardware e a animação de display
    rotinaSeguranca();
    lcdEmergencia();
  }
}