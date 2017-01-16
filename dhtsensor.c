#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "fast-gpio.h"

typedef struct structDhtSensor {
	float humidity, temperature;
} structDhtSensor;

void sleepMillis(uint32_t millis) {
  struct timespec sleep;
  sleep.tv_sec = millis / 1000;
  sleep.tv_nsec = (millis % 1000) * 1000000L;
  while(clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, &sleep) && errno == EINTR);
}

structDhtSensor* dhtRead(int port, int pin, int type)
{
	int origMux, origValue, origDebounce;
	structDhtSensor dhtSensor;
	dhtSensor.humidity = -255.0f;
	dhtSensor.temperature = -255.0f;
	int pulseCounts[82] = { 0 };
	struct sched_param sched;

	origMux = readMux(port, pin);
	origValue = readPin(port, pin);
	origDebounce = readDebounce();

	memset(&sched, 0, sizeof(sched));
	sched.sched_priority = sched_get_priority_max(SCHED_FIFO);
	sched_setscheduler(0, SCHED_FIFO, &sched);

	writeMux(port, pin, _GPIO_MUX_OUT);
	writePin(port, pin, _GPIO_LOW);
	sleepMillis(500);
	writePin(port, pin, _GPIO_HIGH);
	sleepMillis(500);
	writePin(port, pin, _GPIO_LOW);
	sleepMillis(20);
	writeMux(port, pin, _GPIO_MUX_IN);
	{
    volatile int i = 0;
    for (i; i < 50; i++)
    {
    }
  }
	uint32_t count = 0;
	do {
		if(count++ >= 32400)
		{
			memset(&sched, 0, sizeof(sched));
		  sched.sched_priority = 0;
		  sched_setscheduler(0, SCHED_OTHER, &sched);
			writeMux(port, pin, origMux);
			writePin(port, pin, origValue);
			writeDebounce(origDebounce);
			return NULL;
		}
	} while(readPin(port, pin));

	int i;
	for(i=0; i<82; i+=2)
	{
		do
		{
			if (pulseCounts[i]++ >= 32400)
			{
				memset(&sched, 0, sizeof(sched));
			  sched.sched_priority = 0;
			  sched_setscheduler(0, SCHED_OTHER, &sched);
				writeMux(port, pin, origMux);
				writePin(port, pin, origValue);
				writeDebounce(origDebounce);
				return NULL;
			}
		} while(!readPin(port, pin));

		do
		{
			if (pulseCounts[i+1]++ >= 32400)
			{
				memset(&sched, 0, sizeof(sched));
			  sched.sched_priority = 0;
			  sched_setscheduler(0, SCHED_OTHER, &sched);
				writeMux(port, pin, origMux);
				writePin(port, pin, origValue);
				writeDebounce(origDebounce);
				return NULL;
			}
		} while(readPin(port, pin));
	}

	memset(&sched, 0, sizeof(sched));
	sched.sched_priority = 0;
	sched_setscheduler(0, SCHED_OTHER, &sched);

	uint32_t threshold = 0;
	for (i=2; i < 82; i+=2) threshold += pulseCounts[i];
	threshold /= 40;

	uint8_t data[5] = {0};
	for (i=3; i < 82; i+=2) {
		int index = (i-3)/16;
		data[index] <<= 1;
		if (pulseCounts[i] >= threshold) data[index] |= 1;
	}

	if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
		if (type == 0) {
			dhtSensor.humidity = (float)data[0];
			dhtSensor.temperature = (float)data[2];
		} else if (type == 1) {
			dhtSensor.humidity = (data[0] * 256 + data[1]) / 10.0f;
			dhtSensor.temperature = ((data[2] & 0x7F) * 256 + data[3]) / 10.0f;
			if (data[2] & 0x80) dhtSensor.temperature *= -1.0f;
		}
		writeMux(port, pin, origMux);
		writePin(port, pin, origValue);
		writeDebounce(origDebounce);
		structDhtSensor* returnSensor = malloc(sizeof(*returnSensor));
		if(returnSensor==NULL) return NULL;
		returnSensor->humidity = dhtSensor.humidity;
		returnSensor->temperature = dhtSensor.temperature;
		return returnSensor;
	}
	writeMux(port, pin, origMux);
	writePin(port, pin, origValue);
	writeDebounce(origDebounce);
	return NULL;
}

int main(int argc, char* argv[])
{
	uint8_t port, pin, retries;
	structDhtSensor *dhtSensor=NULL;

	if(argc <= 1 || strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0){
		printf("This is a simple application for reading temperature and\n"
			"humidity off a DHT11- or DHT22-sensor. Call it as follows:\n\n");
		printf("dhtsensor [sensortype] [port] [pin]\n");
		printf("  'sensortype' can be 'dht11' or 'dht22'.\n");
		printf("  'port' can be a number 0-6, or a letter 'a'-'g'.\n");
		printf("  'pin' can be a number 0-31.\n");
		return 0;
	}

	if(argc < 4){
		printf("Too few parameters given!\n");
		return 1;
	}

	port = atoi(argv[2]);
	if(*argv[2] >= 'a' && *argv[2] <= 'g') port = *argv[2] - 'a';
	if(*argv[2] >= 'A' && *argv[2] <= 'G') port = *argv[2] - 'A';
	if(*argv[2] >= '0' && *argv[2] <= '6') port = *argv[2] - '0';

	pin = atoi(argv[3]);
	if(port > 6){
		printf("Port must be in the range of 0-6, or a-g!\n");
		return 1;
	}
	if(pin > 31){
		printf("Pin must be in the range of 0-31!\n");
		return 1;
	}

	int error=initGpio();
	if(error==1){
		printf("Error opening /dev/mem!\n");
		return 1;
	} else if(error==2){
		printf("Error mmapping memory!\n");
		return 2;
	}

	retries=3;
	if(strcmp(argv[1], "dht11")==0 || strcmp(argv[1], "DHT11")==0){
		while(dhtSensor==NULL){
			dhtSensor=dhtRead(port, pin, 0);
			if(dhtSensor){
				printf("DHT11 temperature: %.01fC, humidity: %.01f%%.\n",
					dhtSensor->temperature, dhtSensor->humidity);
				break;
			}
			retries--;
			if(!retries){
				printf("Error reading from sensor!\n");
				return 1;
			}
		}
	} else if(strcmp(argv[1], "dht22")==0 || strcmp(argv[1], "DHT22")==0){
		while(dhtSensor==NULL){
			dhtSensor=dhtRead(port, pin, 1);
			if(dhtSensor){
				printf("DHT22 temperature: %.01fC, humidity: %.01f%%.\n",
					dhtSensor->temperature, dhtSensor->humidity);
				printf("Retries: %d\n", 3 - retries);
				break;
			}
			retries--;
			if(!retries){
				printf("Error reading from sensor!\n");
				return 1;
			}
		}
	} else {
		printf("Unknown argument '%s' given, should be 'dht11' or 'dht22'!\n",
			argv[1]);
		return 1;
	}
	free(dhtSensor);
	return 0;
}
