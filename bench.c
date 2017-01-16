#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#ifdef _WITH_LIBSOC
#include <libsoc_gpio.h>
#endif

#include "fast-gpio.h"

int main(int argc, char* argv[])
{
	struct timespec timeStart, timeEnd;
	uint32_t counter;
	double accum;

	int error=initGpio();
	if(error==1){
		printf("Error opening /dev/mem!\n");
		return 1;
	} else if(error==2){
		printf("Error mmapping memory!\n");
		return 2;
	}

	int origMux, origValue;
	origMux = readMux(3, 25);
	origValue = readPin(3, 25);
	writeMux(3, 25, _GPIO_MUX_OUT);

#ifdef _WITH_LIBSOC
	gpio *testPin;
	int libsocOrigMux;
	testPin = libsoc_gpio_request(121, LS_SHARED);
	if(testPin == NULL)
	{
			printf("Error setting up libsoc!\n");
			return 1;
	}
	libsocOrigMux = libsoc_gpio_get_direction(testPin);
	libsoc_gpio_set_direction(testPin, OUTPUT);
	printf("Test 10 million pin-togglings using libsoc on PD25...\n");
	clock_gettime(CLOCK_MONOTONIC, &timeStart);
	for(counter=0; counter<5000000; counter++){
		libsoc_gpio_set_level(testPin, LOW);
		libsoc_gpio_set_level(testPin, HIGH);
	}
	clock_gettime(CLOCK_MONOTONIC, &timeEnd);
	accum = ( timeEnd.tv_sec - timeStart.tv_sec )
		+ ( timeEnd.tv_nsec - timeStart.tv_nsec )
		/ 1E9;
	printf( "That took %.02lf seconds at ~%.02lfMbps\n", accum,  1E7 / accum / 1E6);
#endif

	printf("Test 10 million pin-togglings using writePin on PD25...\n");
	clock_gettime(CLOCK_MONOTONIC, &timeStart);
	for(counter=0; counter<5000000; counter++){
		writePin(3, 25, _GPIO_LOW);
		writePin(3, 25, _GPIO_HIGH);
	}
	clock_gettime(CLOCK_MONOTONIC, &timeEnd);
	accum = ( timeEnd.tv_sec - timeStart.tv_sec )
		+ ( timeEnd.tv_nsec - timeStart.tv_nsec )
		/ 1E9;
	printf( "That took %.02lf seconds at ~%.02lfMbps\n", accum,  1E7 / accum / 1E6);

	printf("Test 10 million pin-togglings using clearPort and setPort on PD25...\n");
	uint32_t mask = 1 << 25;
	clock_gettime(CLOCK_MONOTONIC, &timeStart);
	for(counter=0; counter<5000000; counter++){
		clearPort(3, mask); //Toggle pin LOW
		setPort(3, mask); //Toggle pin HIGH
	}
	clock_gettime(CLOCK_MONOTONIC, &timeEnd);
	accum = ( timeEnd.tv_sec - timeStart.tv_sec )
		+ ( timeEnd.tv_nsec - timeStart.tv_nsec )
		/ 1E9;
	printf( "That took %.02lf seconds at ~%.02lfMbps\n", accum,  1E7 / accum / 1E6);

#ifdef _WITH_LIBSOC
	libsoc_gpio_set_direction(testPin, libsocOrigMux);
	libsoc_gpio_free(testPin);
#endif

	writeMux(3, 25, origMux);
	writePin(3, 25, origValue);
}
