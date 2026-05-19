#include "sound.h"

int main()
{
	consoleDemoInit();

	if (!nitroFSInit(NULL))
    {
		printf("Failed to initialize nitroFS!\n");
        while (1) {}
    }

    initSound();
    playSound("nitro:/test2.mp3", true);

    while (1) {}
    return 0;
}