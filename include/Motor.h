#ifndef MOTOR_H
#define MOTOR_H

#define VELOCIDADE_MOTOR 255

extern int sentidoMotor; // 0: Parado, 1: Subindo, 2: Descendo

void inicializarMotor();
void ligarMotorSubir();
void ligarMotorDescer();
void pararMotor();
void atualizarRampaMotor();

#endif