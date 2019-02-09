#pragma once

#include <bitset>
#include <iostream>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "lora.hpp"

void register_write(uint8_t, uint8_t);
uint8_t register_read(uint8_t);
uint8_t handle_transfer(uint8_t , uint8_t);
void Init_gpio();
void lora_sleep_mode();
void lora_stby_mode();
void setOCP(uint8_t mA);
void setTxPower(int level);
int initialize(long frequency);
void freq_config(long frequency);
bool txInProgress();
int TXbegin();
int transmitNow();
void sendData(uint8_t* txData, uint8_t size);
void received_data();
void binary_print(uint8_t temp);
void Init_gpio_spi();
int parsePacket();
void transmitter();
uint8_t readData();
void receiver();
void received_data();