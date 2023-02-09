/**
 * @file jbdebug.h
 * @brief Some macros to enabled debuglogging.
 * @author JoostAB (https://github.com/JoostAB)
 * @version 0.1
 * @date 2022-01-06
 * By passing the DEBUGLOG=1 buildflag, these macro's are enabled and will log to std Serial.
 * It will also start the default serial output, using the baudrate set with SERIALBAUDRATE, either 
 * defined or passed as buildflag.
 * If no DEBUGLOG is set, or set to 0, then all these macro's end up in nothing.
 */
# pragma once
#ifndef _JB_DEBUG_H
#define _JB_DEBUG_H

#include <Arduino.h>

// Debugging switches and macros
// #ifndef DEBUGLOG
// # define DEBUGLOG 0 // Switch debug output on and off by 1 or 0
// #endif

#define XQUOTE(x) #x
#define QUOTE(x) XQUOTE(x)

// #define DEBUG_ESP_WIFI
// #define DEBUG_ESP_PORT

#ifdef SERIALBAUDRATE
#define _DEFSPEED SERIALBAUDRATE
#else
#define _DEFSPEED 115200
#endif

#if DEBUGLOG
#define IFDEBUG(s)                   { s }
#define DEBUGSTART(s)                { Serial.begin(s); }
#define DEBUGSTARTDEF                  DEBUGSTART(_DEFSPEED)
#define PRINTS(s)                    { Serial.print(F(s)); }
#define PRINTDS(s)                   { Serial.print(s); }
#define PRINT(s,v)                   { Serial.print(F(s)); Serial.print(v); }
#define PRINTF(s,v1)                 { Serial.printf(s, v1); }
#define PRINTF2(s,v1,v2)             { Serial.printf(s, v1, v2); }
#define PRINTF3(s,v1,v2,v3)          { Serial.printf(s, v1, v2, v3); }
#define PRINTF4(s,v1,v2,v3,v4)       { Serial.printf(s, v1, v2, v3, v4); }
#define PRINTF5(s,v1,v2,v3,v4,v5)    { Serial.printf(s, v1, v2, v3, v4, v5); }
#define PRINTF6(s,v1,v2,v3,v4,v5,v6) { Serial.printf(s, v1, v2, v3, v4, v5, v6); }
#define PRINTLNS(s)                  { Serial.println(F(s)); }
#define PRINTLNSA(s)                 { Serial.println(s); }
#define PRINTLN(s,v)                 { Serial.print(F(s)); Serial.println(v); }
#define PRINTLF                      Serial.println();

#else
// No debugging, replace all PRINT*** calls with nothing
#define IFDEBUG(s)                 
#define DEBUGSTART(s)                
#define DEBUGSTARTDEF                
#define PRINTS(s)
#define PRINTDS(s)
#define PRINT(s,v)
#define PRINTX(s,v)
#define PRINTF(s,v1)
#define PRINTF2(s,v1,v2)
#define PRINTF3(s,v1,v2,v3)
#define PRINTF4(s,v1,v2,v3,v4)
#define PRINTF5(s,v1,v2,v3,v4,v5)
#define PRINTF6(s,v1,v2,v3,v4,v5,v6)
#define PRINTLNS(s)
#define PRINTLNSA(s)
#define PRINTLN(s,v)
#define PRINTLNX(s,v)
#define PRINTLF
#endif

#endif // _JB_DEBUG_H
