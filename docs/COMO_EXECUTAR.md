# 🚀 Guia Passo a Passo: Baixar e Executar o Projeto no VS Code

Este documento fornece o tutorial completo para baixar o código-fonte do **Elevador Inteligente**, configurá-lo no **Visual Studio Code** via **PlatformIO IDE** e compilar o firmware no **Arduino Uno**.

---

## 📋 1. Pré-requisitos Necessários

Antes de começar, certifique-se de ter os seguintes softwares instalados no seu computador:

1. **Visual Studio Code (VS Code):** [Download do VS Code](https://code.visualstudio.com/)
2. **Extensões do VS Code:**
   - Abra o VS Code.
   - Pressione `Ctrl + Shift + X` (ou clique no ícone de quadrados na barra lateral esquerda).
   - Pesquise por **Live Server** e clique em **Install**.
   - Pesquise por **PlatformIO IDE** e clique em **Install**.
   - *Nota: Após a instalação do PlatformIO IDE, é necessário reiniciar o VS Code para que a extensão seja ativada corretamente.*
3. **Cabo USB Tipo A/B:** Para conectar o Arduino Uno ao computador.

---

## 📥 2. Baixando o Código do Repositório

### Download em arquivo ZIP
1. Na página inicial deste repositório no GitHub, clique no botão verde **`<Code>`**.
2. Selecione **Download ZIP**.
3. Após o download, extraia a pasta em qualquer local do seu computador (ex: Área de Trabalho ou Documentos).

---

## 📂 3. Abrindo o Projeto no VS Code

1. Abra o **Visual Studio Code**.
2. No menu superior, clique em **File (Arquivo) ➔ Open Folder... (Abrir Pasta...)** ou use o atalho `Ctrl + K Ctrl + O`.
3. Navegue até a pasta onde você descompactou o projeto (`Elevador_Tcc`) e clique em **Selecionar Pasta**.
4. **Aguarde a inicialização:** O PlatformIO irá identificar o arquivo `platformio.ini` e baixará automaticamente todas as bibliotecas e ferramentas necessárias para o Arduino ATmega328P. Uma barra azul/cinza aparecerá no rodapé do VS Code.

---

## 🛠️ 4. Compilando e Gravando no Arduino

1. Conecte o seu **Arduino Uno** ao computador usando o cabo USB.
2. Na barra inferior esquerda do VS Code (barra de atalhos do PlatformIO), clique nos ícones:
   - **`✓` (Build):** Clique para compilar o código e verificar se não há erros.
   - **`→` (Upload):** Clique para gravar o firmware diretamente no Arduino Uno.
3. Aguarde até aparecer a mensagem **`[SUCCESS]`** no terminal do VS Code.

### ❓ Erro no programa ao abrir ou compilar?
Caso ao compilar apareça algum erro `[FAILED]` ou ocorram avisos de cabeçalho não encontrado (linhas vermelhas sob os `#include`), siga as orientações:

<img width="1366" height="768" alt="image" src="https://github.com/user-attachments/assets/aac8540e-335c-4c9a-8fe6-a4a6853d8a06" />

#### Passo 1: Recriar o índice do C/C++ no VS Code
- Pressione `Ctrl + Shift + P`, digite **`PlatformIO: Rebuild C/C++ Project Index`** e pressione `Enter`.
- *Verificação:* Aguarde alguns segundos e confirme se as linhas vermelhas sob os `#include` desapareceram.

#### Passo 2: Executar a compilação de teste
- Clique no ícone de verificação **`✓` (Build)** na barra inferior do VS Code.
- *Verificação:* O painel de Output/Terminal deve finalizar o processo exibindo a mensagem **`[SUCCESS]`** em verde.

---

## 💻 5. Executando o Supervisório Web (SCADA)

### Passo 1: Emparelhar o Módulo HC-05 no Windows
1. Ligue o circuito do Arduino para alimentar o módulo Bluetooth HC-05 (o LED vermelho do HC-05 começará a piscar rapidamente).
2. No Windows, acesse **Configurações > Bluetooth e dispositivos > Adicionar dispositivo**.
3. Selecione a opção **Bluetooth** e escolha o dispositivo **HC-05** na lista.
4. Quando solicitado o PIN de emparelhamento, digite `1234` ou `0000` e confirme.

### Passo 2: Executar a Interface Web (HTML)
1. Com o VS Code aberto, expanda a pasta `Supervisorio_html/`.
2. Clique com o botão direito sobre o arquivo **`index.html`** e selecione **Open with Live Server (Abrir com Live Server)**.
3. O sistema supervisório será aberto automaticamente no seu navegador padrão.

<img width="1917" height="1078" alt="image" src="https://github.com/user-attachments/assets/f593e67f-d9e7-45f3-bd10-da2917ecfdea" />

### Passo 3: Estabelecer a Conexão Serial no Navegador
1. Na página do supervisório HTML, clique no botão **Conectar** (ou Conectar Bluetooth / Porta Serial).
2. O navegador exibirá uma janela pop-up listando as portas de comunicação disponíveis.
3. Selecione a porta COM correspondente ao módulo HC-05 e clique em **Conectar**.
4. O LED do módulo HC-05 mudará seu padrão de piscada (passando a piscar em intervalos mais longos), confirmando que a conexão entre o supervisório e o Arduino foi estabelecida.

---

## 🎉 Tudo Pronto!

Com o firmware gravado no Arduino, a placa energizada e o supervisório Web conectado via Bluetooth, **o sistema do elevador inteligente está 100% pronto para testes e operação!**

Agora você já pode realizar chamadas de andares, monitorar o deslocamento da cabine em tempo real e testar os mecanismos de segurança (como a barreira IR de porta e os botões de emergência).

---

💬 **Comunidade:** Tem alguma dúvida, problema na compilação ou ideia de melhoria? Participe na nossa aba de [Discussões do Repositório](https://github.com/OarthurOtorres/Elevador_Tcc/discussions).

[⬅️ Voltar para o README Principal](../README.md)
