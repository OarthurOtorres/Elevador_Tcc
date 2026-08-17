#include "Bluetooth.h"
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
int estadoAnteriorLog = -1;

void setup() {
  inicializarMotor();
  initBtsESensores();
  lcdInit();
  EmergenciaInit();
  inicializarPortas();
  BluetoothInit(); // Inicializa comunicação HC-05
}

void loop() {
  // Processa entrada e saída de dados do Bluetooth
  lerComandosBluetooth();
  atualizarInterfaceBluetooth();

  gerenciarMaquinaPortas(); 

  if (emergenciaAtivada == false) { 
    
    // SE ACABOU DE SAIR DA EMERGÊNCIA:
    if (emergenciasTratadas == true) { 
      pararMotor();                               // Garante motor parado
      restaurarPortasAposEmergencia(andarAtual); // Reativa os servos e FECHA as portas
      telaEmergenciaEscrita = false; 
      lcd.backlight();               
      lcd.clear();                   
      emergenciasTratadas = false;       // Reseta a flag de emergência
      estado = 0;                        // Volta para o estado PARADO
      enviarLog("Emergência normalizada. Elevador pronto.");
    }

    lerBotoes();

    // ------------------ MÁQUINA DE ESTADOS DO ELEVADOR ------------------
    if (estado == 0) { // --------- PARADO ---------
      lcdParado();

      if (estadoAnteriorLog != 0) {
        enviarLog("Elevador Parado no " + String(andarAtual) + "º Andar");
        estadoAnteriorLog = 0;
      }

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
      if (estadoAnteriorLog != 1) {
        enviarLog("Elevador em Movimento para o " + String(andarDestino) + "º Andar");
        estadoAnteriorLog = 1;
      }

      if (andarDestino > andarAtual) {
        ligarMotorSubir();
        lcdSubindo();

        if (sensorAtivo(andarAtual + 1)) {
          andarAtual++;
          enviarLog("Passando pelo " + String(andarAtual) + "º Andar");
          delay(600); 
        }
      } else if (andarDestino < andarAtual) {
        ligarMotorDescer();
        lcdDescendo();

        if (sensorAtivo(andarAtual - 1)) {
          andarAtual--;
          enviarLog("Passando pelo " + String(andarAtual) + "º Andar");
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
      if (estadoAnteriorLog != 2) {
        enviarLog("Chegou ao " + String(andarAtual) + "º Andar - Abrindo Porta");
        estadoAnteriorLog = 2;
      }

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
      pararMotor();                               // Interrompe a subida/descida
      ativarPortaEmergencia(andarAtual); // Abre a porta se estiver no andar e desativa os servos
      emergenciasTratadas = true;         // Executa a transição apenas uma vez
      enviarLog("PERIGO: Emergência Ativada!");
      estadoAnteriorLog = -1;
    }

    rotinaSeguranca();
    lcdEmergencia();
  }
}