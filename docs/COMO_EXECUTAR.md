# 🚀 Guia Passo a Passo: Baixar e Executar o Projeto no VS Code

Este documento fornece o tutorial completo para baixar o código-fonte do **Elevador Inteligente**, configurá-lo no **Visual Studio Code** via **PlatformIO IDE** e compilar o firmware no **Arduino Uno**.

---

## 📋 1. Pré-requisitos Necessários

Antes de começar, certifique-se de ter os seguintes softwares instalados no seu computador:

1. **Visual Studio Code (VS Code):** [Download do VS Code](https://code.visualstudio.com/)
2. **Extensão PlatformIO IDE:**
   - Abra o VS Code.
   - Pressione `Ctrl + Shift + X` (ou clique no ícone de quadrados na barra lateral esquerda).
   - Pesquise por **Live Server** e clique em **Install**.
   - Pesquise por **PlatformIO IDE** e clique em **Install**.
   - Depois de instalar o **PlatformIO IDE**, é preciso reiniciar o **VS Code** para que a extensão seja ativada.
3. **Cabo USB Tipo A/B:** Para conectar o Arduino Uno ao computador.

---

## 📥 2. Baixando o Código do Repositório

### Download em arquivo ZIP
1. Na página inicial deste repositório no GitHub, clique no botão verde **`<Code>`**.
2. Selecione **Download ZIP**.
3. Após o download, extraia a pasta em qualquer local do seu computador (ex: Área de Trabalho ou Documentos).

---

## 📂 3. Abrindo o Projeto no VS Code

Abra o Visual Studio Code.

No menu superior, clique em File (Arquivo) ➔ Open Folder... (Abrir Pasta...) ou use o atalho Ctrl + K Ctrl + O.

Navegue até a pasta onde você descompactou o projeto (Elevador_Tcc) e clique em Selecionar Pasta.

Aguarde a inicialização: O PlatformIO irá identificar o arquivo platformio.ini e baixará automaticamente todas as bibliotecas e ferramentas necessárias para o Arduino ATmega328P. Uma barra azul/cinza aparecerá no rodapé do VS Code.

---

## 🛠️ 4. Compilando e Gravando no Arduino

Conecte o seu Arduino Uno ao computador usando o cabo USB.

Na barra inferior esquerda do VS Code (barra do PlatformIO), você verá os seguintes ícones:

✓ (Build): Clique neste ícone para compilar o código e verificar se não há erros.

→ (Upload): Clique neste ícone para gravar o firmware diretamente no Arduino Uno.

Aguarde até aparecer a mensagem [SUCCESS] no terminal do VS Code.

>### Erro no programa ao abrir ou compilar:
>Caso ao compilar apareça algum **erro** [FAILED] ou apareçam **problemas** ao abrir o programa, como na imagem, siga as orientações:
>
><img width="1366" height="768" alt="image" src="https://github.com/user-attachments/assets/aac8540e-335c-4c9a-8fe6-a4a6853d8a06" />
>
>## Recriar o índice do C/C++ no VS Code:
>### Passo 1.
>Pressione Ctrl + Shift + P, digite **PlatformIO: Rebuild C/C++ Project Index** e pressione Enter.
>
>**Para verificar se o passo funcionou:**
>
>Aguarde alguns segundos e verifique se as linhas vermelhas sob os #include começam a desaparecer.
>
>## Executar a compilação de teste:
>
>### Passo 2.
>
>Clique no ícone de verificação **✓** (Build) localizado na barra inferior azul do VS Code.
>
>**Para verificar se o passo funcionou:**
>
> O painel de Output/Terminal deve terminar o processo mostrando a mensagem [SUCCESS] em verde.
>
> Se mesmo depois do passo a passo o problema persistir, entre em contato comigo na aba [Discussões do GitHub](https://github.com/OarthurOtorres/Elevador_Tcc/discussions)
>

---

## 💻 5. Executando o Supervisório Web (SCADA)

### Passo 1: Emparelhar o Módulo HC-05 no Windows

Ligue o circuito do Arduino para alimentar o módulo HC-05 (o LED vermelho do HC-05 começará a piscar rapidamente).

No Windows, aceda a Definições > Bluetooth e dispositivos > Adicionar dispositivo.

Selecione a opção Bluetooth e escolha o dispositivo HC-05 na lista.

Quando solicitado o PIN de emparelhamento, introduza 1234 ou 0000 e confirme.

---

### Passo 2: Executar a Interface Web (HTML)

Com o **VS Code** aberto, clique em Supervisorio_html e clique com o botão direito sob o arquivo index.html e depois em **Abrir com Live Server/Open with Live Server** para abrir a interface, como na imagem:

<img width="1917" height="1078" alt="image" src="https://github.com/user-attachments/assets/f593e67f-d9e7-45f3-bd10-da2917ecfdea" />

O sistema supervisório será aberto no seu navegador padrão, ou aquele que estiver aberto.

---

### Passo 3: Estabelecer a Conexão Série no Navegador

Na página do supervisorio HTML, clique no botão de Conectar (ou Conectar Bluetooth / Porta Série).

O navegador exibirá uma janela pop-up a listar as portas de comunicação disponíveis.

Selecione a porta COM correspondente ao HC-05 identificada no Passo 2 e clique em Conectar.

O LED do módulo HC-05 mudará o padrão de pisca (passando a piscar em intervalos mais longos), indicando que a ligação série entre o supervisório HTML e o Arduino foi estabelecida com sucesso.

Mantenha o circuito do elevador energizado e o módulo Bluetooth ligado.

No seu computador, abra a pasta do projeto e entre em Supervisorio_html/.

Dê um duplo clique no arquivo index.html para abri-lo no seu navegador web (Chrome, Edge ou Firefox).

Realize a conexão Bluetooth com o módulo para monitorar o status dos andares e emergências em tempo real.

---

> 💬 **Dúvidas, Sugestões ou Feedback?**
> Para fazer perguntas sobre a montagem, sugerir melhorias ou conversar com a equipe, acesse a nossa aba de [Discussões do GitHub](https://github.com/OarthurOtorres/Elevador_Tcc/discussions).

---

[⬅️ Voltar para o README Principal](../../README.md)
