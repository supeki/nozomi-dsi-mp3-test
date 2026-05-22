#include "sound.h"

char **pathes;
char cwd[1024];
int numpathes = 0;

void wait(int frames)
{
	for (int i = 0; i < frames; i++)
		swiWaitForVBlank();
}

void openDirOrFile(const char *dirfile) {
	int p = 0;
	struct dirent *de;
	struct stat path_stat;
	
	getcwd(cwd, sizeof(cwd));
	
	if (stat(dirfile, &path_stat) != 0) {
		printf("Invalid File/Directory:\n%s\n", dirfile);
		wait(60);
		return;
    }
	
	if (S_ISDIR(path_stat.st_mode))
	{
		if (strcmp(dirfile, "..") > 0) {
			int nplen = sizeof(cwd) + strlen(dirfile)*sizeof(char);
			char *newpath = malloc(nplen);
			snprintf(newpath, nplen, "%s%s", cwd, dirfile);		
			chdir(newpath);
			free(newpath);
		} else
			chdir("..");
		
		getcwd(cwd, sizeof(cwd));
		DIR *dr = opendir(cwd);
		
		if (pathes != NULL) {
			for (int i = numpathes-1; i > -1; i--)
				free(pathes[i]);
			
			free(pathes);
		}
		
		while ((de = readdir(dr)) != NULL)
			p++;
		
		numpathes = p;

		pathes = (char **)malloc(numpathes * sizeof(char *));
		closedir(dr); 
		
		// ok now do it again
		dr = opendir(cwd);
		
		p = 0;
		
		while ((de = readdir(dr)) != NULL) {
			pathes[p] = malloc(strlen(de->d_name)+1);
            strcpy(pathes[p], de->d_name); // copy the names tho
			pathes[p][strlen(de->d_name)] = '\0';
			p++;
		}
		
		closedir(dr);
		return;
	}
	else if (playSound(dirfile, false)) // it's not a directory, try mp3 playback
		return;
	else
		stopSound();
		
	printf("Invalid File/Directory:\n%s!\n", dirfile);
	wait(60);
}

int choice = 0;
void handleMenu(void) {
	uint16_t keys;
	
	scanKeys();
	keys = keysDown();
	
	if (keys & KEY_UP)
		choice--;
	if (keys & KEY_DOWN)
		choice++;
	
	if (choice > numpathes-1)
		choice = 0;
	if (choice < 0)
		choice = numpathes-1;

	if (keys & KEY_A && numpathes > 0)
		openDirOrFile(pathes[choice]);
	if (keys & KEY_B)
		openDirOrFile("..");
}

void handlePlayer(void) {
	uint16_t keys;
	
	scanKeys();
	keys = keysDown();
	
	if (keys & KEY_A)
		pauseSound();
	if (keys & KEY_B)
		stopSound();
}

PrintConsole botscr, topscr;

int main(int argv, const char *argc[])
{
	videoSetMode(MODE_0_3D);
	videoSetModeSub(MODE_0_2D);
	consoleInit(&botscr, 0, BgType_Text4bpp, BgSize_T_256x256, 7, 0, false, true);
	consoleInit(&topscr, 1, BgType_Text4bpp, BgSize_T_256x256, 7, 0, true, true);
	defaultExceptionHandler();
	
	bgSetPriority(0, 1);
	bgSetPriority(1, 0);
	glScreen2D();

	if (!fatInitDefault())
    {
		printf("Failed to initialize FAT!\n");
        while (1) {}
    }
	
	openDirOrFile(".");
    initSound();

    while (1) {
		consoleSelect(&botscr);
		consoleClear();
		
		if (sound_playing)
			handlePlayer();
		else {
			for (int p = 0; p < numpathes; p++)
				if (p == choice)
					printf("> %s\n", pathes[p]);
				else
					printf("  %s\n", pathes[p]);
				
			consoleSetCursor(&topscr, 0, 23);
			printf("A: Select Option");
		
			consoleSelect(&topscr);
			consoleClear();
			handleMenu();
		}
		
		consoleSelect(&topscr);
		consoleClear();
		
		if (sound_playing) {
			// get status
			float pos = soundPosition(), len = soundLength();
			int w = (int)(pos / len * 256.0f);
			
			// show status, etc
			printf("\nListening to:\n%s\n\n", pathes[choice]);
			printf(
				"sample rate: %d\n"\
				"format: %s\n",
				soundRate(),
				soundFormat()
			);
			
			consoleSetCursor(&topscr, 0, 22);
			printf("A: Play/Pause");
			consoleSetCursor(&topscr, 0, 23);
			printf("B: Return to Menu");
			
			swiWaitForVBlank();
			
			glBegin2D();
			
			glBoxFilled(0, 0, w, 4, RGB15(255, 255, 255));
			
			glEnd2D();
			glFlush(0);
		} else {
			swiWaitForVBlank();
			glBegin2D();
			glEnd2D();
			glFlush(0);
		}
	}
	
    return 0;
}