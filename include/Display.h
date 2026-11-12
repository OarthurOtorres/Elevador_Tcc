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
void lcdManutencao(); // Função para exibir o modo de manutenção no LCD

#endif