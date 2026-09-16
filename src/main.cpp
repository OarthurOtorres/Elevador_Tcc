#include "Bluetooth.h"
#include "Display.h"
#include "Emergencia.h"
#include "Logica.h"
#include "Motor.h"
#include "Portas.h"
#include "DingDong.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

unsigned long tempoParada = 0;
bool comandoPortaEnviado = false;
bool emergenciasTratadas = false;
int estadoAnteriorLog = -1;

void setup() {
  inicializarMotor();
  BluetoothInit();      // 1. Inicializa o Bluetooth primeiro para os logs do Homing
  lcdInit();            // 2. Inicializa o LCD antes do Homing para exibir "REFERENC"
  initBtsESensores();   // 3. Agora o Homing pode usar a tela LCD e o Bluetooth
  EmergenciaInit();
  inicializarPortas();
  initDingDong();
}

void loop() {
  atualizarDingDong();
  atualizarRampaMotor();
  
  lerComandosBluetooth();
  atualizarInterfaceBluetooth();

  gerenciarMaquinaPortas(); 

  if (emergenciaAtivada == false) { 
    
    if (emergenciasTratadas == true) { 
      pararMotor();
      setSireneEmergencia(false);
      restaurarPortasAposEmergencia(andarAtual);
      desligarLedReset(); // Já zera a telaEmergenciaEscrita internamente no Display.cpp
      emergenciasTratadas = false;
      estado = 0;
      enviarLog("Emergência normalizada. Elevador pronto.");
    }

    lerBotoes();
    atualizarLedsBotoes();

    if (estado == 0) { // PARADO
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
        
        if (andarDestino != 0 && andarDestino != andarAtual) {
          estado = 1; 
        }
      }

    } else if (estado == 1) { // MOVENDO
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

    } else if (estado == 2) { // PORTA ABERTA
      if (estadoAnteriorLog != 2) {
        enviarLog("Chegou ao " + String(andarAtual) + "º Andar - Abrindo Porta");
        estadoAnteriorLog = 2;
      }

      chamada[andarAtual] = false; 
      lcdChegou();

      if (!comandoPortaEnviado) {
        comandarAberturaPorta(andarAtual); 
        tocarDingDong();
        comandoPortaEnviado = true;
      }

      if (portaEstaTotalmenteFechada()) {
        chamada[andarAtual] = false; 
        estado = 0;
      }
    }

  } else { // MODO EMERGÊNCIA ATIVO
    if (emergenciasTratadas == false) {
      pararMotor();
      ativarPortaEmergencia(andarAtual);
      emergenciasTratadas = true;
      enviarLog("PERIGO: Emergência Ativada!");
      estadoAnteriorLog = -1;
    }

    rotinaSeguranca();
    lcdEmergencia();
    atualizarLedsBotoes();
  }
}