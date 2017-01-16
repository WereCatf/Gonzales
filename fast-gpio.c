#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

#include "fast-gpio.h"

uint8_t *memmap;

char *strUnknownMux="UNKNOWN";
char *strInputMux="INPUT";
char *strOutputMux="OUTPUT";

char *muxNames[76][4]={
  "TWI0_SCK", NULL, NULL, NULL,
  "TWI0_SDA", NULL, NULL, NULL,
  "PWM", NULL, NULL, "EINT6",
  "IR_TX", NULL, NULL, "EINT17",
  "IR_RX", NULL, NULL, "EINT18",
  "SPI2_CS1", NULL, NULL, "EINT24",
  "TWI1_SCK", NULL, NULL, NULL,
  "TWI1_SDA", NULL, NULL, NULL,
  "TWI2_SCK", NULL, NULL, NULL,
  "TWI2_SDA", NULL, NULL, NULL,
  "NWE", "SPI0_MOSI", NULL, NULL,
  "NALE", "SPI0_MISO", NULL, NULL,
  "NCLE", "SPI0_CLK", NULL, NULL,
  "NCE1", "SPI0_CS0", NULL, NULL,
  "NCE0", NULL, NULL, NULL,
  "NRE", NULL, NULL, NULL,
  "NRB0", "SDC2_CMD", NULL, NULL,
  "NRB1", "SDC2_CLK", NULL, NULL,
  "NDQ0", "SDC2_D0", NULL, NULL,
  "NDQ1", "SDC2_D1", NULL, NULL,
  "NDQ2", "SDC2_D2", NULL, NULL,
  "NDQ3", "SDC2_D3", NULL, NULL,
  "NDQ4", "SDC2_D4", NULL, NULL,
  "NDQ5", "SDC2_D5", NULL, NULL,
  "NDQ6", "SDC2_D6", NULL, NULL,
  "NDQ7", "SDC2_D7", NULL, NULL,
  "NDQS", NULL, NULL, NULL,
  "LCD_D2", "UART2_TX", NULL, NULL,
  "LCD_D3", "UART2_RX", NULL, NULL,
  "LCD_D4", "UART2_CTS", NULL, NULL,
  "LCD_D5", "UART2_RTS", NULL, NULL,
  "LCD_D6", "ECRS", NULL, NULL,
  "LCD_D7", "ECOL", NULL, NULL,
  "LCD_D10", "ERXD0", NULL, NULL,
  "LCD_D11", "ERXD1", NULL, NULL,
  "LCD_D12", "ERXD2", NULL, NULL,
  "LCD_D13", "ERXD3", NULL, NULL,
  "LCD_D14", "ERXCK", NULL, NULL,
  "LCD_D15", "ERXERR", NULL, NULL,
  "LCD_D18", "ERXDV", NULL, NULL,
  "LCD_D19", "ETXD0", NULL, NULL,
  "LCD_D20", "ETXD1", NULL, NULL,
  "LCD_D21", "ETXD2", NULL, NULL,
  "LCD_D22", "ETXD3", NULL, NULL,
  "LCD_D23", "ETXEN", NULL, NULL,
  "LCD_CLK", "ETXCK", NULL, NULL,
  "LCD_DE", "ETXERR", NULL, NULL,
  "LCD_HSYNC", "EMDC", NULL, NULL,
  "LCD_VSYNC", "EMDIO", NULL, NULL,
  "TS_CLK", "CSI_PCLK", "SPI2_CS0", "EINT14",
  "TS_ERR", "CSI_MCLK", "SPI2_CLK", "EINT15",
  "TS_SYNC", "CSI_HSYNC", "SPI2_MOSI", NULL,
  "TS_DVLD", "CSI_VSYNC", "SPI2_MISO", NULL,
  "TS_D0", "CSI_D0", "SDC2_D0", NULL,
  "TS_D1", "CSI_D1", "SDC2_D1", NULL,
  "TS_D2", "CSI_D2", "SDC2_D2", NULL,
  "TS_D3", "CSI_D3", "SDC2_D3", NULL,
  "TS_D4", "CSI_D4", "SDC2_CMD", NULL,
  "TS_D5", "CSI_D5", "SDC2_CLK", NULL,
  "TS_D6", "CSI_D6", "UART1_TX", NULL,
  "TS_D7", "CSI_D7", "UART1_RX", NULL,
  "SDC0_D1", NULL, "JTAG_MS1", NULL,
  "SDC0_D0", NULL, "JTAG_DI1", NULL,
  "SDC0_CLK", NULL, "UART0_TX", NULL,
  "SDC0_CMD", NULL, "JTAG_DO1", NULL,
  "SDC0_D3", NULL, "UART0_TX", NULL,
  "SDC0_D2", NULL, "JTAG_CK1", NULL,
  "GPS_CLK", NULL, NULL, "EINT0",
  "GPS_SIG", NULL, NULL, "EINT1",
  "GPS_MAG", NULL, NULL, "EINT2",
  NULL, NULL, "UART1_TX", "EINT3",
  NULL, NULL, "UART1_RX", "EINT4",
  "SPI1_CS0", "UART3_TX", NULL, "EINT9",
  "SPI1_CLK", "UART3_RX", NULL, "EINT10",
  "SPI1_MOSI", "UART3_CTS", NULL, "EINT11",
  "SPI1_MISO", "UART3_RTS", NULL, "EINT12"
};

uint8_t pinsWithMux[]={
  10, //How many pins on this port have special mux, port B
  0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 10, 5, 15, 6, 16, 7, 17, 8, 18, 9, //Pin, muxNames[row]
  17, //Port C
  0, 10, 1, 11, 2, 12, 3, 13, 4, 14, 5, 15, 6, 16, 7, 17, 8, 18, 9, 19,
  10, 20, 11, 21, 12, 22, 13, 23, 14, 24, 15, 25, 19, 26,
  22, //Port D
  2, 27, 3, 28, 4, 29, 5, 30, 6, 31, 7, 32, 10, 33, 11, 34, 12, 35,
  13, 36, 14, 37, 15, 38, 18, 39, 19, 40, 20, 41, 21, 42, 22, 43,
  23, 44, 24, 45, 25, 46, 26, 47, 27, 48,
  12, //Port E
  0, 49, 1, 50, 2, 51, 3, 52, 4, 53, 5, 54, 6, 55, 7, 56, 8, 57,
  9, 58, 10, 59, 11, 60,
  6, //Port F
  0, 61, 1, 62, 2, 63, 3, 64, 4, 65, 5, 66,
  9, //Port G
  0, 67, 1, 68, 2, 69, 3, 70, 4, 71, 9, 72, 10, 73, 11, 74, 12, 75
};

uint8_t portOffsetData[] = { 0*0x24+0x10, 1*0x24+0x10, 2*0x24+0x10,
  3*0x24+0x10, 4*0x24+0x10, 5*0x24+0x10, 6*0x24+0x10 };

char *muxToString(uint8_t port, uint8_t pin, uint8_t mux)
{
  char *returnStr=strUnknownMux;
  if(port < 1 || port > 6 || pin > 31) return strUnknownMux;
  if(mux==0) return strInputMux;
  if(mux==1) return strOutputMux;
  if(mux==5) return strUnknownMux; //Manual gives no definition for such mux
  mux-=2;
  if(mux==4) mux=3; //Skip the "missing" mux above
  uint8_t i, _pins, _port, _row;
  _port=1;
  i=0;
  while(_port != port){
    _pins=pinsWithMux[i];
    i+=_pins*2+1;
    _port++;
  }
  _pins=pinsWithMux[i];
  //printf("Debug: %d, %d\n", i, _pins);
  i++;
  while(_pins){
    if(pinsWithMux[i]==pin){
      _row=pinsWithMux[i+1];
      returnStr=muxNames[_row][mux];
      if(returnStr==NULL) returnStr=strUnknownMux;
      break;
    }
    i+=2;
    _pins--;
  }
  return returnStr;
}

int initGpio()
{
  int fd = open("/dev/mem", O_RDWR|O_SYNC);
  if(fd < 0) return 1;
  // uint32_t addr = 0x01c20800 & ~(getpagesize() - 1);
  //Requires memmap to be on pagesize-boundary
  memmap=(uint8_t *)mmap(NULL, getpagesize()*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x01c20000);
  if(memmap == NULL) return 1;
  close(fd);

  //Set memmap to point to PIO-registers
  memmap=memmap+0x800;
  return 0;
}

int readMux(int port, int pin)
{
	volatile uint32_t *pioMem32, *configRegister;
	pioMem32=(uint32_t *)(memmap+port*0x24);
	configRegister=pioMem32+(pin >> 3);
	return *configRegister >> ((pin & 7) * 4) & 7;
}

void writeMux(int port, int pin, uint8_t mux)
{
	volatile uint32_t *pioMem32, *configRegister;
  uint32_t mask;

	pioMem32=(uint32_t *)(memmap+port*0x24);
	mux &= 7;
	configRegister=pioMem32+(pin >> 3);
	mask = ~(7 << ((pin & 7) * 4));
	*configRegister &= mask;
	*configRegister |= mux << ((pin & 7) * 4);
}

int readPin(int port, int pin)
{
	volatile uint32_t *pioMem32;
	pioMem32=(uint32_t *)(memmap+portOffsetData[port]);
	return (*pioMem32 >> pin) & 1;
}

void writePin(int port, int pin, uint8_t value)
{
	value &= 1;
	volatile uint32_t *pioMem32;
  uint32_t mask;
	pioMem32=(uint32_t *)(memmap+portOffsetData[port]);
	mask = ~(1 << pin);
	*pioMem32 &= mask;
  if(value)	*pioMem32 |= value << pin;
}

uint8_t readPull(int port, int pin)
{
	volatile uint32_t *pioMem32, *configRegister;
	pioMem32=(uint32_t *)(memmap+port*0x24+0x1c); //0x1c == pull-register
	configRegister=pioMem32+(pin >> 4);
	return *configRegister >> ((pin & 15) * 2) & 3;
}

void writePull(int port, int pin, uint8_t value)
{
	value &= 3;
	volatile uint32_t *pioMem32, *configRegister;
  uint32_t mask;
	pioMem32=(uint32_t *)(memmap+port*0x24+0x1c); //0x1c == pull-register
	configRegister=pioMem32+(pin >> 4);
  mask = ~(3 << ((pin & 15) * 2));
	*configRegister &= mask;
	*configRegister |= value << ((pin & 15) * 2);
}

uint32_t readPort(int port)
{
	volatile uint32_t *pioMem32;
	pioMem32=(uint32_t *)(memmap+portOffsetData[port]);
	return *pioMem32;
}

void writePort(int port, uint32_t data)
{
	volatile uint32_t *pioMem32;
	pioMem32=(uint32_t *)(memmap+portOffsetData[port]);
	*pioMem32=data;
}

void clearPort(int port, uint32_t mask)
{
	volatile uint32_t *pioMem32;
	pioMem32=(uint32_t *)(memmap+portOffsetData[port]);
	*pioMem32 &=~mask;
}

void setPort(int port, uint32_t mask)
{
	volatile uint32_t *pioMem32;
	pioMem32=(uint32_t *)(memmap+portOffsetData[port]);
	*pioMem32 |=mask;
}
