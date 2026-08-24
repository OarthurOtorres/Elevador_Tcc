#ifndef Motor_h
#define Motor_h

// Velocidade máxima de trabalho (0 a 255)
#define VELOCIDADE_MOTOR 255 

void inicializarMotor();
void ligarMotorSubir();
void ligarMotorDescer();
void pararMotor();
void atualizarRampaMotor(); // Mantida vazia para não quebrar a chamada no seu main.cpp

#endif