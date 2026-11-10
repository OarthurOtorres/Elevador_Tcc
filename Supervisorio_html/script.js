let portaSerial = null, escritor = null, leitor = null;
let somHabilitado = true;
let audioCtx = null;

// Objetos Web Audio para a Sirene Modulada
let oscSirene = null;
let lfoSirene = null;
let lfoGain = null;
let gainSirene = null;

let contadorViagens = 0, contadorAlertas = 0;
let ultimoAndarRegistrado = null, tempoInicioConexao = null, timerUptime = null;
let paradasA1 = 0, paradasA2 = 0, paradasA3 = 0;

const statusTxt = document.getElementById('statusTxt');
const relogioTxt = document.getElementById('relogioTxt');
const btnConectar = document.getElementById('btnConectar');
const btnSom = document.getElementById('btnSom');
const logTerminal = document.getElementById('logTerminal');
const cabineElevador = document.getElementById('cabineElevador');
const cntViagens = document.getElementById('cntViagens');
const cntAlertas = document.getElementById('cntAlertas');
const icoSentido = document.getElementById('icoSentido');
const txtSentido = document.getElementById('txtSentido');
const valUptime = document.getElementById('valUptime');
const ledP = document.getElementById('ledP');
const lblPorta = document.getElementById('lblPorta');
const ledIR = document.getElementById('ledIR');
const lblIR = document.getElementById('lblIR');

function initAudio() {
    if (!audioCtx) {
        audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    }
    if (audioCtx.state === 'suspended') {
        audioCtx.resume();
    }
}

// SOM DING-DONG REALISTA (Mi5 -> Do5)
function tocarDingDong() {
    if (!somHabilitado) return;
    initAudio();
    if (!audioCtx) return;

    const now = audioCtx.currentTime;

    // DING
    const osc1 = audioCtx.createOscillator();
    const gain1 = audioCtx.createGain();
    osc1.type = 'sine';
    osc1.frequency.setValueAtTime(659.25, now);
    gain1.gain.setValueAtTime(0.35, now);
    gain1.gain.exponentialRampToValueAtTime(0.001, now + 0.8);
    osc1.connect(gain1);
    gain1.connect(audioCtx.destination);
    osc1.start(now);
    osc1.stop(now + 0.8);

    // DONG
    const osc2 = audioCtx.createOscillator();
    const gain2 = audioCtx.createGain();
    osc2.type = 'sine';
    osc2.frequency.setValueAtTime(523.25, now + 0.35);
    gain2.gain.setValueAtTime(0.35, now + 0.35);
    gain2.gain.exponentialRampToValueAtTime(0.001, now + 1.2);
    osc2.connect(gain2);
    gain2.connect(audioCtx.destination);
    osc2.start(now + 0.35);
    osc2.stop(now + 1.2);
}

// SIRENE REALISTA (WAIL INDUSTRIAL COM MODULAÇÃO LFO)
function iniciarSirene() {
    if (oscSirene || !somHabilitado) return;
    initAudio();
    if (!audioCtx) return;

    const now = audioCtx.currentTime;

    oscSirene = audioCtx.createOscillator();
    lfoSirene = audioCtx.createOscillator();
    lfoGain = audioCtx.createGain();
    gainSirene = audioCtx.createGain();

    // Frequência base e formato de onda da sirene
    oscSirene.type = 'sawtooth';
    oscSirene.frequency.setValueAtTime(850, now);

    // Oscilador de baixa frequência (LFO) para varrimento contínuo (Wail)
    lfoSirene.type = 'sine';
    lfoSirene.frequency.setValueAtTime(1.2, now); // Velocidade do ciclo da sirene (1.2 Hz)
    lfoGain.gain.setValueAtTime(350, now);        // Variação de pitch (+/- 350 Hz => 500 Hz a 1200 Hz)

    lfoSirene.connect(lfoGain);
    lfoGain.connect(oscSirene.frequency);

    gainSirene.gain.setValueAtTime(0.18, now);

    oscSirene.connect(gainSirene);
    gainSirene.connect(audioCtx.destination);

    oscSirene.start(now);
    lfoSirene.start(now);
}

function pararSirene() {
    if (oscSirene) {
        try {
            oscSirene.stop();
            lfoSirene.stop();
            oscSirene.disconnect();
            lfoSirene.disconnect();
        } catch(e){}
        oscSirene = null;
        lfoSirene = null;
    }
}

function tocarSomClique() {
    if (!somHabilitado) return;
    initAudio();
    if (!audioCtx) return;
    const now = audioCtx.currentTime;
    const osc = audioCtx.createOscillator();
    const gain = audioCtx.createGain();
    osc.type = 'sine';
    osc.frequency.setValueAtTime(700, now);
    gain.gain.setValueAtTime(0.1, now);
    gain.gain.exponentialRampToValueAtTime(0.001, now + 0.05);
    osc.connect(gain);
    gain.connect(audioCtx.destination);
    osc.start(now);
    osc.stop(now + 0.05);
}

function tocarSomReset() {
    if (!somHabilitado) return;
    initAudio();
    if (!audioCtx) return;
    const now = audioCtx.currentTime;
    const osc = audioCtx.createOscillator();
    const gain = audioCtx.createGain();
    osc.type = 'triangle';
    osc.frequency.setValueAtTime(587.33, now);
    osc.frequency.setValueAtTime(880, now + 0.1);
    gain.gain.setValueAtTime(0.2, now);
    gain.gain.exponentialRampToValueAtTime(0.01, now + 0.3);
    osc.connect(gain);
    gain.connect(audioCtx.destination);
    osc.start(now);
    osc.stop(now + 0.3);
}

// TECLAS DE FUNÇÃO (F1 a F5)
window.addEventListener('keydown', (e) => {
    if (e.key === 'F1') { e.preventDefault(); enviarComando('1'); }
    else if (e.key === 'F2') { e.preventDefault(); enviarComando('2'); }
    else if (e.key === 'F3') { e.preventDefault(); enviarComando('3'); }
    else if (e.key === 'F4') { e.preventDefault(); enviarComando('R'); }
    else if (e.key === 'F5') { e.preventDefault(); enviarComando('E'); }
});

function atualizarRelogio() {
    const agora = new Date();
    relogioTxt.innerText = agora.toLocaleDateString() + ' ' + agora.toLocaleTimeString();
}
setInterval(atualizarRelogio, 1000);
atualizarRelogio();

function iniciarUptime() {
    tempoInicioConexao = Date.now();
    if (timerUptime) clearInterval(timerUptime);
    timerUptime = setInterval(() => {
        if (!tempoInicioConexao) return;
        const diff = Math.floor((Date.now() - tempoInicioConexao) / 1000);
        const h = String(Math.floor(diff / 3600)).padStart(2, '0');
        const m = String(Math.floor((diff % 3600) / 60)).padStart(2, '0');
        const s = String(diff % 60).padStart(2, '0');
        valUptime.innerText = `${h}:${m}:${s}`;
    }, 1000);
}

function exportarLog() {
    const conteudoLog = logTerminal.innerText;
    if (!conteudoLog || conteudoLog.trim() === "Aguardando conexão com o elevador...") return;
    const blob = new Blob([conteudoLog], { type: "text/plain;charset=utf-8" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = `log_elevador_${new Date().toISOString().split('T')[0]}.txt`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
}

function alternarSom() {
    somHabilitado = !somHabilitado;
    if (!somHabilitado) pararSirene();
    btnSom.innerText = somHabilitado ? "🔊 Som: LIGADO" : "🔇 Som: DESLIGADO";
    btnSom.classList.toggle('mutado', !somHabilitado);
    if (somHabilitado) tocarSomClique();
}

btnConectar.addEventListener('click', async () => {
    initAudio();
    try {
        statusTxt.innerText = "Conectando...";
        portaSerial = await navigator.serial.requestPort();
        await portaSerial.open({ baudRate: 9600 });
        escritor = portaSerial.writable.getWriter();
        statusTxt.innerText = "Status: Conectado!";
        statusTxt.style.color = "#00adb5";
        logTerminal.innerHTML = "[SISTEMA]: Conexão Serial/Bluetooth Estabelecida.\n";
        iniciarUptime();
        tocarSomClique();
        
        setTimeout(() => enviarComando('Q'), 300);

        lerDadosDaSerial();
    } catch (erro) {
        statusTxt.innerText = "Status: Erro Conexão";
        statusTxt.style.color = "#ff4d4d";
    }
});

async function enviarComando(caractere) {
    tocarSomClique();
    if (!escritor) return;
    const encoder = new TextEncoder();
    await escritor.write(encoder.encode(caractere));
}

async function lerDadosDaSerial() {
    const textDecoder = new TextDecoderStream();
    portaSerial.readable.pipeTo(textDecoder.writable);
    leitor = textDecoder.readable.getReader();
    let bufferAcumulado = "";

    while (true) {
        const { value, done } = await leitor.read();
        if (done) break;
        if (value) {
            bufferAcumulado += value;
            const linhas = bufferAcumulado.split('\n');
            bufferAcumulado = linhas.pop();

            linhas.forEach(linha => {
                let linhaLimpa = linha.trim();
                if (!linhaLimpa) return;

                if (linhaLimpa.startsWith('*')) {
                    processarTagEspecial(linhaLimpa);
                } else {
                    if (!/^E:[01]$/i.test(linhaLimpa) && !/^E[01]$/i.test(linhaLimpa)) {
                        adicionarAoLog(linhaLimpa);
                    }
                }
            });
        }
    }
}

function processarTagEspecial(tag) {
    // CONTAGEM DE VIAGENS ACUMULADAS
    if (tag.startsWith('*V')) {
        contadorViagens = parseInt(tag.substring(2)) || 0;
        cntViagens.innerText = contadorViagens;
        return;
    }

    // MOTOR
    if (tag === '*MU') {
        icoSentido.innerText = "⬆️";
        txtSentido.innerText = "SUBINDO";
        txtSentido.style.color = "#00adb5";
    } else if (tag === '*MD') {
        icoSentido.innerText = "⬇️";
        txtSentido.innerText = "DESCENDO";
        txtSentido.style.color = "#f0ad4e";
    } else if (tag === '*MS') {
        icoSentido.innerText = "⏸️";
        txtSentido.innerText = "PARADO";
        txtSentido.style.color = "#00adb5";
    }

    // SENSOR IR DE PRESENÇA
    else if (tag === '*IR1') {
        ledIR.classList.add('ir-detectado');
        lblIR.innerText = "IR Presença: Detectado";
    } else if (tag === '*IR0') {
        ledIR.classList.remove('ir-detectado');
        lblIR.innerText = "IR Presença: Livre";
    }

    // ESTADO DA PORTA (ACIONA O DING-DONG AO ABRIR)
    else if (tag === '*P1') {
        ledP.classList.add('porta-aberta');
        lblPorta.innerText = "Porta Aberta";
        tocarDingDong();
    } else if (tag === '*P0') {
        ledP.classList.remove('porta-aberta');
        lblPorta.innerText = "Porta Fechada";
    }

    // SENSORES DE ANDAR
    else if (tag.startsWith('*S')) {
        const num = tag.charAt(2);
        const st = tag.charAt(3);
        const el = document.getElementById("ledS" + num);

        if (el) {
            if (st === '1') {
                el.classList.add('sensor-ativo');
                
                if (num === '1') { cabineElevador.style.bottom = '10px'; cabineElevador.innerText = '1º'; }
                else if (num === '2') { cabineElevador.style.bottom = 'calc(50% - 21px)'; cabineElevador.innerText = '2º'; }
                else if (num === '3') { cabineElevador.style.bottom = 'calc(100% - 52px)'; cabineElevador.innerText = '3º'; }

                if (num === '1') { paradasA1++; document.getElementById('statA1').innerText = paradasA1 + 'x'; }
                if (num === '2') { paradasA2++; document.getElementById('statA2').innerText = paradasA2 + 'x'; }
                if (num === '3') { paradasA3++; document.getElementById('statA3').innerText = paradasA3 + 'x'; }
                
                atualizarGraficoDemanda();
                ultimoAndarRegistrado = num;
            } else {
                el.classList.remove('sensor-ativo');
            }
        }
    }

    // CHAMADAS E EMERGÊNCIA
    else {
        const idLed = "led" + tag.charAt(1);
        const status = tag.charAt(2);
        const elemento = document.getElementById(idLed);

        if (idLed === 'ledX') {
            if (status === '1') {
                elemento.classList.add('emergencia-ativa');
                document.body.classList.add('emergencia-ativa');
                icoSentido.innerText = "🚨";
                txtSentido.innerText = "BLOQUEADO";
                txtSentido.style.color = "#ff4d4d";
                contadorAlertas++;
                cntAlertas.innerText = contadorAlertas;
                iniciarSirene();
                adicionarAoLog("[ALERTA]: Emergência do sistema ativada!");
            } else {
                elemento.classList.remove('emergencia-ativa');
                document.body.classList.remove('emergencia-ativa');
                icoSentido.innerText = "⏸️";
                txtSentido.innerText = "PARADO";
                txtSentido.style.color = "#00adb5";
                pararSirene();
                tocarSomReset();
                adicionarAoLog("[SISTEMA]: Emergência desativada / Normalizado.");
            }
            return;
        }

        if (elemento) {
            let classeAlvo = (idLed === 'ledR') ? 'reset-ativo' : 'chamada-ativa';
            if (status === '1') elemento.classList.add(classeAlvo);
            else elemento.classList.remove(classeAlvo);
        }
    }
}

function atualizarGraficoDemanda() {
    const total = paradasA1 + paradasA2 + paradasA3;
    if (total === 0) return;
    document.getElementById('barA1').style.width = Math.round((paradasA1 / total) * 100) + '%';
    document.getElementById('pctA1').innerText = Math.round((paradasA1 / total) * 100) + '%';
    document.getElementById('barA2').style.width = Math.round((paradasA2 / total) * 100) + '%';
    document.getElementById('pctA2').innerText = Math.round((paradasA2 / total) * 100) + '%';
    document.getElementById('barA3').style.width = Math.round((paradasA3 / total) * 100) + '%';
    document.getElementById('pctA3').innerText = Math.round((paradasA3 / total) * 100) + '%';
}

function adicionarAoLog(mensagem) {
    const hora = new Date().toLocaleTimeString();
    logTerminal.innerText += `[${hora}] ${mensagem}\n`;
    logTerminal.scrollTop = logTerminal.scrollHeight;
}