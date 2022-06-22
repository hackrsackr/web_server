#pragma once

#include "secrets.h"

// Number of sensor solenoid pairs
#define _NUMBER_OF_SPUNDERS 4

// WiFi credentials
#define _SSID SECRET_SSID
#define _PASS SECRET_PASS

// Name of each setpoint
#define _SPNM1 "setpoint-1"
#define _SPNM2 "setpoint-2"
#define _SPNM3 "setpoint-3"
#define _SPNM4 "setpoint-4"

std::array<const char*,  _NUMBER_OF_SPUNDERS> _SETPOINT_NAMES = { _SPNM1, _SPNM2, _SPNM3, _SPNM4 };

// Name of each setpoint data file
#define _FILE1 "/setpoint-1.txt"
#define _FILE2 "/setpoint-2.txt"
#define _FILE3 "/setpoint-3.txt"
#define _FILE4 "/setpoint-4.txt"

std::array<const char*,  _NUMBER_OF_SPUNDERS> _SETPOINT_FILES = { _FILE1, _FILE2, _FILE3, _FILE4 };
