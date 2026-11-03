#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>

void BluetoothInit();
void enviarLog(const String& mensagem);
void enviarEstadoCompleto();
void atualizarInterfaceBluetooth();
void lerComandosBluetooth();

#endif