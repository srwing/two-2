#ifndef LOGFILE
#define LOGFILE

#ifdef __cplusplus
extern "C" {
#endif

	#include "SDL.h"
	FILE *file;
	char logfile[128];

	void initFileLog();
	void openFileLog(char* fname);
	void closeFileLog();
	SDL_bool Log (char *message);
	void LogErr (char *message);
	SDL_bool myOpenFile(SDL_bool training);

#ifdef __cplusplus
}
#endif
#endif