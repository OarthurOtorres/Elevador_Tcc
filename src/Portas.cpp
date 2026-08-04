#include "Portas.h"
#include <Arduino.h>

Servo PortaAndar1;
Servo PortaAndar2;
Servo PortaAndar3;

#define UltraTrig 10
#define UltraEcho 11

// Variáveis de controle internas da máquina de estados das portas
enum EstadoPorta {
    PORTA_FECHADA,
    PORTA_ABRINDO,
    PORTA_ABERTA_ESPERANDO,
    PORTA_FECHANDO
};

EstadoPorta estadoAtualPorta = PORTA_FECHADA;
unsigned long tempoInicioEspera = 0;
int andarAtivo = 1; // Guarda qual andar o elevador está para mexer no servo certo
bool comandoAbrir = false;

// Função interna para ler o único sensor ultrassônico da cabine
float lerDistanciaCabineCM() {
    digitalWrite(UltraTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(UltraTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(UltraTrig, LOW);
    
    long duracao = pulseIn(UltraEcho, HIGH, 20000); // Timeout de 20ms para não travar
    if (duracao == 0) return 999;
    
    return (duracao * 0.0343) / 2.0;
}

// Função para mover o servo do andar correto
void moverServoAndar(int andar, int angulo) {
    if (andar == 1) PortaAndar1.write(angulo);
    else if (andar == 2) PortaAndar2.write(angulo);
    else if (andar == 3) PortaAndar3.write(angulo);
}

void inicializarPortas() {
    PortaAndar1.attach(A0);
    PortaAndar2.attach(A1);
    PortaAndar3.attach(A2);

    pinMode(UltraTrig, OUTPUT);
    pinMode(UltraEcho, INPUT);

    // Garante que todas começam fechadas
    PortaAndar1.write(ANGULO_FECHADO);
    PortaAndar2.write(ANGULO_FECHADO);
    PortaAndar3.write(ANGULO_FECHADO);
    
    estadoAtualPorta = PORTA_FECHADA;
    comandoAbrir = false;
}

// Seu código principal chama essa função passando o andar (1, 2 ou 3) quando o elevador parar
void comandarAberturaPorta(int andar) {
    if (andar >= 1 && andar <= 3) {
        andarAtivo = andar;
        comandoAbrir = true;
    }
}

// Devolve true se a máquina voltou para o estado FECHADA (libera o elevador para andar)
bool portaEstaTotalmenteFechada() {
    return (estadoAtualPorta == PORTA_FECHADA);
}

void gerenciarMaquinaPortas() {
    float distancia = 999;

    switch (estadoAtualPorta) {
        case PORTA_FECHADA:
            if (comandoAbrir) {
                // Abre apenas o servo do andar onde a cabine parou
                moverServoAndar(andarAtivo, ANGULO_ABERTO);
                estadoAtualPorta = PORTA_ABRINDO;
                tempoInicioEspera = millis();
                comandoAbrir = false;
            }
            break;

        case PORTA_ABRINDO:
            // Aguarda o tempo físico do servo abrir a porta (800ms)
            if (millis() - tempoInicioEspera >= 800) {
                estadoAtualPorta = PORTA_ABERTA_ESPERANDO;
                tempoInicioEspera = millis();
            }
            break;

        case PORTA_ABERTA_ESPERANDO:
            // O único sensor lê a distância (ele está na cabine, olhando para o vão da porta aberta)
            distancia = lerDistanciaCabineCM();

            // Se alguém interromper o raio, empurra o cronômetro para frente
            if (distancia < DISTANCIA_SEGURANCA) {
                tempoInicioEspera = millis(); 
            }

            // Se ficou 3 segundos livre, fecha a porta do andar ativo
            if (millis() - tempoInicioEspera >= TEMPO_PORTA_ABERTA) {
                moverServoAndar(andarAtivo, ANGULO_FECHADO);
                tempoInicioEspera = millis();
                estadoAtualPorta = PORTA_FECHANDO;
            }
            break;

        case PORTA_FECHANDO:
            distancia = lerDistanciaCabineCM();

            // ANTI-ESMAGAMENTO: Alguém tentou entrar com a porta fechando? Reabre imediatamente!
            if (distancia < DISTANCIA_SEGURANCA) {
                moverServoAndar(andarAtivo, ANGULO_ABERTO);
                estadoAtualPorta = PORTA_ABRINDO;
                tempoInicioEspera = millis();
                break;
            }

            // Aguarda o servo terminar de fechar fisicamente
            if (millis() - tempoInicioEspera >= 800) {
                estadoAtualPorta = PORTA_FECHADA;
            }
            break;
    }
}