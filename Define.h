#pragma once

#ifndef MODEL
#define MODEL "RD_HC_FULL"
#endif

#define STR_(x) #x
#define STR(x) STR_(x)

#ifndef VERSION
#define VERSION 2.0.1
#endif

#define CONFIG_FILE_NAME "/root/smh/config.json"

#ifndef DB_NAME
#define DB_NAME "/root/smh/smh.sqlite"
#endif

#ifndef BLE_UART_PORT
#define BLE_UART_PORT "/dev/ttyUSB0"
#endif

