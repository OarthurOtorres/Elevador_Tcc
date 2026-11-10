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
unsigned long tempoUltimaPassagemAndar = 0; 
bool comandoPortaEnviado = false;
bool emergenciasTratadas = false;
bool tocarSomAoAbrir = true; // Controle para silenciar o buzzer em chamadas de mesmo andar em standby
int estadoAnteriorLog = -1;

void setup() {
  inicializarMotor();
  BluetoothInit();      
  lcdInit();            
  initBtsESensores();   
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
      restaurarPortasAposEmergencia(andarAtual); // Fecha a porta suavemente ao resetar
      desligarLedReset(); 
      emergenciasTratadas = false;
      estado = 0;
      enviarLog("Emergência normalizada. Elevador pronto.");
      enviarLog("*X0"); 
    }

    lerBotoes();
    atualizarLedsBotoes();

    if (estado == 0) { // PARADO (STANDBY)
      lcdParado();

      if (estadoAnteriorLog != 0) {
        enviarLog("Elevador Parado no " + String(andarAtual) + "º Andar");
        estadoAnteriorLog = 0;
      }

      if (chamada[andarAtual] == true) {
        chamada[andarAtual] = false; 
        comandoPortaEnviado = false; 
        tocarSomAoAbrir = false; // CHAMADA NO MESMO ANDAR EM STANDBY: NÃO TOCA DING-DONG OU SIRENE
        estado = 2; 
      } else {
        andarDestino = escolherProximoAndar();
        
        if (andarDestino != 0 && andarDestino != andarAtual) {
          if (portaEstaTotalmenteFechada()) {
            estado = 1; 
          }
        }
      }

    } else if (estado == 1) { // MOVENDO
      if (!portaEstaTotalmenteFechada()) {
        pararMotor();
        return;
      }

      if (estadoAnteriorLog != 1) {
        enviarLog("Elevador em Movimento para o " + String(andarDestino) + "º Andar");
        estadoAnteriorLog = 1;
      }

      if (andarDestino > andarAtual) {
        ligarMotorSubir();
        lcdSubindo();

        if (sensorAtivo(andarAtual + 1) && (millis() - tempoUltimaPassagemAndar > 600)) {
          andarAtual++;
          tempoUltimaPassagemAndar = millis();
          enviarLog("Passando pelo " + String(andarAtual) + "º Andar");
        }
      } else if (andarDestino < andarAtual) {
        ligarMotorDescer();
        lcdDescendo();

        if (sensorAtivo(andarAtual - 1) && (millis() - tempoUltimaPassagemAndar > 600)) {
          andarAtual--;
          tempoUltimaPassagemAndar = millis();
          enviarLog("Passando pelo " + String(andarAtual) + "º Andar");
        }
      }

      // CHEGADA AO ANDAR DE DESTINO
      if ((andarAtual == andarDestino && sensorAtivo(andarDestino)) ||
          (chamada[andarAtual] && sensorAtivo(andarAtual))) {

        pararMotor();
        
        registrarViagemConcluida(); 
        enviarLog("*V" + String(totalViagensAcumuladas)); 
        
        chamada[andarAtual] = false; 
        comandoPortaEnviado = false; 
        tocarSomAoAbrir = true; // CHEGADA DE VIAGEM: TOCA DING-DONG
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
        if (tocarSomAoAbrir) {
          tocarDingDong(); // Só toca se for chegada de viagem
        }
        enviarLog("*P1"); 
        comandoPortaEnviado = true;
      }

      if (portaEstaTotalmenteFechada()) {
        enviarLog("*P0");
        chamada[andarAtual] = false; 
        estado = 0;
      }
    }

  } else { // MODO EMERGÊNCIA ATIVO
    if (emergenciasTratadas == false) {
      pararMotor();
      setSireneEmergencia(true); 
      ativarPortaEmergencia(andarAtual); // Abre a porta e mantendo-a aberta
      emergenciasTratadas = true;
      enviarLog("PERIGO: Emergência Ativada!");
      enviarLog("*X1"); 
      estadoAnteriorLog = -1;
    }

    rotinaSeguranca();
    lcdEmergencia();
    atualizarLedsBotoes();
  }
}