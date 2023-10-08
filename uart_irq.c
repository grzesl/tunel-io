#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define RX_BUFFER_SIZE 256
#define TX_BUFFER_SIZE 256

typedef struct TUART_IRQ {
int uartid;
int uartIRQ;
uart_inst_t * uart;

unsigned char *comRx_buf;											// pointer to the buffer for received characters
volatile int comRx_head, comRx_tail;								// head and tail of the ring buffer for com2 Rx
unsigned char *comTx_buf;											// pointer to the buffer for transmitted characters
volatile int comTx_head, comTx_tail;
}UART_IRQ;

UART_IRQ UARTIO [2];

static char glob_com0Rx_buf[RX_BUFFER_SIZE];
static char glob_com0Tx_buf[TX_BUFFER_SIZE];
static char glob_com1Rx_buf[RX_BUFFER_SIZE];
static char glob_com1Tx_buf[TX_BUFFER_SIZE];

void on_uart_irq1();
void on_uart_irq0();

void init_uart_io()
{
UARTIO[0].uartid = 0;
UARTIO[0].uartIRQ = UART0_IRQ;
UARTIO[0].uart = uart0;
UARTIO[0].comRx_buf = glob_com0Rx_buf;
UARTIO[0].comTx_buf = glob_com0Tx_buf;

UARTIO[1].uartid = 1;
UARTIO[1].uartIRQ = UART1_IRQ;
UARTIO[1].uart = uart1;
UARTIO[1].comRx_buf = glob_com1Rx_buf;
UARTIO[1].comTx_buf = glob_com1Tx_buf;
}

/***************************************************************************************************
Initialise the serial function including the interrupts.
****************************************************************************************************/
#define UART_ID  (uart ? UARTIO[1]: UARTIO[0])
void setupuart(int uart, int s2,int parity, int b7, int baud){
	uart_init(UART_ID.uart,baud);
    uart_set_hw_flow(UART_ID.uart, false, false);
    uart_set_format(UART_ID.uart, b7, s2, parity);
    uart_set_fifo_enabled(UART_ID.uart, false);
    if(uart){
		irq_set_exclusive_handler(UART_ID.uartIRQ, on_uart_irq1);
    	irq_set_enabled(UART_ID.uartIRQ, true);
	} else {
		irq_set_exclusive_handler(UART_ID.uartIRQ, on_uart_irq0);
    	irq_set_enabled(UART_ID.uartIRQ, true);
	}
    uart_set_irq_enables(UART_ID.uart, true, false);
}
/***************************************************************************************************
Add a character to the serial output buffer.
****************************************************************************************************/
unsigned char SerialPutchar(int uart, unsigned char c) {
        int empty=uart_is_writable(UART_ID.uart);
		if(UART_ID.comTx_tail == ((UART_ID.comTx_head + 1) % TX_BUFFER_SIZE)); //wait if buffer full
		UART_ID.comTx_buf[UART_ID.comTx_head] = c;							// add the char
		UARTIO[uart].comTx_head = (UART_ID.comTx_head + 1) % TX_BUFFER_SIZE;		   // advance the head of the queue
        if(empty){
	        	uart_set_irq_enables(UART_ID.uart, true, true);
			irq_set_pending(UART_ID.uartIRQ);
		}
	}

int SerialGetchar(int uart) {
	int c;
    	c = -1;                                                         // -1 is no data
	    uart_set_irq_enables(UART_ID.uart, false, true);
		if(UART_ID.comRx_head != UART_ID.comRx_tail) {                            // if the queue has something in it
			c = UART_ID.comRx_buf[UART_ID.comRx_tail];                            // get the char
 			UARTIO[uart].comRx_tail = (UART_ID.comRx_tail + 1) % RX_BUFFER_SIZE;        // and remove from the buffer
		}
		uart_set_irq_enables(UART_ID.uart, true, true);
    return c;
	}

void on_uart_irq0() {
    if(uart_is_readable(UARTIO[0].uart)) {
		UARTIO[0].comRx_buf[UARTIO[0].comRx_head]  = uart_getc(UARTIO[0].uart);   // store the byte in the ring buffer
		UARTIO[0].comRx_head = (UARTIO[0].comRx_head + 1) % RX_BUFFER_SIZE;     // advance the head of the queue
		if(UARTIO[0].comRx_head == UARTIO[0].comRx_tail) {                           // if the buffer has overflowed
			UARTIO[0].comRx_tail = (UARTIO[0].comRx_tail + 1) % RX_BUFFER_SIZE; // throw away the oldest char
		}
    }
    if(uart_is_writable(UARTIO[0].uart)){
		if(UARTIO[0].comTx_head != UARTIO[0].comTx_tail) {
			uart_putc_raw(UARTIO[0].uart,UARTIO[0].comTx_buf[UARTIO[0].comTx_tail]);
			UARTIO[0].comTx_tail = (UARTIO[0].comTx_tail + 1) % TX_BUFFER_SIZE;       // advance the tail of the queue
		} else {
			uart_set_irq_enables(UARTIO[0].uart, true, false);
		}
    }
}
void on_uart_irq1() {
	    while (uart_is_readable(UARTIO[1].uart)) {
		UARTIO[1].comRx_buf[UARTIO[1].comRx_head]  = uart_getc(UARTIO[1].uart);   // store the byte in the ring buffer
		UARTIO[1].comRx_head = (UARTIO[1].comRx_head + 1) % RX_BUFFER_SIZE;     // advance the head of the queue
		if(UARTIO[1].comRx_head == UARTIO[1].comRx_tail) {                           // if the buffer has overflowed
			UARTIO[1].comRx_tail = (UARTIO[1].comRx_tail + 1) % RX_BUFFER_SIZE; // throw away the oldest char
		}
    }
    if(uart_is_writable(UARTIO[1].uart)){
		if(UARTIO[1].comTx_head != UARTIO[1].comTx_tail) {
			uart_putc_raw(UARTIO[1].uart,UARTIO[1].comTx_buf[UARTIO[1].comTx_tail]);
			UARTIO[1].comTx_tail = (UARTIO[1].comTx_tail + 1) % TX_BUFFER_SIZE;       // advance the tail of the queue
		} else {
			uart_set_irq_enables(UARTIO[1].uart, true, false);
		}
    }
}