#include "Portas.h"
#include <Arduino.h>

Servo PortaAndar1;
Servo PortaAndar2;
Servo PortaAndar3;

enum EstadoPorta {
    PORTA_FECHADA,
    PORTA_ABRINDO,
    PORTA_ABERTA_ESPERANDO,
    PORTA_FECHANDO
};

EstadoPorta estadoAtualPorta = PORTA_FECHADA;
unsigned long tempoInicioEspera = 0;
int andarAtivo = 1;
bool comandoAbrir = false;

void moverServoAndar(int andar, int angulo) {
    if (andar == 1) PortaAndar1.write(angulo);
    else if (andar == 2) PortaAndar2.write(angulo);
    else if (andar == 3) PortaAndar3.write(angulo);
}

void inicializarPortas() {
    PortaAndar1.attach(A0);
    PortaAndar2.attach(A1);
    PortaAndar3.attach(A2);

    // Garante que todas começam fechadas
    PortaAndar1.write(ANGULO_FECHADO);
    PortaAndar2.write(ANGULO_FECHADO);
    PortaAndar3.write(ANGULO_FECHADO);
    
    estadoAtualPorta = PORTA_FECHADA;
    comandoAbrir = false;
}

void comandarAberturaPorta(int andar) {
    if (andar >= 1 && andar <= 3) {
        andarAtivo = andar;
        comandoAbrir = true;
    }
}

bool portaEstaTotalmenteFechada() {
    return (estadoAtualPorta == PORTA_FECHADA);
}

void gerenciarMaquinaPortas() {
    switch (estadoAtualPorta) {
        case PORTA_FECHADA:
            if (comandoAbrir) {
                moverServoAndar(andarAtivo, ANGULO_ABERTO);
                estadoAtualPorta = PORTA_ABRINDO;
                tempoInicioEspera = millis();
                comandoAbrir = false;
            }
            break;

        case PORTA_ABRINDO:
            // Dá 800ms para o servo chegar fisicamente a 90 graus
            if (millis() - tempoInicioEspera >= 800) {
                estadoAtualPorta = PORTA_ABERTA_ESPERANDO;
                tempoInicioEspera = millis();
            }
            break;

        case PORTA_ABERTA_ESPERANDO:
            // Aguarda os 3 segundos com a porta aberta
            if (millis() - tempoInicioEspera >= TEMPO_PORTA_ABERTA) {
                moverServoAndar(andarAtivo, ANGULO_FECHADO);
                tempoInicioEspera = millis();
                estadoAtualPorta = PORTA_FECHANDO;
            }
            break;

        case PORTA_FECHANDO:
            // Dá 800ms para o servo fechar completamente antes de avisar a main
            if (millis() - tempoInicioEspera >= 800) {
                estadoAtualPorta = PORTA_FECHADA;
            }
            break;
    }
}