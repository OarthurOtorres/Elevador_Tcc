#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>

void BluetoothInit();
void atualizarInterfaceBluetooth();
void lerComandosBluetooth();
void enviarLog(const String& mensagem);

#endif