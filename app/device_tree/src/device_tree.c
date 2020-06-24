#include "_project.h"

#include "dev_management_api.h"



#if 1

/*************   UART SHELL *****************/

/***********************************/
/********** esp8266_uart_dev ********/
#define DT_DEV_NAME                        esp8266_uart_dev
#define DT_DEV_MODULE                      uart_linux

#define UART_LINUX_DT_TX_CALLBACK_PDEV      esp8266_uart_tx_wrap_dev
#define UART_LINUX_DT_RX_CALLBACK_PDEV      esp8266_uart_rx_wrap_dev
#define UART_LINUX_DT_TTY_DEV_PATH           "/dev/ttyUSB0"
#define UART_LINUX_DT_BAUD_RATE              115200

#include ADD_CURRENT_DEV


/***********************************/
/********** esp8266_uart_tx_wrap_dev ********/
#define DT_DEV_NAME                        esp8266_uart_tx_wrap_dev
#define DT_DEV_MODULE                      async_tx_wrapper

#define ASYNC_TX_WRAPPER_DT_SERVER_PDEV        esp8266_uart_dev

#include ADD_CURRENT_DEV


/***********************************/
/********** esp8266_uart_rx_wrap_dev ********/
#define DT_DEV_NAME                        esp8266_uart_rx_wrap_dev
#define DT_DEV_MODULE                      async_rx_wrapper

#define ASYNC_RX_WRAPPER_DT_SERVER_PDEV        esp8266_uart_dev
#define ASYNC_RX_WRAPPER_DT_CLIENT_PDEV        esp8266_dev
#define ASYNC_RX_WRAPPER_DT_RX_BUFFER_SIZE     4096

#include ADD_CURRENT_DEV


/***********************************/
/********** linux_timer_dev ********/
#define DT_DEV_NAME                        linux_timer_dev
#define DT_DEV_MODULE                      linux_timer

#include ADD_CURRENT_DEV



/***********************************/
/********** esp8266_timer_dev ********/
#define DT_DEV_NAME                        esp8266_timer_dev
#define DT_DEV_MODULE                      timer_wrapper

#define TIMER_WRAPPER_DT_HW_TIMER_PDEV        linux_timer_dev

#include ADD_CURRENT_DEV



/***********************************/
/********** esp8266_dev ********/
#define DT_DEV_NAME                        esp8266_dev
#define DT_DEV_MODULE                      ESP8266

#define ESP8266_DT_TIMER_PDEV         esp8266_timer_dev
#define ESP8266_DT_UART_TX_DEV        esp8266_uart_tx_wrap_dev
#define ESP8266_DT_UART_RX_DEV        esp8266_uart_rx_wrap_dev
#define ESP8266_DT_UART_DEV           esp8266_uart_dev

#include ADD_CURRENT_DEV

#endif
