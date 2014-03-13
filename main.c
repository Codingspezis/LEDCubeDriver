#include <stdio.h>
#include <pthread.h>

#include "illuminator.h"
#include "communicator.h"

// #define DELAY 1000/125*1000
#define DELAY 1000 * 2

static void *communication_thread(void *arg) {
	(void)arg;
	if(!initServer(37373)) {
		printf("starting listening loop...\n");
		listen4Clients();
		return 0;
	}
	printf("ERROR: server initialization failed\n");
	return 0;
}

int main(int argc, char **argv) {
	if(initSpi()!=-1) {
		pthread_t thread_ptr;
		pthread_create(&thread_ptr, NULL, &communication_thread, NULL);
		printf("starting illumination loop...\n");
		illuminationLoop(DELAY);
	} else {
		printf("ERROR: couldn't initialize SPI device\n");
	}
	return 0;
}
