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
bool portaAbertaNaEmergencia = false;
bool tocarSomAoAbrir = true;
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
      if (portaAbertaNaEmergencia) {
        restaurarPortasAposEmergencia(andarAtual);
        portaAbertaNaEmergencia = false;
      }
      desligarLedReset(); 
      emergenciasTratadas = false;
      estado = 0;
      estadoAnteriorLog = -1;

      if (referenciandoAtivo) {
        iniciarReferenciamento(); // Reinicia timer e retoma o referenciamento interrompido
        enviarLog(F("Emergência normalizada. Continuando referenciamento..."));
      } else {
        enviarLog(F("Emergência normalizada. Retomando operação..."));
      }
    }

    lerBotoes();
    atualizarLedsBotoes();

    if (!modoManual) {

      if (referenciandoAtivo) {
        gerenciarReferenciamento();
      } else if (estado == 0) { // PARADO (STANDBY)
        lcdParado();

        if (chamada[andarAtual] == true && sensorAtivo(andarAtual)) {
          chamada[andarAtual] = false; 
          comandoPortaEnviado = false; 
          tocarSomAoAbrir = false;
          estado = 2; 
        } else {
          if (chamada[andarAtual] == true && !sensorAtivo(andarAtual)) {
            if (direcaoAtual == 1 && andarAtual < 3) andarAtual++;
            else if (direcaoAtual == -1 && andarAtual > 1) andarAtual--;
          }

          andarDestino = escolherProximoAndar();
          
          if (andarDestino != 0 && andarDestino != andarAtual) {
            if (portaEstaTotalmenteFechada()) {
              estado = 1; 
            }
          } else if (estadoAnteriorLog != 0 && sensorAtivo(andarAtual)) {
            Serial.print(F("Elevador Parado no "));
            Serial.print(andarAtual);
            Serial.println(F("º Andar"));
            estadoAnteriorLog = 0;
          }
        }

      } else if (estado == 1) { // MOVENDO
        if (!portaEstaTotalmenteFechada()) {
          pararMotor();
          return;
        }

        if (estadoAnteriorLog != 1) {
          Serial.print(F("Elevador em Movimento para o "));
          Serial.print(andarDestino);
          Serial.println(F("º Andar"));
          estadoAnteriorLog = 1;
        }

        if (andarDestino > andarAtual) {
          ligarMotorSubir();
          lcdSubindo();

          if (sensorAtivo(andarAtual + 1) && (millis() - tempoUltimaPassagemAndar > 600)) {
            andarAtual++;
            tempoUltimaPassagemAndar = millis();
            Serial.print(F("Passando pelo "));
            Serial.print(andarAtual);
            Serial.println(F("º Andar"));
          }
        } else if (andarDestino < andarAtual) {
          ligarMotorDescer();
          lcdDescendo();

          if (sensorAtivo(andarAtual - 1) && (millis() - tempoUltimaPassagemAndar > 600)) {
            andarAtual--;
            tempoUltimaPassagemAndar = millis();
            Serial.print(F("Passando pelo "));
            Serial.print(andarAtual);
            Serial.println(F("º Andar"));
          }
        }

        if ((andarAtual == andarDestino && sensorAtivo(andarDestino)) ||
            (chamada[andarAtual] && sensorAtivo(andarAtual))) {

          pararMotor();
          
          registrarViagemConcluida(); 
          Serial.print(F("*V"));
          Serial.println(totalViagensAcumuladas); 
          
          chamada[andarAtual] = false; 
          comandoPortaEnviado = false; 
          tocarSomAoAbrir = true;
          estado = 2;                   
        }

      } else if (estado == 2) { // PORTA ABERTA
        if (estadoAnteriorLog != 2) {
          Serial.print(F("Chegou ao "));
          Serial.print(andarAtual);
          Serial.println(F("º Andar - Abrindo Porta"));
          estadoAnteriorLog = 2;
        }

        chamada[andarAtual] = false; 
        lcdChegou();

        if (!comandoPortaEnviado) {
          comandarAberturaPorta(andarAtual); 
          if (tocarSomAoAbrir) {
            tocarDingDong();
          }
          enviarLog(F("*P1")); 
          comandoPortaEnviado = true;
        }

        if (portaEstaTotalmenteFechada()) {
          enviarLog(F("*P0"));
          chamada[andarAtual] = false; 
          estado = 0;
        }
      }

    } else {
      lcdManutencao();

      if (sentidoMotor == 1 && sensorAtivo(3)) {
        pararMotor();
        andarAtual = 3;
        enviarLog(F("[ADMIN]: Limite superior (3º Andar) atingido - Motor parado."));
      } else if (sentidoMotor == 2 && sensorAtivo(1)) {
        pararMotor();
        andarAtual = 1;
        enviarLog(F("[ADMIN]: Limite inferior (1º Andar) atingido - Motor parado."));
      }
    }

  } else { // MODO EMERGÊNCIA ATIVO
    if (emergenciasTratadas == false) {
      pararMotor();
      setSireneEmergencia(true); 

      int andarSensorAtivo = 0;
      if (sensorAtivo(1)) andarSensorAtivo = 1;
      else if (sensorAtivo(2)) andarSensorAtivo = 2;
      else if (sensorAtivo(3)) andarSensorAtivo = 3;

      if (andarSensorAtivo > 0) {
        andarAtual = andarSensorAtivo;
        ativarPortaEmergencia(andarAtual);
        portaAbertaNaEmergencia = true;
        Serial.print(F("PERIGO: Emergência Ativada! Porta do "));
        Serial.print(andarAtual);
        Serial.println(F("º andar aberta para resgate."));
      } else {
        portaAbertaNaEmergencia = false;
        enviarLog(F("PERIGO: Emergência Ativada! Cabine fora de nível - Portas mantidas TRAVADAS."));
      }

      emergenciasTratadas = true;
      estadoAnteriorLog = -1;
    }

    rotinaSeguranca();
    lcdEmergencia();
    atualizarLedsBotoes();
  }
}