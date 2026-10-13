# Elevador Inteligente - TCC EtecLG 🛗

Projeto de automação e controle de um elevador de carga/passageiro utilizando **Arduino Uno** e ambiente **PlatformIO**, com controle de tração, segurança, sinalização sonora e supervisório web.

---

## 🛠️ Tecnologias e Hardware Utilizados

* **Microcontrolador:** Arduino Uno (ATmega328P)
* **Linguagem / Framework:** C++ com Framework Arduino (PlatformIO)
* **Atuador Principal:** Motor com Driver Ponte H (BTS7960)
* **Atuador de Portas:** Servomotor
* **Display:** LCD I2C (20x4 / 16x2)
* **Comunicação:** Módulo Bluetooth
* **Expansão de I/O:** Módulo PCF8574
* **Supervisório:** Interface Web em HTML/CSS/JS

---

## 📁 Estrutura do Projeto

```text
Elevador_Tcc/
├── include/              # Arquivos de cabeçalho (.h)
├── src/                  # Código-fonte principal (.cpp)
│   ├── main.cpp          # Loop principal do firmware
│   ├── Logica.cpp        # Lógica de controle dos andares
│   ├── Motor.cpp         # Controle de tração e acionamento da Ponte H
│   ├── Portas.cpp        # Controle de abertura e fechamento das portas (Servo)
│   ├── Display.cpp       # Interface visual no LCD I2C
│   ├── Bluetooth.cpp     # Recepção de comandos via Bluetooth
│   ├── Emergencia.cpp    # Sistema de parada de emergência e interrupções
│   └── DingDong.cpp      # Sinalização sonora de chegada ao andar
├── Supervisorio_html/    # Interface do Supervisório SCADA
│   └── index.html        # Painel web de monitoramento
└── platformio.ini        # Configurações de build e dependências
