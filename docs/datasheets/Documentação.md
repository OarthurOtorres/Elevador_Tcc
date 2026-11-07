# 📐 Especificações Técnicas e Datasheets dos Componentes

Esta pasta armazena a documentação técnica oficial (*datasheets* em PDF, manuais e diagramas de pinagem) de todos os componentes eletrônicos, módulos e atuadores utilizados no projeto do **Elevador Inteligente**, além dos arquivos da placa de circuito impresso (PCI) desenvolvida no KiCad.

---

## 📊 Tabela Consolidada de Componentes

| Componente / Módulo | Modelo / CI Principal | Tensão / Comunicação | Função no Sistema | Arquivo PDF |
| :--- | :--- | :--- | :--- | :--- |
| **Microcontrolador** | ATmega328P (Arduino Uno) | 5V DC / 16 MHz | Processamento principal e lógica de controle em C++ | [Ver Datasheet](./arduino_uno.pdf) |
| **Motor de Tração** | Motor DC de Para-brisa | 24V DC / Controle PWM | Movimentação mecânica da cabine (subida e descida) | [Ver Datasheet](./motor_parabrisa.pdf) |
| **Driver de Tração** | BTS7960 (Ponte H Dupla) | 5V (Lógica) / 24V (Motor) | Controle de aceleração (PWM) e sentido do motor principal | [Ver Datasheet](./ponte_h_bts7960.pdf) |
| **Expansor de I/O** | PCF8574 | 2.5V - 6V DC / Barramento I2C (`0x27`) | Leitura da botoeira física e acionamento dos LEDs do painel | [Ver Datasheet](./expansor_pcf8574.pdf) |
| **Módulo Bluetooth** | HC-05 | 3.3V - 5V DC / UART Serial (9600 baud) | Comunicação sem fio com o Supervisório Web (SCADA) | [Ver Datasheet](./modulo_bluetooth.PDF) |
| **Display de Sinalização** | LCD 16x2 I2C | 5V DC / Barramento I2C (`0x25`) | Exibição visual de andar, sentido e estado de emergência | [Ver Datasheet](./display_lcd.PDF) |
| **Sensores de Andar** | Sensor Magnético Hall / Fim de Curso | 5V DC / Digital (Pull-Up) | Detecção da posição física da cabine nos andares (S1, S2, S3) | [Ver Datasheet](./sensor_hall.PDF) |
| **Sensor de Porta** | Módulo IR de Barreira (FC-51) | 3.3V - 5V DC / Digital | Detecção de presença/obstrução na porta da cabine (Pino D8) | [Ver Datasheet (ES/EN)](./sensor_ir.pdf) |
| **Atuador de Porta** | Micro Servomotor (MG90S) | 4.8V - 6V DC / Sinal PWM | Mecanismo de abertura e fechamento físico das portas | [Ver Datasheet](./servo_mg90s.PDF) |
| **Sinalizador Sonoro** | Buzzer Passivo 5V | 5V DC / Frequência (Tone) | Emissão da sirene de emergência e som "Ding-Dong" | *Módulo Padrão 5V* |

---

## 🛠️ Documentação de Engenharia e Fabricação da PCI

Os arquivos de projeto da Placa de Circuito Impresso (PCI) desenvolvida no software KiCad estão organizados abaixo:

| Documento | Descrição | Arquivo PDF |
| :--- | :--- | :--- |
| **Esquema Elétrico** | Diagrama esquemático completo das ligações, barramentos e pinagem | [Ver Esquemático](./esquematico_circuito.pdf) |
| **Layout Técnico PCI** | Desenho cotado com as dimensões exatas e o traçado das trilhas de cobre | [Ver Layout PCI](./layout_pci.pdf) |
| **Mapa de Montagem (Serigrafia)** | Guia de posicionamento e identificação dos componentes na placa (R1, U1, conectores) | [Ver Serigrafia](./serigrafia_pci.pdf) |
| **Fotolito para Impressão** | Máscara fotográfica em vetor pronta para impressão em transparência (Insolação UV) | [Ver Fotolito](./fotolito_impressao.pdf) |

---

### 🔗 Acesse também:

💬 **Comunidade:** Tem alguma dúvida, problema na compilação ou ideia de melhoria? Participe na nossa aba de [Discussões do Repositório](https://github.com/OarthurOtorres/Elevador_Tcc/discussions).

📌 **Navegação do Repositório:**  
[⬅️ Voltar ao README Principal](../../README.md) | [ Ver Guia de Execução no VS Code](../ComoExecutar.md)
