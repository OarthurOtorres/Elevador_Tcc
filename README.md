# Elevador Inteligente - TCC EtecLG 🛗

**Curso Técnico em Automação Industrial — 3° AMS**

Projeto de automação e controle de um elevador de carga/passageiro utilizando **Arduino Uno** e ambiente **PlatformIO**, com controle de tração, segurança, sinalização sonora e supervisório web.

---

## 🛠️ Tecnologias e Hardware Utilizados

* **Microcontrolador:** Arduino Uno (ATmega328P)
* **Linguagem / Framework:** C++ com Framework Arduino (PlatformIO)
* **Atuador Principal:** Motor DC com Driver Ponte H (BTS7960)
* **Atuador de Portas:** Servomotor
* **Display:** LCD I2C (16x2)
* **Comunicação:** Módulo Bluetooth
* **Expansão de I/O:** Módulo PCF8574
* **Supervisório:** Interface Web em HTML/CSS/JS

> 📄 **Documentação de Hardware:** Para consultar os manuais técnicos, tensão de operação e PDFs dos componentes utilizados, acesse nosso [Guia de Datasheets e Conexões](./docs/datasheets/README.md).

---

## 🔌 Diagrama Elétrico e Conexões

<img width="1232" height="847" alt="image" src="https://github.com/user-attachments/assets/e9eb89c1-f05e-4ac7-95fc-69a23bf2f6c9" />

O circuito foi projetado para integrar a eletrônica de potência do acionamento do motor com o sistema de controle digital e sensores do elevador:

* **Controle de Potência (BTS7960):** Acionamento do motor de tração com suporte a aceleração, desaceleração e inversão de sentido.
* **Expansão de I/O (PCF8574):** Gerenciamento da botoeira física e sinalização via barramento I2C, otimizando os pinos do microcontrolador.
* **Sensores e Conectividade:** Sensores de posição, barreira infravermelha de porta, módulo Bluetooth e sinalização sonora (buzzer).
  
---

## 🎛️ Placa de Circuito Impresso (Shield Customizado)

| Vista Frontal (Componentes) | Vista Traseira (Trilhas / Cobre) |
| :---: | :---: |
| <img width="1200" height="1600" alt="PCIVista" src="https://github.com/user-attachments/assets/f0f9dcd9-6b50-4dd6-90f1-e810d0069661" /> | <img width="1200" height="1600" alt="PCIVistaTraseira" src="https://github.com/user-attachments/assets/d69b74a5-5500-4068-a688-b3cb942383ce" /> |

Para garantir maior estabilidade elétrica, eliminar o uso de *protoboards* e conferir um acabamento profissional ao protótipo, foi desenvolvida uma **Placa de Circuito Impresso (PCI)** dedicada para acoplamento direto ao Arduino Uno.

### 🔬 Processo de Fabricação da PCI
A confecção da placa seguiu o processo de fotolitografia artesanal:
1. **Mapeamento e Layout:** Design do circuito impresso e diagrama esquemático desenvolvido no software KiCad.
2. **Transferência de Fotolito:** Impressão do layout do circuito em **papel de retroprojetor (transparência)** com alta densidade de preto.
3. **Insolação e Máscara UV:** Aplicação e cura de **tinta UV fotossensível** exposta à luz ultravioleta para transferência do fotolito para a placa de fenolite/fibra de vidro.
4. **Corrosão e Acabamento:** Banho químico para corrosão do cobre excedente, furação dos ilhós e soldagem dos conectores e componentes.

> 🙏 **Agradecimento Especial:**  
> Um agradecimento muito especial ao meu tio **Sérgio Pacheco de Torres**, por todo o apoio técnico, mentoria e por disponibilizar generosamente o espaço e a infraestrutura da sua empresa, a **Premix**, viabilizando a fabricação e montagem da placa de circuito impresso do nosso projeto de TCC.

---

## 🖥️ Interface Supervisória Web (SCADA)

<img width="1917" height="970" alt="image" src="https://github.com/user-attachments/assets/41d97e53-2714-4bc3-9560-83c9004865fa" />

A interface do supervisório atua como um sistema de monitoramento em tempo real do estado do elevador. Desenvolvida para rodar diretamente em navegadores web, ela se comunica com o sistema para oferecer visualização e controle remoto:

* **Monitoramento de Posição:** Exibe em tempo real o andar atual da cabine e o sentido de deslocamento (subindo, descendo ou parado).
* **Gestão de Chamadas:** Permite realizar chamadas de andares diretamente pela tela, simulando a botoeira física do elevador.
* **Diagnóstico e Segurança:** Apresenta indicadores visuais sobre o estado das portas (abertas/fechadas), ativação do sistema de emergência e sensores acionados.

---

## 📁 Estrutura do Projeto

```text
Elevador_Tcc/
├── docs/                 # Documentação técnica e guias do projeto
│   ├── datasheets/       # PDFs dos componentes, esquemático e PCB
│   └── COMO_EXECUTAR.md  # Passo a passo para compilar e rodar no VS Code
├── include/              # Arquivos de cabeçalho (.h)
├── src/                  # Código-fonte principal (.cpp)
│   ├── main.cpp          # Loop principal do firmware
│   ├── Logica.cpp        # Lógica de controle dos andares
│   ├── Motor.cpp         # Controle de tração e acionamento da Ponte H
│   ├── Portas.cpp        # Controle de abertura e fechamento das portas (Servo)
│   ├── Display.cpp       # Interface visual no LCD I2C
│   ├── Bluetooth.cpp     # Recepção de comandos via Bluetooth
│   ├── Emergencia.cpp    # Sistema de parada de emergência
│   └── DingDong.cpp      # Sinalização sonora de chegada ao andar
├── Supervisorio_html/    # Interface Web SCADA (HTML/CSS/JS)
│   └── index.html        # Painel web de monitoramento e controle
└── platformio.ini        # Configurações de build e dependências do PlatformIO
```
>📖 Guia de Instalação:
>Para ver o tutorial completo de como baixar o código e compilar no VS Code, acesse o [Guia de Execução e Instalação](./docs/COMO_EXECUTAR.md).
---

## 👥 Integrantes e Orientação

### 👨‍🎓 Alunos Integrantes

Arthur Oliveira Torres

Gabriel Moura da Silva

Giovanny Manoel P. Arantes

João Pedro Souza

João Vitor C. de Moura

José Felipe Fiocruci

Júlio César Silva

Kauê Silva Ribeiro

Miguel Oliveira de Moraes

### 👨‍🏫 Professor Orientador

Prof. Nivaldo Bertozzo Junior
