#ifndef DISPLAY_H
#define DISPLAY_H

void lcdInit();
void lcdParado();
void lcdSubindo();
void lcdDescendo();
void lcdChegou();
void lcdEmergencia();
void lcdReferenciando(); // <-- Nova função para a etapa de homing/referenciamento
void desligarLedReset();

#endif