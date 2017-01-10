#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "fast-gpio.h"

#define COLOUR_RED			"\x1b[31m"
#define COLOUR_GREEN		"\x1b[32m"
#define COLOUR_YELLOW		"\x1b[33m"
#define COLOUR_BLUE			"\x1b[34m"
#define COLOUR_MAGENTA	"\x1b[35m"
#define COLOUR_CYAN			"\x1b[36m"
#define COLOUR_CLEAR		"\x1b[0m"

void printPair(int port, int pin, char *strSystem, int port2, int pin2, char *strSystem2)
{
	char *strPrintout;
	int i=0, spacing=9;
	//Print mux
	if(pin==255) strPrintout="SYSTEM";
	else strPrintout=muxToString(port, pin, readMux(port, pin));
	for(i=0; i<=(spacing-strlen(strPrintout));i++) printf(" ");
	if(pin!=255) switch(readMux(port, pin)){
		case 0:
			printf(COLOUR_GREEN);
			break;
		case 1:
			printf(COLOUR_BLUE);
			break;
		default:
			printf(COLOUR_MAGENTA);
			break;
	}
	printf("%s" COLOUR_CLEAR " │ ", strPrintout);
	//Print pull
	if(pin==255) printf(" N/A │ ");
	else switch(readPull(port, pin)){
		case 0:
			printf(" NO  │ ");
			break;
		case 1:
			printf(COLOUR_GREEN " UP  " COLOUR_CLEAR "│ ");
			break;
		case 2:
			printf(COLOUR_GREEN "DOWN " COLOUR_CLEAR "│ ");
			break;
		default:
			printf("UKWN │ ");
			break;
	}
	if(pin==255){
		for(i=0; i<=(spacing-strlen(strSystem));i++) printf("-");
		printf(" %s │**│ ", strSystem);
	}
	else printf(COLOUR_CYAN "P%c%02d" COLOUR_CLEAR " ------ │**│ ", port+'A', pin);
	//Right side
	if(pin2==255){
		printf("%s ", strSystem2);
		for(i=0; i<=(spacing-strlen(strSystem2));i++) printf("-");
	}
	else printf("------ " COLOUR_CYAN "P%c%02d" COLOUR_CLEAR, port2+'A', pin2);
	if(pin2==255) printf(" | N/A ");
	else switch(readPull(port2, pin2)){
		case 0:
			printf(" │  NO ");
			break;
		case 1:
			printf(" │ " COLOUR_GREEN " UP " COLOUR_CLEAR);
			break;
		case 2:
			printf(" │ " COLOUR_GREEN "DOWN" COLOUR_CLEAR);
			break;
		default:
			printf(" │ UKWN");
			break;
	}
	if(pin2==255) printf(" │ SYSTEM");
	else {
		printf(" │ ");
		switch(readMux(port2, pin2)){
			case 0:
				printf(COLOUR_GREEN);
				break;
			case 1:
				printf(COLOUR_BLUE);
				break;
			default:
				printf(COLOUR_MAGENTA);
				break;
		}
		printf("%s" COLOUR_CLEAR, muxToString(port2, pin2, readMux(port2, pin2)));
	}

	printf("\n");
}

int main(int argc, char* argv[])
{
	uint8_t port, pin, value;

	if(argc <= 1 || strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0){
		printf("Chippy Gonzales is a tool for directly manipulating the\n");
		printf("GPIO hardware-registers, including pull-ups/-downs, muxes\n");
		printf("and pin-states. This tool is only intended for those who\n");
		printf("know what they are doing and you can very much mess things\n");
		printf("with this.\n\n");
		printf("YOU ASSUME ALL RESPONSIBILITY FOR USE OF THIS TOOL.\n\n");
		printf("The tool accepts the following commands: readPin, readPull, readMux,\n");
		printf("writePin, writePull, writeMux, header1 and header2. All the\n");
		printf("read-commands follow the same format, ie. `gonzales command port pin`\n");
		printf("where port is a number ranging from 0 to 6 or a character from a to g,\n");
		printf("and pin is 0 to 31. Port 0 corresponds to port A, port 1 to port B\n");
		printf("and so on, and if given as a character, the character can be lowercase\n");
		printf("or uppercase. Example: `gonzales readPin d 25` to read pin PD25.\n\n");
		printf("The format for the write-commands is the same, except for the value\n");
		printf("given:\n");
		printf("`writePin [port] [pin] [low/LOW/high/HIGH/0/1]`\n");
		printf("`writeMux [port] [pin] [in/input/INPUT/out/output/OUTPUT/0-6]`\n");
		printf("  -- 0 means INPUT, 1 means OUTPUT and the rest depends on the pin,\n");
		printf("  and the command takes no human-readable input for those.\n");
		printf("`writePull [port] [pin] [up/UP/down/DOWN/disable/DISABLE/0/1/2]`\n");
		printf("  -- 0 means disable, 1 means pull-up, 2 means pull-down.\n\n");
		printf("The commands `header1` and `header2` do not take any parameters.\n");
		return 0;
	}

	if(initGpio()) return 1;

	if(argc < 4){
		if(argc > 1 && strcmp(argv[1], "header1")==0){
			printf("      Mux  │ Pull │  Function   J13    Function  │ Pull │  Mux\n");
			printf("      ─────┼──────┼──────────────────────────────┼──────┼─────\n");
			printPair(0, 255, "GND", 0, 255, "CHG-IN");
			printPair(0, 255, "5V Out", 0, 255, "GND");
			printPair(0, 255, "3.3V Out", 0, 255, "Tempsense");
			printPair(0, 255, "1.8V Out", 0, 255, "BAT+");
			printPair(1, 16, NULL, 0, 255, "PWR-BTN");
			printPair(1, 15, NULL, 0, 255, "GND");
			printPair(0, 255, "Touch-X1", 0, 255, "Touch-X2");
			printPair(0, 255, "Touch-Y1", 0, 255, "Touch-Y2");
			printPair(3, 2, NULL, 1, 2, NULL);
			printPair(3, 4, NULL, 3, 3, NULL);
			printPair(3, 6, NULL, 3, 5, NULL);
			printPair(3, 10, NULL, 3, 7, NULL);
			printPair(3, 12, NULL, 3, 11, NULL);
			printPair(3, 14, NULL, 3, 13, NULL);
			printPair(3, 18, NULL, 3, 15, NULL);
			printPair(3, 20, NULL, 3, 19, NULL);
			printPair(3, 22, NULL, 3, 21, NULL);
			printPair(3, 24, NULL, 3, 23, NULL);
			printPair(3, 27, NULL, 3, 26, NULL);
			printPair(0, 255, "GND", 3, 25, NULL);
			printf("      ─────┼──────┼──────────────────────────────┼──────┼─────\n");
			printf("      Mux  │ Pull │  Function   J13    Function  │ Pull │  Mux\n");
			return 0;
		} else
		if(argc > 1 && strcmp(argv[1], "header2")==0){
			printf("      Mux  │ Pull │  Function   J14    Function  │ Pull │  Mux\n");
			printf("      ─────┼──────┼──────────────────────────────┼──────┼─────\n");
			printPair(0, 255, "GND", 0, 255, "5V-Out");
			printPair(6, 3, NULL, 0, 255, "Hphone-L");
			printPair(6, 4, NULL, 0, 255, "Hp-common");
			printPair(0, 255, "FEL", 0, 255, "Hphone-R");
			printPair(0, 255, "3.3V Out", 0, 255, "Mic-GND");
			printPair(0, 255, "LRADC", 0, 255, "Mic-in");
			printPair(0, 255, "XIO-P0", 0, 255, "XIO-P1");
			printPair(0, 255, "XIO-P2", 0, 255, "XIO-P3");
			printPair(0, 255, "XIO-P4", 0, 255, "XIO-P5");
			printPair(0, 255, "XIO-P6", 0, 255, "XIO-P7");
			printPair(0, 255, "GND", 0, 255, "GND");
			printPair(6, 1, NULL, 1, 3, NULL);
			printPair(1, 18, NULL, 1, 17, NULL);
			printPair(4, 0, NULL, 4, 1, NULL);
			printPair(4, 2, NULL, 4, 3, NULL);
			printPair(4, 4, NULL, 4, 5, NULL);
			printPair(4, 6, NULL, 4, 7, NULL);
			printPair(4, 8, NULL, 4, 9, NULL);
			printPair(4, 10, NULL, 4, 11, NULL);
			printPair(0, 255, "GND", 0, 255, "GND");
			printf("      ─────┼──────┼──────────────────────────────┼──────┼─────\n");
			printf("      Mux  │ Pull │  Function   J14    Function  │ Pull │  Mux\n");
			return 0;
		}
		printf("Too few parameters!\n");
		return 1;
	}

	port = 255;
	if(*argv[2] >= 'a' && *argv[2] <= 'g') port = *argv[2] - 'a';
	if(*argv[2] >= 'A' && *argv[2] <= 'G') port = *argv[2] - 'A';
	if(*argv[2] >= '0' && *argv[2] <= '6') port = *argv[2] - '0';

	pin = atoi(argv[3]);
	if(port > 6){
		printf("Port must be in the range of 0-6, or a-g!\n");
		return 1;
	}
	if(pin > 31){
		printf("Pin must be in the range of 0 to 31!\n");
		return 1;
	}

	if(strcmp(argv[1], "readPin")==0){
		printf("Port %c, pin %d: ", port+'A', pin);
		if(readPin(port, pin)) printf("HIGH\n");
		else printf("LOW\n");
	}
	else if(strcmp(argv[1], "readMux")==0){
		printf("Port %c, pin %d: ", port+'A', pin);
		value=readMux(port, pin);
		printf("%s\n", muxToString(port, pin, value));
	}
	else if(strcmp(argv[1], "readPull")==0){
		printf("Port %c, pin %d: ", port+'A', pin);
		switch(readPull(port, pin)){
			case 0:
				printf("pull-up/-down disabled.\n");
				break;
			case 1:
				printf("pull-up enabled.\n");
				break;
			case 2:
				printf("pull-down enabled.\n");
				break;
			default:
				printf("unknown status.\n");
				break;
		}
	}
	else if(strcmp(argv[1], "writePull")==0){
		if(argc < 5){
			printf("Too few parameters!\n");
			return 1;
		}
		value = atoi(argv[4]);
		if(strcmp(argv[4], "up")==0 || strcmp(argv[4], "UP")==0) value=1;
		else if(strcmp(argv[4], "down")==0 || strcmp(argv[4], "DOWN")==0) value=2;
		else if(strcmp(argv[4], "disable")==0 || strcmp(argv[4], "disabled")==0
			|| strcmp(argv[4], "DISABLE")==0 || strcmp(argv[4], "DISABLED")==0) value=0;
		printf("New setting for port %c, pin %d: ", port+'A', pin);
		switch(value){
			case 0:
				printf("pull-up/-down disabled.\n");
				break;
			case 1:
				printf("pull-up enabled.\n");
				break;
			case 2:
				printf("pull-down enabled.\n");
				break;
			default:
				printf("unknown status.\n");
				break;
		}
		writePull(port, pin, value);
	}
	else if(strcmp(argv[1], "writePin")==0){
		if(argc < 5){
			printf("Too few parameters!\n");
			return 1;
		}
		value = atoi(argv[4]);
		if(strcmp(argv[4], "high")==0 || strcmp(argv[4], "HIGH")==0) value=1;
		else if(strcmp(argv[4], "low")==0 || strcmp(argv[4], "LOW")==0) value=2;
		printf("New setting for port %c, pin %d: ", port+'A', pin);
		switch(value){
			case 0:
				printf("LOW.\n");
				break;
			case 1:
				printf("HIGH.\n");
				break;
			default:
				printf("unknown value given.\n");
				return 1;
				break;
		}
		writePin(port, pin, value);
	}
	else if(strcmp(argv[1], "writeMux")==0){
		if(argc < 5){
			printf("Too few parameters!\n");
			return 1;
		}
		value = atoi(argv[4]);
		if(strcmp(argv[4], "input")==0 || strcmp(argv[4], "INPUT")==0
			|| strcmp(argv[4], "in")==0 || strcmp(argv[4], "IN")==0) value=0;
		else if(strcmp(argv[4], "output")==0 || strcmp(argv[4], "OUTPUT")==0
				|| strcmp(argv[4], "out")==0 || strcmp(argv[4], "OUT")==0) value=1;
		printf("New setting for port %c, pin %d: ", port+'A', pin);
		switch(value){
			case 0:
				printf("INPUT.\n");
				break;
			case 1:
				printf("OUTPUT.\n");
				break;
			default:
				printf("altmux 0x%x.\n", value);
				break;
		}
		writeMux(port, pin, value);
	}
	return 0;
}
