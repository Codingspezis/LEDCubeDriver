#ifndef ILLUMINATOR_H
#define ILLUMINATOR_H

#include <stdint.h>

int initSpi();
void illuminationLoop(int delayUs);

extern uint8_t *cubeState;

#endif // ILLUMINATOR_H
