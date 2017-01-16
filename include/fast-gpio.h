#ifndef _FAST_GPIO_H_
#define _FAST_GPIO_H_

#include <stdint.h>

#define _GPIO_PA 0
#define _GPIO_PB 1
#define _GPIO_PC 2
#define _GPIO_PD 3
#define _GPIO_PE 4
#define _GPIO_PF 5
#define _GPIO_PG 6

#define _GPIO_PULL_DISABLE 0
#define _GPIO_PULL_UP 1
#define _GPIO_PULL_DOWN 2
#define _GPIO_PULL_UNKNOWN 3

#define _GPIO_MUX_IN 0
#define _GPIO_MUX_OUT 1

#define _GPIO_HIGH 1
#define _GPIO_LOW 0


//Returns 0 on success, non-zero on error
//1 == couldn't open /dev/mem, 2 == error mmapping memory
int initGpio();
//Mux 0 is INPUT, 1 is OUTPUT and the rest are defined per-pin
//Mux can be 0-6
int readMux(int port, int pin);
void writeMux(int port, int pin, uint8_t mux);
//Port can be 0-6, ie. port A to port G
//The code does NO checking if you try to use an illegal
//port- or pin-number!
int readPin(int port, int pin);
void writePin(int port, int pin, uint8_t value);
//0 is no pull, 1 is pull-up, 2 is pull-down, 3 undefined
uint8_t readPull(int port, int pin);
void writePull(int port, int pin, uint8_t value);
char *muxToString(uint8_t port, uint8_t pin, uint8_t mux);
//Functions for manipulating the whole data-register for the port
uint32_t readPort(int port);
void writePort(int port, uint32_t data);
//Set the bits HIGH for the pins you want cleared, ie. LOW
void clearPort(int port, uint32_t mask);
//Set the bits HIGH for the pins you want set, ie. HIGH
void setPort(int port, uint32_t mask);

#endif
