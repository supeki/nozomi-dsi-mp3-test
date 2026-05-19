#ifndef SOUND_H
#define SOUND_H

#include "defines.h"

bool initSound(void);
bool playSound(const char *filename, bool loop);
void freeSound(void);

mm_word streamCallback(mm_word length, mm_addr dest, mm_stream_formats format);

#endif // SOUND_H