#include "Bluetooth.h"
#include "Logica.h"
#include "Emergencia.h"
#include "Motor.h"
#include "Portas.h"

extern bool chamada[4];
extern volatile bool emergenciaAtivada;

static bool last_c1 = false, last_c2 = false, last_c3 = false;
static bool last_s1 = false, last_s2 = false, last_s3 = false;
static bool last_emerg = false;
static int last_motor = -1; 
static bool last_porta = false;
static bool last_ir = false;

unsigned long tempoUltimaAtualizacao = 0;

void BluetoothInit() {
  Serial.begin(9600); 
}

void enviarLog(const String& mensagem) {
  Serial.println(mensagem);
}

void enviarEstadoCompleto() {
  // 1. Chamadas
  last_c1 = chamada[1]; Serial.println(last_c1 ? "*A1" : "*A0");
  last_c2 = chamada[2]; Serial.println(last_c2 ? "*B1" : "*B0");
  last_c3 = chamada[3]; Serial.println(last_c3 ? "*C1" : "*C0");

  // 2. Sensores de Andar
  last_s1 = sensorAtivo(1); Serial.println(last_s1 ? "*S11" : "*S10");
  last_s2 = sensorAtivo(2); Serial.println(last_s2 ? "*S21" : "*S20");
  last_s3 = sensorAtivo(3); Serial.println(last_s3 ? "*S31" : "*S30");

  // 3. Emergência
  last_emerg = emergenciaAtivada; Serial.println(last_emerg ? "*X1" : "*X0");

  // 4. Sentido do Motor
  last_motor = sentidoMotor;
  if (last_motor == 1) Serial.println("*MU");
  else if (last_motor == 2) Serial.println("*MD");
  else Serial.println("*MS");

  // 5. Estado da Porta (*P1 = Aberta, *P0 = Fechada)
  last_porta = portaEstaAberta();
  Serial.println(last_porta ? "*P1" : "*P0");

  // 6. Sensor IR de Presença (*IR1 = Detectado, *IR0 = Livre)
  last_ir = lerSensorIR();
  Serial.println(last_ir ? "*IR1" : "*IR0");
}

void atualizarInterfaceBluetooth() {
  if (millis() - tempoUltimaAtualizacao < 150) return;
  tempoUltimaAtualizacao = millis();

  // 1. Chamadas
  if (chamada[1] != last_c1) { last_c1 = chamada[1]; Serial.println(last_c1 ? "*A1" : "*A0"); }
  if (chamada[2] != last_c2) { last_c2 = chamada[2]; Serial.println(last_c2 ? "*B1" : "*B0"); }
  if (chamada[3] != last_c3) { last_c3 = chamada[3]; Serial.println(last_c3 ? "*C1" : "*C0"); }

  // 2. Sensores de Andar
  bool s1 = sensorAtivo(1);
  bool s2 = sensorAtivo(2);
  bool s3 = sensorAtivo(3);

  if (s1 != last_s1) { last_s1 = s1; Serial.println(s1 ? "*S11" : "*S10"); }
  if (s2 != last_s2) { last_s2 = s2; Serial.println(s2 ? "*S21" : "*S20"); }
  if (s3 != last_s3) { last_s3 = s3; Serial.println(s3 ? "*S31" : "*S30"); }

  // 3. Emergência
  if (emergenciaAtivada != last_emerg) {
    last_emerg = emergenciaAtivada;
    Serial.println(last_emerg ? "*X1" : "*X0");
  }

  // 4. Sentido do Motor
  if (sentidoMotor != last_motor) {
    last_motor = sentidoMotor;
    if (last_motor == 1) Serial.println("*MU");
    else if (last_motor == 2) Serial.println("*MD");
    else Serial.println("*MS");
  }

  // 5. Estado da Porta
  bool pAberta = portaEstaAberta();
  if (pAberta != last_porta) {
    last_porta = pAberta;
    Serial.println(last_porta ? "*P1" : "*P0");
  }

  // 6. Sensor IR de Presença
  bool irDetectado = lerSensorIR();
  if (irDetectado != last_ir) {
    last_ir = irDetectado;
    Serial.println(last_ir ? "*IR1" : "*IR0");
  }
}

void lerComandosBluetooth() {
  while (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'Q' || cmd == 'q') {
      enviarEstadoCompleto();
    }
    else if (cmd == '1') { chamada[1] = true; enviarLog("Chamada efetuada: 1º Andar"); }
    else if (cmd == '2') { chamada[2] = true; enviarLog("Chamada efetuada: 2º Andar"); }
    else if (cmd == '3') { chamada[3] = true; enviarLog("Chamada efetuada: 3º Andar"); }

    else if (cmd == 'E' || cmd == 'e') {
      emergenciaAtivada = true;
      enviarLog("ALERTA: Emergencia acionada via Supervisorio!");
    }

    else if (cmd == 'R' || cmd == 'r') {
      ResetEmergencia(); 
      if (!emergenciaAtivada) {
        Serial.println("*R1");
        delay(300);
        Serial.println("*R0");
        enviarLog("Sistema de emergencia resetado.");
      } else {
        enviarLog("ERRO: O botao de emergencia fisico ainda esta acionado!");
      }
    }
  }
}