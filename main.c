/**
 * Copyright (c) 2023 grzesl
 *
 * License: BSD-3-Clause
 */


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "config.h"

#define UART1_TX_PIN 0
#define UART1_RX_PIN 1

#define UART2_TX_PIN 4
#define UART2_RX_PIN 5


void lcd_init();
void lcd_loop();
void init_uart_io();
void setupuart(int uart, int s2,int parity, int b7, int baud);
unsigned char SerialPutchar(int uart, unsigned char c);
int SerialGetchar(int uart);

void* run_tcp_server(int port);
int step_tcp_server(void *arg);

int tcp_server_read(void *arg, char*data, int data_len);
int tcp_server_write(void *arg, char*data, int data_len);

#define IO_BUFF_LEN 256
void* server_state;
int main () { 
    
    char buff [IO_BUFF_LEN];
    stdio_init_all();
    lcd_init();
    printf("Starting...\n");

    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART2_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART2_RX_PIN, GPIO_FUNC_UART); 

    init_uart_io();
    setupuart(0, 1, 0, 8, 9600);
    setupuart(1, 1, 0, 8, 9600);

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    UG_ConsolePutString("Try to connect WiFI " );
    UG_ConsolePutString(WIFI_SSID );
    UG_ConsolePutString("...\n" );

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        return 1;
    } else {
        printf("Connected.\n");
    }

    
    server_state = run_tcp_server(4242);



    while(server_state)
    {
        int c = 0;
        c = SerialGetchar(0);
        if (c >= 0)
        {
            SerialPutchar(1,c);
            tcp_server_write (server_state, (char*)&c,1);
        }else 
        {
         c = getchar_timeout_us(0);
            if (c >= 0)
            {
                SerialPutchar(1,c);
                tcp_server_write (server_state, (char*)&c,1);
            }
        }

        c = SerialGetchar(1);
        if (c >= 0)
        {
            putchar_raw(c);
            SerialPutchar(0, c);
            tcp_server_write(server_state, (char *)&c, 1);
        }


        if (step_tcp_server(server_state))
            break;

        int read = tcp_server_read(server_state, buff, IO_BUFF_LEN);

            buff[read] = 0;
            UG_ConsolePutString(buff);

            for(int i = 0 ; i < read; i++)
            {
            
            putchar_raw(buff[i]);
            SerialPutchar(0,buff[i]);
            SerialPutchar(1,buff[i]);
        }


        lcd_loop();


    }

    cyw43_arch_deinit();

    return 0;
}
