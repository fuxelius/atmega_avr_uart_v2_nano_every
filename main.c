/*
 *     main.c
 * 
 *          Description:  UART test session 
 *          Author:       Hans-Henrik Fuxelius   
 *          Date:         Uppsala, 2023-05-24 
 *          License:      MIT
 *          Version:      RC1     
 */

#define F_CPU 2666666

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

int main(void) {

    uint16_t c;
    uint8_t j=0;

    char buffer[100];

    // (0) - USART settings; 
    usart_set(&usart3, &PORTB, PORTMUX_USART3_ALT1_gc, PIN4_bm, PIN5_bm);

    while (1) {

        // (1) - Init USART
        usart_init(&usart3, (uint16_t)BAUD_RATE(9600));

        // (2) - Enable global interrupts
        sei(); 

        // (3) - Send string to USART
        usart_send_string(&usart3, "\r\n\r\nLove & Peace!\r\n\r\n");

        // (4) - Use sprintf_P, fputs to write to stream
        sprintf_P(buffer, PSTR("Hello world!\r\n"));
        fputs(buffer, &usart3_stream);

        for(size_t i=0; i<5; i++) {
            // (5) - Use formatted fprintf_P to write to stream
            fprintf_P(&usart3_stream, PSTR("\r\nCounter value: 0x%02X, "), j++);
            _delay_ms(500);

            // (6) - Get Rx count with usart_rx_count()
            sprintf_P(buffer, PSTR("rx count: %i "), usart_rx_count(&usart3));
            fputs(buffer, &usart3_stream);            

            // (7) - Get USART input by polling ringbuffer
            while(!((c = usart_read_char(&usart3)) & USART_NO_DATA)) {

                if (c & USART_PARITY_ERROR) {
                    usart_send_string_P(&usart3, PSTR("USART PARITY ERROR:\r\n"));
                }
                if (c & USART_FRAME_ERROR) {
                    usart_send_string_P(&usart3, PSTR("USART FRAME ERROR:\r\n"));
                }
                if (c & USART_BUFFER_OVERFLOW) {
                    usart_send_string_P(&usart3, PSTR("USART BUFFER OVERFLOW ERROR:\r\n"));
                }

                // (8) - Send single character to USART
                usart_send_char(&usart3, (char)c);
            }
        }

        // (9) - Check that everything is printed before closing USART
        fprintf_P(&usart3_stream, PSTR("\r\n\r\n<-<->->"));

        // (10) - Close USART3
        usart_close(&usart3);    

        // (11) - Clear global interrupts
        cli();

    }
}