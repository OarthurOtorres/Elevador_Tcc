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
bool emergenciasTratadas = false; // Flag para transição de emergência

void setup() {
  inicializarMotor();
  initBtsESensores();
  lcdInit();
  EmergenciaInit();
  inicializarPortas();
}

void loop() {
  gerenciarMaquinaPortas(); 

  if (emergenciaAtivada == false) { 
    
    // SE ACABOU DE SAIR DA EMERGÊNCIA:
    if (emergenciasTratadas == true) { 
      pararMotor();                       // Garante motor parado
      restaurarPortasAposEmergencia(andarAtual); // Reativa os servos e FECHA as portas
      telaEmergenciaEscrita = false; 
      lcd.backlight();               
      lcd.clear();                   
      emergenciasTratadas = false;       // Reseta a flag de emergência
      estado = 0;                        // Volta para o estado PARADO
    }

    lerBotoes();

    // ------------------ MÁQUINA DE ESTADOS DO ELEVADOR ------------------
    if (estado == 0) { // --------- PARADO ---------
      lcdParado();

      if (chamada[andarAtual] == true) {
        chamada[andarAtual] = false; 
        comandoPortaEnviado = false; 
        estado = 2;                  
      } else {
        andarDestino = escolherProximoAndar();
        if (andarDestino != andarAtual) {
          estado = 1; 
        }
      }

    } else if (estado == 1) { // --------- MOVENDO ---------
      if (andarDestino > andarAtual) {
        ligarMotorSubir();
        lcdSubindo();

        if (sensorAtivo(andarAtual + 1)) {
          andarAtual++;
          delay(600); 
        }
      } else if (andarDestino < andarAtual) {
        ligarMotorDescer();
        lcdDescendo();

        if (sensorAtivo(andarAtual - 1)) {
          andarAtual--;
          delay(600); 
        }
      }

      if ((andarAtual == andarDestino && sensorAtivo(andarDestino)) ||
          (chamada[andarAtual] && sensorAtivo(andarAtual))) {

        pararMotor();
        chamada[andarAtual] = false; 
        comandoPortaEnviado = false; 
        estado = 2;                  
      }

    } else if (estado == 2) { // --------- PORTA ABERTA ---------
      chamada[andarAtual] = false; 
      lcdChegou();

      if (!comandoPortaEnviado) {
        comandarAberturaPorta(andarAtual); 
        comandoPortaEnviado = true;
      }

      if (portaEstaTotalmenteFechada()) {
        chamada[andarAtual] = false; 
        estado = 0; 
      }
    }

  } else { 
    // ------------------ MODO EMERGÊNCIA ATIVO ------------------
    
    if (emergenciasTratadas == false) {
      pararMotor();                       // Interrompe a subida/descida
      ativarPortaEmergencia(andarAtual); // Abre a porta se estiver no andar e desativa os servos
      emergenciasTratadas = true;         // Executa a transição apenas uma vez
    }

    rotinaSeguranca();
    lcdEmergencia();
  }
}