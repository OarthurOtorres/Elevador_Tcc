#include "Bluetooth.h"
#include "Logica.h"
#include "Emergencia.h"
#include "Motor.h"
#include "Portas.h"

extern bool chamada[4];
extern volatile bool emergenciaAtivada;
extern unsigned int totalViagensAcumuladas; 

static bool last_c1 = false, last_c2 = false, last_c3 = false;
static bool last_s1 = false, last_s2 = false, last_s3 = false;
static bool last_emerg = false;
static int last_motor = -1; 
static bool last_porta = false;
static bool last_ir = false;
static unsigned int last_viagens = 0xFFFF; // Força primeira sincronização

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

  // 2. Sensores
  last_s1 = sensorAtivo(1); Serial.println(last_s1 ? "*S11" : "*S10");
  last_s2 = sensorAtivo(2); Serial.println(last_s2 ? "*S21" : "*S20");
  last_s3 = sensorAtivo(3); Serial.println(last_s3 ? "*S31" : "*S30");

  // 3. Emergência (Transmite apenas o estado com tag *X)
  last_emerg = emergenciaAtivada; 
  Serial.println(last_emerg ? "*X1" : "*X0");

  // 4. Sentido Motor
  last_motor = sentidoMotor;
  if (last_motor == 1) Serial.println("*MU");
  else if (last_motor == 2) Serial.println("*MD");
  else Serial.println("*MS");

  // 5. Porta e IR
  last_porta = portaEstaAberta();
  Serial.println(last_porta ? "*P1" : "*P0");

  last_ir = lerSensorIR();
  Serial.println(last_ir ? "*IR1" : "*IR0");

  // 6. Viagens
  last_viagens = totalViagensAcumuladas;
  Serial.print("*V");
  Serial.println(last_viagens);
}

void atualizarInterfaceBluetooth() {
  if (millis() - tempoUltimaAtualizacao < 100) return;
  tempoUltimaAtualizacao = millis();

  if (chamada[1] != last_c1) { last_c1 = chamada[1]; Serial.println(last_c1 ? "*A1" : "*A0"); }
  if (chamada[2] != last_c2) { last_c2 = chamada[2]; Serial.println(last_c2 ? "*B1" : "*B0"); }
  if (chamada[3] != last_c3) { last_c3 = chamada[3]; Serial.println(last_c3 ? "*C1" : "*C0"); }

  bool s1 = sensorAtivo(1);
  bool s2 = sensorAtivo(2);
  bool s3 = sensorAtivo(3);

  if (s1 != last_s1) { last_s1 = s1; Serial.println(s1 ? "*S11" : "*S10"); }
  if (s2 != last_s2) { last_s2 = s2; Serial.println(s2 ? "*S21" : "*S20"); }
  if (s3 != last_s3) { last_s3 = s3; Serial.println(s3 ? "*S31" : "*S30"); }

  // Notifica emergência estritamente por evento
  if (emergenciaAtivada != last_emerg) {
    last_emerg = emergenciaAtivada;
    Serial.println(last_emerg ? "*X1" : "*X0");
  }

  if (sentidoMotor != last_motor) {
    last_motor = sentidoMotor;
    if (last_motor == 1) Serial.println("*MU");
    else if (last_motor == 2) Serial.println("*MD");
    else Serial.println("*MS");
  }

  bool pAberta = portaEstaAberta();
  if (pAberta != last_porta) {
    last_porta = pAberta;
    Serial.println(last_porta ? "*P1" : "*P0");
  }

  bool irDetectado = lerSensorIR();
  if (irDetectado != last_ir) {
    last_ir = irDetectado;
    Serial.println(last_ir ? "*IR1" : "*IR0");
  }

  if (totalViagensAcumuladas != last_viagens) {
    last_viagens = totalViagensAcumuladas;
    Serial.print("*V");
    Serial.println(last_viagens);
  }
}

void lerComandosBluetooth() {
  while (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'Q' || cmd == 'q') {
      enviarEstadoCompleto();
    }
    else if (cmd == '1') { chamada[1] = true; }
    else if (cmd == '2') { chamada[2] = true; }
    else if (cmd == '3') { chamada[3] = true; }
    else if (cmd == 'E' || cmd == 'e') {
      if (!emergenciaAtivada) {
        emergenciaAtivada = true;
        enviarLog("ALERTA: Emergência acionada via Supervisório!");
      }
    }
    else if (cmd == 'R' || cmd == 'r') {
      ResetEmergencia(); 
      if (!emergenciaAtivada) {
        enviarLog("Sistema de emergência resetado.");
      } else {
        enviarLog("ERRO: Botão físico ainda acionado!");
      }
    }
  }
}