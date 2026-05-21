#define DR_MP3_IMPLEMENTATION

#include "sound.h"
#include "dr_mp3.h"

drmp3dec dec;
drmp3 mp3;
mm_stream stream;

bool sound_playing = false;
static bool sound_loop = false;

bool initSound(void)
{
	mmInitNoSoundbank();
	soundEnable();
	drmp3dec_init(&dec);

	return true;
}

bool playSound(const char *filename, bool loop)
{
	if (sound_playing)
		stopSound();
	
	if (!drmp3_init_file(&mp3, filename, NULL)) {
		printf("Failed to load %s!\n", filename);
		wait(60);
		return false;
	}

	stream.sampling_rate = mp3.sampleRate;
    stream.buffer_length = (DRMP3_MAX_PCM_FRAMES_PER_MP3_FRAME*mp3.channels);
    stream.callback = streamCallback;
    stream.format = (mp3.channels > 1) ? MM_STREAM_16BIT_STEREO : MM_STREAM_16BIT_MONO;
    stream.timer = MM_TIMER3;
    stream.manual = false;
	mmStreamOpen(&stream);
	
	sound_playing = true;
	sound_loop = loop;
	return true;
}

void stopSound(void)
{
	freeSound(&mp3);
	sound_playing = false;
	mmStreamClose();
}

bool sound_paused = false;

void pauseSound(void)
{
	sound_paused = (!sound_paused);
	
	// hack since idk how to stream null audio
	if (sound_paused)
		mmStreamVolume(0);
	else
		mmStreamVolume(127);
}

void freeSound(void *ptr)
{
	drmp3_free(ptr, NULL);
}

int soundPosition()
{
	return mp3.currentPCMFrame;
}

int soundLength()
{
	return mp3.totalPCMFrameCount;
}

int soundRate()
{
	return mp3.sampleRate;
}

char *soundFormat()
{
	return (stream.format == MM_STREAM_16BIT_STEREO) ? "stereo" : "mono";
}

mm_word streamCallback(mm_word length, mm_addr dest, mm_stream_formats format)
{	
	if (!sound_playing || sound_paused) {
		memset(dest, 0, length);
		return length;
	}
	
	mm_word got = drmp3_read_pcm_frames_s16(&mp3, length, dest);
	
	// We've hit the end of the PCM data!
	if (got == 0) {
		if (sound_loop) {
			drmp3_seek_to_pcm_frame(&mp3, 0);
			got = drmp3_read_pcm_frames_s16(&mp3, length, dest);
		} else {
			sound_playing = false;
			freeSound(&mp3);
			memset(dest, 0, length);
			return length;
		}
	}
	
    return length;
}