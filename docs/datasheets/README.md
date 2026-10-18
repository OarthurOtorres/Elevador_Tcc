# 📄 Especificações Técnicas e Datasheets dos Componentes

Esta pasta armazena a documentação técnica oficial (*datasheets* em PDF, manuais e diagramas de pinagem) de todos os componentes eletrônicos, módulos e atuadores utilizados no projeto do **Elevador Inteligente**.

---

## 📚 Tabela Consolidada de Componentes

| Componente / Módulo | Modelo / CI Principal | Tensão / Comunicação | Função no Sistema | Arquivo PDF |
| :--- | :--- | :--- | :--- | :--- |
| **Microcontrolador** | ATmega328P (Arduino Uno) | 5V DC / 16 MHz | Processamento principal e lógica de controle em C++ | [Ver Datasheet](./arduino_uno.pdf) |
| **Driver de Tração** | BTS7960 (Ponte H Dupla) | 5V (Lógica) / 12V-24V (Motor) | Controle de aceleração (PWM) e sentido do motor principal | [Ver Datasheet](./ponte_h_bts7960.pdf) |
| **Expansor de I/O** | PCF8574 | 2.5V - 6V DC / Barramento I2C (`0x27`) | Leitura da botoeira física e acionamento dos LEDs do painel | [Ver Datasheet](./expansor_pcf8574.pdf) |
| **Módulo Bluetooth** | HC-05 | 3.3V - 5V DC / UART Serial (9600 baud) | Comunicação sem fio com o Supervisório Web (SCADA) | [Ver Datasheet](./modulo_bluetooth.PDF) |
| **Display de Sinalização** | LCD 16x2 I2C | 5V DC / Barramento I2C | Exibição visual de andar, sentido e estado de emergência | [Ver Datasheet](./lcd_i2c.pdf) |
| **Sensores de Andar** | Sensor Magnético Hall / Fim de Curso | 5V DC / Digital (Pull-Up) | Detecção da posição física da cabine nos andares (S1, S2, S3) | [Ver Datasheet](./sensor_hall.PDF) |
| **Sensor de Porta** | Módulo IR de Barreira (FC-51) | 3.3V - 5V DC / Digital | Detecção de presença/obstrução na porta da cabine (Pino D8) | [Ver Datasheet (ES/EN)](./sensor_ir.pdf) |
| **Atuador de Porta** | Micro Servomotor (MG90S) | 4.8V - 6V DC / Sinal PWM | Mecanismo de abertura e fechamento físico das portas | [Ver Datasheet](./servo_mg90s.PDF) |
| **Sinalizador Sonoro** | Buzzer Passivo 5V | 5V DC / Frequência (Tone) | Emissão da sirene de emergência e som "Ding-Dong" | *Módulo Padrão 5V* |

---

## 🔌 Resumo de Conexões e Mapeamento Elétrico

### 1. Motor de Tração (BTS7960)
- **R_PWM / L_PWM:** Ligados aos pinos **D9** e **D10** do Arduino Uno (saídas PWM para controle de velocidade por rampa de aceleração e inversão de sentido).
- **Alimentação de Potência:** Fonte externa dedicada para o motor de tração, com **GND unificado** ao Arduino.

### 2. Barramento I2C (PCF8574 & Display LCD)
- **SDA:** Conectado ao pino analógico **A4**.
- **SCL:** Conectado ao pino analógico **A5**.
- **Endereço Padrão I2C:** `0x27` (usado para expansão de portas dos botões e sinalização LCD).

### 3. Sensores de Posição e Segurança
- **Sensores de Fim de Curso / Hall (S1, S2, S3):** Conectados com resistores de *pull-up* internos acionados no firmware.
- **Botão de Parada de Emergência:** Conectado ao pino com interrupção externa **D2** (`INT0`).
- **Botão de Reset:** Conectado ao pino com interrupção externa **D3** (`INT1`).
- **Barreira IR de Porta:** Entrada digital no pino **D8**.

---

## ⚠️ Observações de Montagem e Segurança Elétrica

1. **GND Unificado:** O GND da fonte de potência do motor (BTS7960) **deve estar obrigatoriamente interligado ao GND do Arduino** para garantir a referência correta dos sinais PWM.
2. **Filtragem de Ruído (Debounce):** Os pinos de interrupção (D2 e D3) utilizam tratamento de software (*debounce* por tempo de máquina) e resistores *pull-up* para prevenir falsos disparos causados por ruído eletromagnético dos motores.
3. **Endereçamento I2C dos Módulos:** O barramento I2C é compartilhado entre os periféricos com endereços definidos para evitar conflitos de comunicação: o **Display LCD** opera no endereço **0x25** e o **Expansor de I/O (PCF8574)** opera no endereço **0x27**.
4. **Padrão de Nomenclatura dos PDFs:** Para garantir o funcionamento dos links neste repositório, mantenha os nomes dos arquivos PDF exatamente como indicados na tabela acima.
   
---

[⬅️ Voltar para o README Principal](../../README.md)
