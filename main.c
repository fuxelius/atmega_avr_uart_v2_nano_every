/*
 *     main.c
 *
 *          Project:  Test of USART for ATmega4808
 *          Author:   Hans-Henrik Fuxelius   
 *          Date:     Uppsala, 2023-05-24          
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

int main(void) {

    uint16_t c;
    uint8_t j=0;

    // (0) - USART settings; 
    usart_set(&usart3, &PORTB, PORTMUX_USART3_ALT1_gc, PIN4_bm, PIN5_bm);

    while (1) {

        // (1) - Init USART
        usart_init(&usart3, (uint16_t)BAUD_RATE(9600));

        // (2) - Enable global interrupts
        sei(); 

        // (3) - Send string to USART
        usart_send_string(&usart3, "\r\n\r\nPEACE BRO!\r\n\r\n", 18);

        // (4) - Use fprintf to write to stream
        fprintf(&usart3_stream, "Hello world!\r\n");

        for(size_t i=0; i<5; i++) {
            // (5) - Use formatted fprintf to write to stream
            fprintf(&usart3_stream, "\r\nCounter value is: 0x%02X ", j++);
            _delay_ms(500);

            // (6) - Get USART input by polling ringbuffer
            while(!((c = usart_read_char(&usart3)) & USART_NO_DATA)) {

                if (c & USART_PARITY_ERROR) {
                    fprintf(&usart3_stream, "USART PARITY ERROR: ");
                }
                if (c & USART_FRAME_ERROR) {
                    fprintf(&usart3_stream, "USART FRAME ERROR: ");
                }
                if (c & USART_BUFFER_OVERFLOW) {
                    fprintf(&usart3_stream, "USART BUFFER OVERFLOW ERROR: ");
                }

                // (7) - Send single character to USART
                usart_send_char(&usart3, (char)c);
            }
        }

        // (8) - Check that everything is printed before closing USART
        fprintf(&usart3_stream, "\r\n\r\n<-<->->");

        // (9) - Close USART0
        usart_close(&usart3);    

        // (10) - Clear global interrupts
        cli();

    }
}