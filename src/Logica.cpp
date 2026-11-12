#include "Logica.h"
#include "Motor.h"
#include "Display.h"   
#include "Bluetooth.h" 
#include "Portas.h"
#include "Emergencia.h"
#include <Arduino.h>
#include <Wire.h>

#define PCF_ADDR 0x27

#define S1 5
#define S2 6
#define S3 7

bool chamada[4] = {false, false, false, false};
bool modoManual = false;
bool referenciandoAtivo = false;

int andarAtual = 0;
int andarDestino = 0;
int estado = 0;
int direcaoAtual = 0;

unsigned int totalViagensAcumuladas = 0;
static unsigned long inicioHoming = 0;

void registrarViagemConcluida() { totalViagensAcumuladas++; }

bool lerSensorComFiltro(int pino) {
  int contagemLow = 0;
  for (int i = 0; i < 4; i++) {
    if (digitalRead(pino) == LOW) contagemLow++;
    delayMicroseconds(300);
  }
  return (contagemLow >= 3);
}

bool sensorAtivo(int andar) {
  if (andar == 1) return lerSensorComFiltro(S1);
  if (andar == 2) return lerSensorComFiltro(S2);
  if (andar == 3) return lerSensorComFiltro(S3);
  return false;
}

// Inicia referenciamento não-bloqueante (usado ao sair do modo manual)
void iniciarReferenciamento() {
  referenciandoAtivo = true;
  inicioHoming = millis();
  if (!portaEstaTotalmenteFechada()) {
    restaurarPortasAposEmergencia(andarAtual);
  }
}

// Executado continuamente no loop() sem bloquear Bluetooth ou Emergência
void gerenciarReferenciamento() {
  if (!referenciandoAtivo) return;

  // Aguarda portas fecharem antes de mover a cabine
  if (!portaEstaTotalmenteFechada()) {
    pararMotor();
    inicioHoming = millis();
    return;
  }

  lcdReferenciando();
  ligarMotorDescer();

  if (sensorAtivo(1) || sensorAtivo(2) || sensorAtivo(3)) {
    pararMotor();
    if (sensorAtivo(1)) andarAtual = 1;
    else if (sensorAtivo(2)) andarAtual = 2;
    else if (sensorAtivo(3)) andarAtual = 3;

    andarDestino = andarAtual;
    estado = 0;
    estadoAnteriorLog = -1;
    referenciandoAtivo = false;

    lcdParado();
    Serial.print(F("Referenciamento concluído no "));
    Serial.print(andarAtual);
    Serial.println(F("º Andar."));
    return;
  }

  if (millis() - inicioHoming > 12000) {
    pararMotor();
    referenciandoAtivo = false;
    estado = 0;
    enviarLog(F("ALERTA: Timeout no referenciamento! Verifique sensores."));
  }
}

// Homing inicial durante o setup()
void executarHoming() {
  if (!portaEstaTotalmenteFechada()) {
    restaurarPortasAposEmergencia(andarAtual);
    while (!portaEstaTotalmenteFechada() && !emergenciaAtivada) {
      gerenciarMaquinaPortas();
      lerComandosBluetooth();
      atualizarInterfaceBluetooth();
      delay(5);
    }
  }

  if (emergenciaAtivada) return;

  lcdReferenciando();
  enviarLog(F("Elevador em Referenciamento..."));
  
  ligarMotorDescer();
  unsigned long inicio = millis();
  
  while (!sensorAtivo(1) && !sensorAtivo(2) && !sensorAtivo(3) && !emergenciaAtivada) {
    atualizarRampaMotor();
    lerComandosBluetooth();
    atualizarInterfaceBluetooth();
    if (millis() - inicio > 12000) {
      pararMotor();
      enviarLog(F("ALERTA: Timeout no referenciamento! Verifique sensores."));
      return;
    }
    delay(5);
  }
  
  pararMotor();

  if (emergenciaAtivada) return;

  if (sensorAtivo(1)) andarAtual = 1;
  else if (sensorAtivo(2)) andarAtual = 2;
  else if (sensorAtivo(3)) andarAtual = 3;

  andarDestino = andarAtual;
  estado = 0;

  lcdParado();
  Serial.print(F("Referenciamento concluído no "));
  Serial.print(andarAtual);
  Serial.println(F("º Andar."));
}

void detectarAndarInicial() {
  delay(100); 
  if (lerSensorComFiltro(S1)) { andarAtual = 1; andarDestino = 1; }
  else if (lerSensorComFiltro(S2)) { andarAtual = 2; andarDestino = 2; }
  else if (lerSensorComFiltro(S3)) { andarAtual = 3; andarDestino = 3; }
  else { executarHoming(); }
}

void initBtsESensores() {
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);
  Wire.begin();
  Wire.beginTransmission(PCF_ADDR);
  Wire.write(0b01101101);
  Wire.endTransmission();
  detectarAndarInicial();
}

void atualizarLedsBotoes() {
  if (modoManual) {
    uint8_t bytePCF = 0b01101101; 
    Wire.beginTransmission(PCF_ADDR);
    Wire.write(bytePCF);
    Wire.endTransmission();
    return;
  }

  for (int i = 1; i <= 3; i++) {
    if (sensorAtivo(i) && estado != 0) chamada[i] = false;
  }

  uint8_t bytePCF = 0b01101101; 
  if (chamada[1]) bytePCF |= (1 << 1);
  if (chamada[2]) bytePCF |= (1 << 4);
  if (chamada[3]) bytePCF |= (1 << 7);

  Wire.beginTransmission(PCF_ADDR);
  Wire.write(bytePCF);
  Wire.endTransmission();
}

void lerBotoes() {
  if (modoManual || referenciandoAtivo) return; 

  Wire.requestFrom((uint8_t)PCF_ADDR, (uint8_t)1);
  if (!Wire.available()) return;

  uint8_t leitura = Wire.read();
  bool bt1Pressionado = !(leitura & (1 << 0));
  bool bt2Pressionado = !(leitura & (1 << 3));
  bool bt3Pressionado = !(leitura & (1 << 6));

  if (bt1Pressionado) chamada[1] = true;
  if (bt2Pressionado) chamada[2] = true;
  if (bt3Pressionado) chamada[3] = true;

  atualizarLedsBotoes();
}

int escolherProximoAndar() {
  if (direcaoAtual == 1) {
    for (int i = andarAtual + 1; i <= 3; i++) if (chamada[i]) return i;
    direcaoAtual = -1;
  }
  if (direcaoAtual == -1) {
    for (int i = andarAtual - 1; i >= 1; i--) if (chamada[i]) return i;
    direcaoAtual = 0;
  }
  if (direcaoAtual == 0) {
    for (int i = andarAtual + 1; i <= 3; i++) { if (chamada[i]) { direcaoAtual = 1; return i; } }
    for (int i = andarAtual - 1; i >= 1; i--) { if (chamada[i]) { direcaoAtual = -1; return i; } }
  }
  return 0;
}