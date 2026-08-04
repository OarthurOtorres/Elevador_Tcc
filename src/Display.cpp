#include "Display.h"
#include "Logica.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Inicializa o display LCD no endereço 0x27 com 16 colunas e 2 linhas
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variáveis para controlar o piscar do backlight em caso de emergência 
unsigned long tempoPiscaEmergencia = 0;
bool estadoPiscaEmergencia = false;

// Controla o número de escritas no display em caso de emergência para evitar sobrecarga
bool telaEmergenciaEscrita = false;

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
}

// Mostra o status atual e onde o elevador está estacionado
void lcdParado() {
  lcd.setCursor(0, 0);
  lcd.print("STATUS: PARADO  ");
  lcd.setCursor(0, 1);
  lcd.print("Andar Atual: ");
  lcd.print(andarAtual);
  lcd.print("   ");
}

// Mostra o status atual e onde o elevador está estacionado
void lcdSubindo() {
  lcd.setCursor(0, 0);
  lcd.print("STATUS: SUBINDO ");
  lcd.setCursor(0, 1);
  lcd.print("Andar Atual: ");
  lcd.print(andarAtual);
  lcd.print("   ");
}

// Mostra o status atual e onde o elevador está estacionado
void lcdDescendo() {
  lcd.setCursor(0, 0);
  lcd.print("STATUS: DESCENDO");
  lcd.setCursor(0, 1);
  lcd.print("Andar Atual: ");
  lcd.print(andarAtual);
  lcd.print("   ");
}

// Mostra que chegou no andar de destino e que as portas estão abertas
void lcdChegou() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CHEGOU NO ANDAR!");
  lcd.setCursor(0, 1);
  lcd.print(" Portas Abertas ");
}

// Mostra que o elevador está em estado de emergência
void lcdEmergencia() {
  if(telaEmergenciaEscrita == false) { // Evita que o display seja escrito várias vezes, sobrecarregando o LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EMERGENCIA ATIVA!");
    lcd.setCursor(0, 1);
    lcd.print("Pressione Reset");
    telaEmergenciaEscrita = true;
  }

  if (millis() - tempoPiscaEmergencia >= 500) { // Pisca o backlight a cada 500ms
    tempoPiscaEmergencia = millis();
    estadoPiscaEmergencia = !estadoPiscaEmergencia;
    
    if (estadoPiscaEmergencia) {
      lcd.backlight();
    } else {
      lcd.noBacklight();
    }
  }
}