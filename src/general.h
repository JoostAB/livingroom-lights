/**
 * @file general.h
 * @brief 
 * @author JoostAB (https://github.com/JoostAB)
 * @version 0.1
 * @date 2023-02-10
 */
# pragma once
#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <Arduino.h>

const char* cmdOn = "ON";
const char* cmdOff = "OFF";
const char* cmdReCfg = "RECFG";
const char* cmdWebCfg = "WEBCFG";
const char* cmdOtaOn = "OTAON";
const char* cmdOtaOff = "OTAOFF";
const char* cmdReboot = "REBOOT";

#include <jbdebug.h>
#include <config.h>

#endif // __GENERAL_H__