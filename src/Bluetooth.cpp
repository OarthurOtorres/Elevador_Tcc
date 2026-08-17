#include "Bluetooth.h"
#include "Logica.h"
#include "Emergencia.h"
#include <SoftwareSerial.h>

// Pinos do Módulo HC-05: Pin 10 (TX do HC05) e Pin 11 (RX do HC05)
SoftwareSerial bluetooth(10, 11);

// Variáveis globais compartilhadas com o restante do projeto
extern bool chamada[4];
extern volatile bool emergenciaAtivada;

// Armazenamento de estado anterior para transmitir apenas quando houver mudança
static bool last_c1 = false, last_c2 = false, last_c3 = false;
static bool last_s1 = false, last_s2 = false, last_s3 = false;
static bool last_emerg = false;

unsigned long tempoUltimaAtualizacao = 0;

void BluetoothInit() {
  bluetooth.begin(9600); // Velocidade padrão do HC-05
}

void enviarLog(const String& mensagem) {
  bluetooth.println(mensagem);
}

// Transmite o estado de todos os LEDs para o HTML
void atualizarInterfaceBluetooth() {
  if (millis() - tempoUltimaAtualizacao < 150) return; // Envia a cada 150ms
  tempoUltimaAtualizacao = millis();

  // 1. Atualiza Chamadas
  if (chamada[1] != last_c1) { last_c1 = chamada[1]; bluetooth.println(last_c1 ? "*A1" : "*A0"); }
  if (chamada[2] != last_c2) { last_c2 = chamada[2]; bluetooth.println(last_c2 ? "*B1" : "*B0"); }
  if (chamada[3] != last_c3) { last_c3 = chamada[3]; bluetooth.println(last_c3 ? "*C1" : "*C0"); }

  // 2. Atualiza Sensores de Andar
  bool s1 = sensorAtivo(1);
  bool s2 = sensorAtivo(2);
  bool s3 = sensorAtivo(3);

  if (s1 != last_s1) { last_s1 = s1; bluetooth.println(s1 ? "*S11" : "*S10"); }
  if (s2 != last_s2) { last_s2 = s2; bluetooth.println(s2 ? "*S21" : "*S20"); }
  if (s3 != last_s3) { last_s3 = s3; bluetooth.println(s3 ? "*S31" : "*S30"); }

  // 3. Atualiza LED de Emergência
  if (emergenciaAtivada != last_emerg) {
    last_emerg = emergenciaAtivada;
    bluetooth.println(last_emerg ? "*X1" : "*X0");
  }
}

// Trata os botões clicados dentro do Dashboard HTML
void lerComandosBluetooth() {
  if (bluetooth.available()) {
    char cmd = bluetooth.read();

    if (cmd == '1') { chamada[1] = true; enviarLog("Chamada efetuada: 1º Andar"); }
    if (cmd == '2') { chamada[2] = true; enviarLog("Chamada efetuada: 2º Andar"); }
    if (cmd == '3') { chamada[3] = true; enviarLog("Chamada efetuada: 3º Andar"); }

    if (cmd == 'E' || cmd == 'e') {
      emergenciaAtivada = true;
      enviarLog("ALERTA: Emergência acionada via Supervisório!");
    }

    if (cmd == 'R' || cmd == 'r') {
      ResetEmergencia(); // Tenta o reset
      if (!emergenciaAtivada) {
        bluetooth.println("*R1");
        delay(300);
        bluetooth.println("*R0");
        enviarLog("Sistema de emergência resetado.");
      } else {
        enviarLog("ERRO: O botão de emergência físico ainda está acionado!");
      }
    }
  }
}