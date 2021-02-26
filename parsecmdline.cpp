#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include "parsecmdline.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
//#include <gl\glu.h>			// Header File For The GLu32 Library
//#include <gl\glaux.h>		// Header File For The Glaux Library
#include "SDL\SDL.h"

//#undef AUTO
GLboolean AUTO=GL_FALSE;
GLboolean PROGRAM=GL_FALSE;
SDL_bool FULLSCREEN=SDL_TRUE;
GLboolean DEBUG=GL_FALSE;
//extern BOOL BLANK;
BOOL FILENAME;
SDL_bool HELP=SDL_FALSE;

char FileName[256];
int NumObjects=-1;//32
int NumTrials=100;
SDL_bool TOS;
GLdouble speed=200.0f;
GLboolean SETSPEED=false;
SDL_bool BASELINE=SDL_TRUE;

SDL_bool SessionNameSet=SDL_FALSE;
char SessionName[255];
int jog;
long timeonscreen;
//---------------------------------------------------------------------
void SAHELP(void)
{
//	MessageBox(NULL, "sd.exe [/N num] [/B] [/F output filename]\n/A\tRun in auto mode\n/P\tProgram new task\n/D\tPerform drift correction before running the experiment\n/J\tEnter a jog factor (number of possible pixels each item could be off center depending\n\t on random calculations)\n/N\tNumber of trials. default=100\n/O\tNumber of objects (8,16,32, R(Random)) default=R\n/T\tTime to display image on screen in msec default=1000, 0=inf\n/B\tCalculate subjects optimal difficulty level - this value is calculated and modified every ~20 questions.\n/F\tSupply output filename - otherwise there is a prompt\n", "Spatial Distortion", MB_OK | MB_SYSTEMMODAL | MB_TOPMOST);
	MessageBox(NULL, "blank.exe [/T num] [/w]\n/T\tNumber of milisecounds. default=1800\nw\tRun in a window\n", 
		"Blank screen", MB_OK | MB_SYSTEMMODAL | MB_TOPMOST);
	exit(0);
}
//---------------------------------------------------------------------
BOOL ParseCmdLine(LPSTR lpCmdLine)
{
	char tmp[256];
	BOOL F = FALSE;
	BOOL JOG=FALSE;
	BOOL NO=FALSE;//number objects
	GLboolean NT=FALSE;//number trials
	TOS=SDL_FALSE;
//	AUTO=FALSE;
//	BLANK=FALSE;
//  BASELINE=FALSE;
//	HELP=FALSE;
	timeonscreen = 1000;
	jog=0;
	lpCmdLine = GetCommandLine(); //winapi function?
	if (*lpCmdLine == 0)
	{
		return FALSE;
	}
	// Get past .exe name     
	while (*lpCmdLine != ' '&& *lpCmdLine != 0)
		lpCmdLine++;
	// Find the parameters
int counter=0;
	while (*lpCmdLine != 0)
	{
	// Eat white space
		if (*lpCmdLine == ' ')
		{
			lpCmdLine++;
			continue;
		} 
		else if (*lpCmdLine == '-' || *lpCmdLine == '/')
		{
			lpCmdLine++;
			switch (*lpCmdLine)
			{
			case 'a':
//#define AUTO 1
				AUTO=TRUE;
				lpCmdLine++;
			break;
			case 'A':
//#define AUTO 1
				AUTO=TRUE;
				lpCmdLine++;
			break;
			case 'b':
//#define AUTO 1
				BASELINE=SDL_TRUE;
				lpCmdLine++;
			break;
			case 'B':
//#define AUTO 1
				BASELINE=SDL_TRUE;
				lpCmdLine++;
			break;
			case 'd':
				DEBUG=GL_TRUE;
				lpCmdLine++;
			break;
			case 'D':
				DEBUG=GL_TRUE;
				lpCmdLine++;
			break;
//help
			case 'h':
//				SAHELP();
				HELP=SDL_TRUE;
				lpCmdLine++;
			break;
			case 'H':
//				SAHELP();
				HELP=SDL_TRUE;
				lpCmdLine++;
			break;
			case '?':
				HELP=SDL_TRUE;
				lpCmdLine++;
			break;
//
			case 'f':
				FILENAME=TRUE;
				F=TRUE;
				lpCmdLine++;
			break;
			case 'F':
				F=TRUE;
				FILENAME=TRUE;
				lpCmdLine++;
			break;
			case 'p':
				PROGRAM=GL_TRUE;
				lpCmdLine++;
			break;
			case 'C':
				PROGRAM=GL_TRUE;
				lpCmdLine++;
			break;
			case 's':
//				SETSPEED=GL_TRUE;				
				SessionNameSet=SDL_TRUE;
				lpCmdLine++;
			break;
			case 'S':
//				SETSPEED=GL_TRUE;
				SessionNameSet=SDL_TRUE;
				lpCmdLine++;
			break;
			case 'n':
				NT=TRUE;
				lpCmdLine++;
			break;
			case 'N':
				NT=TRUE;
				lpCmdLine++;
			case 'o':
				NO=TRUE;
				lpCmdLine++;
			break;
			case 'O':
				NO=TRUE;
				lpCmdLine++;
			case 't':
				TOS=SDL_TRUE;
				lpCmdLine++;
			break;
			case 'T':
				TOS=SDL_TRUE;
				lpCmdLine++;
			case 'w':
				FULLSCREEN=SDL_FALSE;
				lpCmdLine++;
			break;
			case 'W':
				FULLSCREEN=SDL_FALSE;
				lpCmdLine++;
			break;
			default:
				lpCmdLine++;
				break;
			}
		}
		else
		{
			strcpy(tmp,"");
			while (*lpCmdLine != ' ' && *lpCmdLine != 0)
			{
				sprintf(tmp,"%s%c",tmp,*lpCmdLine);
//				strcat(tmp,*lpCmdLine,1);
				lpCmdLine++;
			}
			if (TOS)
			{
				timeonscreen=atoi(tmp);
				TOS=SDL_FALSE;
			}
			if (F)
			{
				strcpy(FileName,tmp);
				if (!FileExist(FileName)) {
					sprintf(tmp,"File not found: %s\nExiting ...", FileName);
					MessageBox(NULL, tmp, "ERROR", MB_OK | MB_SYSTEMMODAL | MB_TOPMOST);
					exit(0);
				}			
				F=FALSE;
			} else if (SessionNameSet) {
				strcpy(SessionName,tmp);
/*				if (FileExist(SessionName)) {
					sprintf(tmp,"File not found: %s\nExiting ...", FileName);
					MessageBox(NULL, tmp, "ERROR", MB_OK | MB_SYSTEMMODAL | MB_TOPMOST);
					exit(0);
				}			
*/
				SessionNameSet=SDL_FALSE;
			} else if (JOG) {
				jog=atoi(tmp);
				JOG=FALSE;
			} else if (NO){
				NumObjects = atoi(tmp); //=0 if unable to convert
		        NO=FALSE;
			} else if (NT){
				NumTrials = atoi(tmp); //=0 if unable to convert
		        NT=FALSE;
			} else if (SETSPEED){
				speed = atoi(tmp); //=0 if unable to convert
		        SETSPEED=FALSE;
			}

		}
		counter++;
	}
	if (counter <1) HELP = SDL_TRUE;
	if (NumObjects != 8 && NumObjects != 32 && NumObjects != 16)
	{
			NumObjects=-1;
	}
	if (NumTrials <= 0)
	{
			NumTrials=100;
	}

	return TRUE;
}
//---------------------------------------------------------------------
BOOL FileExist(TCHAR* pszFileName)
{
  BOOL bExist = FALSE;
  HANDLE hFile;

  if (NULL != pszFileName)
  {
    // Use the preferred Win32 API call and not the older OpenFile.
    hFile = CreateFile(
		pszFileName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        0);

    if (hFile != INVALID_HANDLE_VALUE)
    {
      // If the handle is valid then the file exists.
      CloseHandle(hFile);
      bExist = TRUE;
    }
  }

  return (bExist);
}
//---------------------------------------------------------------------
