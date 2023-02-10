/**
 * @file main.h
 * @brief 
 * @author JoostAB (https://github.com/JoostAB)
 * @version 0.1
 * @date 2023-02-10
 */
# pragma once
#ifndef __MAIN_H__
#define __MAIN_H__

#include <general.h>
#include <mywifi.h>
#include <mykaku.h>
#include <mymqtt.h>
#include <Ticker.h>

Ticker ledflash;

void _flashLed_isr();
void kakuReceived(t_kakuaddress sender, unsigned long groupBit, unsigned long unit, unsigned long switchType);
void MQTTCmdReceived(const char* cmd);
void setup();
void loop();

#endif // __MAIN_H__