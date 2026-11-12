#include "Bluetooth.h"
#include "Logica.h"
#include "Emergencia.h"
#include "Motor.h"
#include "Portas.h"

extern bool chamada[4];
extern volatile bool emergenciaAtivada;
extern unsigned int totalViagensAcumuladas; 
extern bool modoManual;
extern bool referenciandoAtivo;
extern int estadoAnteriorLog;

static bool last_c1 = false, last_c2 = false, last_c3 = false;
static bool last_s1 = false, last_s2 = false, last_s3 = false;
static bool last_emerg = false;
static int last_motor = -1; 
static bool last_porta = false;
static bool last_ir = false;
static unsigned int last_viagens = 0xFFFF;

unsigned long tempoUltimaAtualizacao = 0;

void BluetoothInit() { Serial.begin(9600); }
void enviarLog(const String& mensagem) { Serial.println(mensagem); }
void enviarLog(const __FlashStringHelper* mensagem) { Serial.println(mensagem); }

void enviarEstadoCompleto() {
  last_c1 = chamada[1]; Serial.println(last_c1 ? F("*A1") : F("*A0"));
  last_c2 = chamada[2]; Serial.println(last_c2 ? F("*B1") : F("*B0"));
  last_c3 = chamada[3]; Serial.println(last_c3 ? F("*C1") : F("*C0"));

  last_s1 = sensorAtivo(1); Serial.println(last_s1 ? F("*S11") : F("*S10"));
  last_s2 = sensorAtivo(2); Serial.println(last_s2 ? F("*S21") : F("*S20"));
  last_s3 = sensorAtivo(3); Serial.println(last_s3 ? F("*S31") : F("*S30"));

  last_emerg = emergenciaAtivada; Serial.println(last_emerg ? F("*X1") : F("*X0"));

  last_motor = sentidoMotor;
  if (last_motor == 1) Serial.println(F("*MU"));
  else if (last_motor == 2) Serial.println(F("*MD"));
  else Serial.println(F("*MS"));

  last_porta = portaEstaAberta(); Serial.println(last_porta ? F("*P1") : F("*P0"));
  last_ir = lerSensorIR(); Serial.println(last_ir ? F("*IR1") : F("*IR0"));

  last_viagens = totalViagensAcumuladas;
  Serial.print(F("*V")); Serial.println(last_viagens);
}

void atualizarInterfaceBluetooth() {
  if (millis() - tempoUltimaAtualizacao < 100) return;
  tempoUltimaAtualizacao = millis();

  if (chamada[1] != last_c1) { last_c1 = chamada[1]; Serial.println(last_c1 ? F("*A1") : F("*A0")); }
  if (chamada[2] != last_c2) { last_c2 = chamada[2]; Serial.println(last_c2 ? F("*B1") : F("*B0")); }
  if (chamada[3] != last_c3) { last_c3 = chamada[3]; Serial.println(last_c3 ? F("*C1") : F("*C0")); }

  bool s1 = sensorAtivo(1); bool s2 = sensorAtivo(2); bool s3 = sensorAtivo(3);
  if (s1 != last_s1) { last_s1 = s1; Serial.println(s1 ? F("*S11") : F("*S10")); }
  if (s2 != last_s2) { last_s2 = s2; Serial.println(s2 ? F("*S21") : F("*S20")); }
  if (s3 != last_s3) { last_s3 = s3; Serial.println(s3 ? F("*S31") : F("*S30")); }

  if (emergenciaAtivada != last_emerg) {
    last_emerg = emergenciaAtivada;
    Serial.println(last_emerg ? F("*X1") : F("*X0"));
  }

  if (sentidoMotor != last_motor) {
    last_motor = sentidoMotor;
    if (last_motor == 1) Serial.println(F("*MU"));
    else if (last_motor == 2) Serial.println(F("*MD"));
    else Serial.println(F("*MS"));
  }

  bool pAberta = portaEstaAberta();
  if (pAberta != last_porta) { last_porta = pAberta; Serial.println(last_porta ? F("*P1") : F("*P0")); }

  bool irDetectado = lerSensorIR();
  if (irDetectado != last_ir) { last_ir = irDetectado; Serial.println(last_ir ? F("*IR1") : F("*IR0")); }

  if (totalViagensAcumuladas != last_viagens) {
    last_viagens = totalViagensAcumuladas;
    Serial.print(F("*V")); Serial.println(last_viagens);
  }
}

void lerComandosBluetooth() {
  while (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'Q' || cmd == 'q') { enviarEstadoCompleto(); }
    else if (cmd == '1') { if (!modoManual && !referenciandoAtivo) chamada[1] = true; }
    else if (cmd == '2') { if (!modoManual && !referenciandoAtivo) chamada[2] = true; }
    else if (cmd == '3') { if (!modoManual && !referenciandoAtivo) chamada[3] = true; }
    else if (cmd == 'E' || cmd == 'e') {
      if (!emergenciaAtivada) {
        emergenciaAtivada = true;
        enviarLog(F("ALERTA: Emergência acionada via Supervisório!"));
      }
    }
    else if (cmd == 'R' || cmd == 'r') {
      if (tentarResetarSistema()) enviarLog(F("Sistema de emergência resetado."));
      else enviarLog(F("ERRO: Botão físico ainda acionado!"));
    }
    else if (cmd == 'M' || cmd == 'm') {
      modoManual = true;
      referenciandoAtivo = false;
      pararMotor();
      chamada[1] = false; chamada[2] = false; chamada[3] = false;
      atualizarLedsBotoes();
      enviarLog(F("[ADMIN]: Modo MANUAL / Manutenção ativado."));
    }
    else if (cmd == 'A' || cmd == 'a') {
      modoManual = false; 
      pararMotor(); 
      estado = 0;
      estadoAnteriorLog = -1;
      atualizarLedsBotoes();
      enviarLog(F("[ADMIN]: Modo AUTOMÁTICO ativado."));

      if (sensorAtivo(1)) {
        andarAtual = 1;
        andarDestino = 1;
        referenciandoAtivo = false;
      } else if (sensorAtivo(2)) {
        andarAtual = 2;
        andarDestino = 2;
        referenciandoAtivo = false;
      } else if (sensorAtivo(3)) {
        andarAtual = 3;
        andarDestino = 3;
        referenciandoAtivo = false;
      } else {
        enviarLog(F("[SISTEMA]: Cabine fora de nível. Iniciando Referenciamento..."));
        iniciarReferenciamento();
      }
    }
    else if (cmd == 'U' || cmd == 'u') {
      if (modoManual && !emergenciaAtivada) {
        if (sensorAtivo(3)) {
          enviarLog(F("[ADMIN]: Cabine já está no 3º Andar (limite superior)."));
        } else {
          ligarMotorSubir();
          enviarLog(F("[ADMIN]: Forçando Subida do Motor."));
        }
      }
    }
    else if (cmd == 'D' || cmd == 'd') {
      if (modoManual && !emergenciaAtivada) {
        if (sensorAtivo(1)) {
          enviarLog(F("[ADMIN]: Cabine já está no 1º Andar (limite inferior)."));
        } else {
          ligarMotorDescer();
          enviarLog(F("[ADMIN]: Forçando Descida do Motor."));
        }
      }
    }
    else if (cmd == 'S' || cmd == 's') {
      if (modoManual) { pararMotor(); enviarLog(F("[ADMIN]: Motor Parado Manualmente.")); }
    }
    else if (cmd == '4') { if (modoManual && !emergenciaAtivada) { comandarTogglePortaEspecifica(1); enviarLog(F("[ADMIN]: Ação na Porta do 1º Andar.")); } }
    else if (cmd == '5') { if (modoManual && !emergenciaAtivada) { comandarTogglePortaEspecifica(2); enviarLog(F("[ADMIN]: Ação na Porta do 2º Andar.")); } }
    else if (cmd == '6') { if (modoManual && !emergenciaAtivada) { comandarTogglePortaEspecifica(3); enviarLog(F("[ADMIN]: Ação na Porta do 3º Andar.")); } }
    else if (cmd == 'T' || cmd == 't') {
      int segs = Serial.parseInt();
      if (segs >= 2 && segs <= 20) {
        salvarTempoPorta(segs * 1000);
        Serial.print(F("[ADMIN]: Tempo de Porta ajustado para "));
        Serial.print(segs);
        Serial.println(F("s (Salvo na EEPROM)."));
      }
    }
    else if (cmd == 'Z' || cmd == 'z') {
      totalViagensAcumuladas = 0;
      Serial.println(F("*V0"));
      enviarLog(F("[ADMIN]: Métricas e estatísticas zeradas pelo Administrador."));
    }
  }
}