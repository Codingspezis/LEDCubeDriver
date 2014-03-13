#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <wiringPiSPI.h>

#include "illuminator.h"

// expander opcode values
#define EOV_START    0x40
#define EOV_IC1_ADDR 0x00
#define EOV_IC2_ADDR 0x02

// expander register addresses for BANK=0
#define ERA_IOCON  0x0A
#define ERA_IODIRA 0x00
#define EPA_GPIOA  0x12

uint16_t led2ic2[] = {
	0x0001, // a0
	0x0002, // a1
	0x0004, // a2
	0x0008, // a3
	0x0010, // a4
	0x0020, // a5
	0x0040, // a6
	0x0080, // a7
	0x0100, // b0
	0x0200, // b1
	0x0400, // b2
	0x0800, // b3
	0x1000, // b4
	0x2000, // b5
	0x4000, // b6
	0x8000  // b7
};

uint16_t led2ic1[] = {
	0x0080, // a7
	0x0100, // b0
	0x0200, // b1
	0x0400, // b2
	0x0800, // b3
	0x1000, // b4
	0x2000, // b5
	0x4000, // b6
	0x8000  // b7
};

uint16_t level2ic1[] = {
	0x0001, // a0
	0x0002, // a1
	0x0004, // a2
	0x0008, // a3
	0x0010  // a4
};

uint8_t spiBuffer[4];

uint8_t *cubeState;

int spiFD;
int initSpi() {
	int i;
	cubeState = (uint8_t *)malloc(16);
	for(i=0; i<16; i++) {
		cubeState[i] = 0x00;
	}
	spiFD = wiringPiSPISetup(0, 8000000);
	return spiFD;
}

int spiWrite(uint8_t *data, int length) {
	if(length != write(spiFD, data, length)) {
		printf("ERROR: SPI communication failed\n");
		return -1;
	}
	return length;
	/*
	if(length != fwrite(data, 1, length , spiDev)) {
		printf("ERROR: SPI communication failed\n");
		return -1;
	}
	return length;
	*/
}

int configureExpanders() {
	// write the IOCON register separate for the two ICs
	spiBuffer[0] = EOV_START | EOV_IC1_ADDR;
	spiBuffer[1] = ERA_IOCON;
	spiBuffer[2] = 0x28; // BANK=0; MIRROR=0; SEQOP=1; DISSLW=0; HAEN=1; ODR=0; INTPOL=0; UNUSED=0;
	if(spiWrite(spiBuffer, 3) != 3) return -1;
	spiBuffer[0] = EOV_START | EOV_IC2_ADDR;
	if(spiWrite(spiBuffer, 3) != 3) return -1;
	// write the IODIR registers sequential
	spiBuffer[0] = EOV_START | EOV_IC1_ADDR;
	spiBuffer[1] = ERA_IODIRA;
	spiBuffer[2] = 0x00;
	spiBuffer[3] = 0x00;
	if(spiWrite(spiBuffer, 4) != 4) return -1;
	spiBuffer[0] = EOV_START | EOV_IC2_ADDR;
	if(spiWrite(spiBuffer, 4) != 4) return -1;
	return 0;
}

inline void illuminateLevel(int l) {
	int i;
	spiBuffer[1] = EPA_GPIOA;
	// IC2
	spiBuffer[0] = EOV_START | EOV_IC2_ADDR;
	*(uint16_t *)(spiBuffer+2) = 0;
	for(i=0; i<16; i++) {
		int led = 25*l + i;
		if(cubeState[led/8] & (1 << (led%8))) {
			*(uint16_t *)(spiBuffer+2) |= led2ic2[i];
		}
	}
	spiWrite(spiBuffer, 4);
	// IC1
	spiBuffer[0] = EOV_START | EOV_IC1_ADDR;
	*(uint16_t *)(spiBuffer+2) = level2ic1[l];
	for(i=0; i<9; i++) {
		int led = 25*l + 16 + i;
		if(cubeState[led/8] & (1 << (led%8))) {
			*(uint16_t *)(spiBuffer+2) |= led2ic1[i];
		}
	}
	spiWrite(spiBuffer, 4);
}

void illuminationLoop(int delayUs) {
	if(!configureExpanders()) {
		int i;
		for(;;) {
			for(i=0; i<5; i++) {
				illuminateLevel(i);
				usleep(delayUs);
			}
		}
	}
	printf("ERROR: configuration of expanders failed\n");
}
