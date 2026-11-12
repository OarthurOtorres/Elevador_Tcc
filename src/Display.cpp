#include "Display.h"
#include "Logica.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x25, 16, 2);

unsigned long tempoPiscaEmergencia = 0;
bool estadoPiscaEmergencia = false;
bool telaEmergenciaEscrita = false;

#define LedReset 4

int obterProximaParada() {
  if (andarDestino > andarAtual) {
    for (int i = andarAtual + 1; i <= andarDestino; i++) {
      if (chamada[i]) return i;
    }
  } else if (andarDestino < andarAtual) {
    for (int i = andarAtual - 1; i >= andarDestino; i--) {
      if (chamada[i]) return i;
    }
  }
  return andarDestino;
}

void lcdInit() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("SISTEMA ELEVADOR"));
  lcd.setCursor(0, 1);
  lcd.print(F("   TCC PRONTO   "));
  delay(2000);
  lcd.clear();
  pinMode(LedReset, OUTPUT);
  digitalWrite(LedReset, LOW);
}

void lcdReferenciando() {
  lcd.setCursor(0, 0);
  lcd.print(F("STATUS: DESCENDO"));
  lcd.setCursor(0, 1);
  lcd.print(F("REFERENCIANDO   "));
}

void lcdParado() {
  lcd.setCursor(0, 0);
  lcd.print(F("STATUS: PARADO  "));
  lcd.setCursor(0, 1);
  lcd.print(F("Andar Atual: "));
  lcd.print(andarAtual);
  lcd.print(F("   "));
}

void lcdSubindo() {
  lcd.setCursor(0, 0);
  lcd.print(F("STATUS: SUBINDO "));
  lcd.setCursor(0, 1);
  lcd.print(F("Rota: "));
  lcd.print(andarAtual);
  lcd.print(F(" -> "));
  lcd.print(obterProximaParada());
  lcd.print(F("   "));
}

void lcdDescendo() {
  lcd.setCursor(0, 0);
  lcd.print(F("STATUS: DESCENDO"));
  lcd.setCursor(0, 1);
  lcd.print(F("Rota: "));
  lcd.print(andarAtual);
  lcd.print(F(" -> "));
  lcd.print(obterProximaParada());
  lcd.print(F("   "));
}

void lcdChegou() {
  lcd.setCursor(0, 0);
  lcd.print(F("STATUS: PORTAS  "));
  lcd.setCursor(0, 1);
  lcd.print(F("Andar Atual: "));
  lcd.print(andarAtual);
  lcd.print(F("   "));
}

void lcdManutencao() {
  lcd.setCursor(0, 0);
  lcd.print(F("MODO MANUTENCAO "));
  lcd.setCursor(0, 1);
  lcd.print(F(" CONTROLE ADMIN "));
}

void lcdEmergencia() {
  if (telaEmergenciaEscrita == false) { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("EMERGENCIA ATIVA"));
    lcd.setCursor(0, 1);
    lcd.print(F("Pressione Reset "));
    telaEmergenciaEscrita = true;
  }

  if (millis() - tempoPiscaEmergencia >= 500) { 
    tempoPiscaEmergencia = millis();
    estadoPiscaEmergencia = !estadoPiscaEmergencia;
    
    if (estadoPiscaEmergencia) {
      lcd.backlight();
      digitalWrite(LedReset, HIGH);
    } else {
      lcd.noBacklight();
      digitalWrite(LedReset, LOW);
    }
  }
}

void desligarLedReset() {
  digitalWrite(LedReset, LOW);
  telaEmergenciaEscrita = false;
  lcd.backlight();
  lcd.clear();
}