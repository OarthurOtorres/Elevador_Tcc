#ifndef Portas_h
#define Portas_h

#include <Servo.h>

// Configurações mecânicas comuns
const int ANGULO_ABERTO  = 90; 
const int ANGULO_FECHADO = 0;
const int DISTANCIA_SEGURANCA = 15; // Em centímetros
const unsigned long TEMPO_PORTA_ABERTA = 3000; // 3 segundos

// Declaração dos objetos Servo como extern para serem usados em outros arquivos
extern Servo PortaAndar1;
extern Servo PortaAndar2;
extern Servo PortaAndar3;

// Funções para controlar as portas do elevador
void inicializarPortas();
void comandarAberturaPorta(int andar);
void gerenciarMaquinaPortas();
bool portaEstaTotalmenteFechada();

#endif