#include "Display.h"
#include "Logica.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Inicializa o display LCD no endereço 0x25 com 16 colunas e 2 linhas
LiquidCrystal_I2C lcd(0x25, 16, 2);

// Variáveis para controlar o piscar do backlight em caso de emergência 
unsigned long tempoPiscaEmergencia = 0;
bool estadoPiscaEmergencia = false;

// Controla o número de escritas no display em caso de emergência para evitar sobrecarga
bool telaEmergenciaEscrita = false;

#define LedReset 4 // Led do Reset piscante

// Função auxiliar interna para descobrir a próxima parada imediata no caminho
int obterProximaParada() {
  if (andarDestino > andarAtual) { // Subindo
    for (int i = andarAtual + 1; i <= andarDestino; i++) {
      if (chamada[i]) return i;
    }
  } else if (andarDestino < andarAtual) { // Descendo
    for (int i = andarAtual - 1; i >= andarDestino; i--) {
      if (chamada[i]) return i;
    }
  }
  return andarDestino;
}

// Inicialização física do display LCD
void lcdInit() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA ELEVADOR");
  lcd.setCursor(0, 1);
  lcd.print("   TCC PRONTO   ");
  delay(2000);
  lcd.clear();
  pinMode(LedReset, OUTPUT);    // Configura o pino do LED de reset como saída
  digitalWrite(LedReset, LOW);  // Inicializa o LED de reset como apagado
}

// NOVO: Mostra status durante a busca de referência/homing
void lcdReferenciando() {
  lcd.setCursor(0, 0);
  lcd.print("STATUS: DESCENDO  ");
  lcd.setCursor(0, 1);
  lcd.print("REFERENCIANDO "); // 16 caracteres para limpar toda a linha
}

// Mostra o status atual e onde o elevador está estacionado
void lcdParado() {
  lcd.setCursor(0, 0);
  lcd.print("STATUS: PARADO  ");
  lcd.setCursor(0, 1);
  lcd.print("Andar Atual: ");
  lcd.print(andarAtual);
  lcd.print("   "); // Espaços para limpar a linha
}

// Mostra a rota dinâmica subindo
void lcdSubindo() {
  lcd.setCursor(0, 0);
  lcd.print("STATUS: SUBINDO ");
  lcd.setCursor(0, 1);
  lcd.print("Rota: ");
  lcd.print(andarAtual);
  lcd.print(" -> ");
  lcd.print(obterProximaParada());
  lcd.print("   ");
}

// Mostra a rota dinâmica descendo
void lcdDescendo() {
  lcd.setCursor(0, 0);
  lcd.print("STATUS: DESCENDO");
  lcd.setCursor(0, 1);
  lcd.print("Rota: ");
  lcd.print(andarAtual);
  lcd.print(" -> ");
  lcd.print(obterProximaParada());
  lcd.print("   ");
}

// Mostra o status quando atinge o andar e abre as portas
void lcdChegou() {
  lcd.setCursor(0, 0);
  lcd.print("STATUS: PORTAS  ");
  lcd.setCursor(0, 1);
  lcd.print("Andar Atual: ");
  lcd.print(andarAtual);
  lcd.print("   ");
}

// Mostra que o elevador está em emergência e pisca o backlight
void lcdEmergencia() {
  if (telaEmergenciaEscrita == false) { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EMERGENCIA ATIVA");
    lcd.setCursor(0, 1);
    lcd.print("Pressione Reset ");
    telaEmergenciaEscrita = true;
  }

  // Pisca o backlight a cada 500ms
  if (millis() - tempoPiscaEmergencia >= 500) { 
    tempoPiscaEmergencia = millis();
    estadoPiscaEmergencia = !estadoPiscaEmergencia;
    
    if (estadoPiscaEmergencia) {
      lcd.backlight();
      digitalWrite(LedReset, HIGH); // Acende o LED de reset quando o backlight está ligado
    } else {
      lcd.noBacklight();
      digitalWrite(LedReset, LOW);  // Apaga o LED de reset quando o backlight está desligado
    }
  }
}

void desligarLedReset() {
  digitalWrite(LedReset, LOW); // Garante que o LED de reset esteja apagado
  telaEmergenciaEscrita = false; // Permite que uma nova tela de emergência seja montada se necessário
  lcd.backlight();
  lcd.clear();
}