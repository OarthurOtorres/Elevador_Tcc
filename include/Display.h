#ifndef Display_h
#define Display_h

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Declaração do objeto lcd como extern para ser usado em outros arquivos
extern LiquidCrystal_I2C lcd;

// Controla o número de escritas no display em caso de emergência para evitar sobrecarga
extern bool telaEmergenciaEscrita;

// Funções do display
void lcdInit();
void lcdParado();
void lcdSubindo();
void lcdDescendo();
void lcdChegou();
void lcdEmergencia();

#endif