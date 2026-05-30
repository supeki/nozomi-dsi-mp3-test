#ifndef SOUND_H
#define SOUND_H

#include "defines.h"

extern bool sound_playing;
extern bool sound_paused;

bool initSound(void);
bool playSound(const char *filename, bool loop);
void stopSound(void);
void freeSound(void *ptr);
int soundPosition(void);
int soundLength(void);
int soundRate(void);
const char *soundFormat(void);
void pauseSound(void);

mm_word streamCallback(mm_word length, mm_addr dest, mm_stream_formats format);

#endif // SOUND_H