#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <SDL/SDL.h>
#include "SFont.h"
#include <SDL/SDL_image.h>
#include "filelog.hpp"
#include "lptport.h"
#include "loadlist.h"
#include "sdlimage.h"
#include "parsecmdline.h"
//#include "chomp.h"
//////////////////////////////////////////////////////////
#define WIDTH 800
#define HEIGHT 600
#define FONT Copper
//#define NUMLOOPS 400

Uint32 now;
SDL_bool textonscreen=SDL_FALSE;//text displayed on screen?
extern SDL_bool LogCreated=SDL_FALSE;

SDL_Surface *screen;
	SFont_Font* Neon;
	SFont_Font* Copper;
char str[128];
CFileLog filelog;
//extern char** list;
//extern int listcount;
extern LISTITEM list1;

extern char** introlist;
extern int introcount;
extern char FileName[256];
extern SDL_bool HELP;
extern SDL_bool FULLSCREEN;
extern SDL_bool BASELINE;
extern long timeonscreen;
//////////////////////////////////////////////////////////
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#elif defined WIN32 //if you want to have windows dependability
#include <windows.h>
#endif
////////////////////////////////////////////////////////////////
void sendSignal(char* str, int num);
void runBaseline(long duration);
char* removeNL(char* str);
void init_SDL()
{
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr,
			"Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);			/* Clean up on exit */

	/* Initialize the display */
/*#ifdef __APPLE__
Str255 msg;
c2pstrcpy(msg, cmsg);
StandardAlert(kAlertStopAlert, "\pError", (ConstStr255Param)msg, NULL, NULL);
#elif defined WIN32
MessageBox(NULL, cmsg, "Error", MB_ICONERROR | MB_OK);
#else
printf("ERROR: %s\n", cmsg);
#endif*/
	
//	if (MessageBox(NULL, "Run in Fullscreen 1024 x 768?", "Alert!", MB_YESNO | MB_ICONINFORMATION) == IDYES)
	if (FULLSCREEN==SDL_TRUE)
	{
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_FULLSCREEN);
	} else {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, 0);
	}
	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
					WIDTH, HEIGHT, 16, SDL_GetError());
		exit(1);
	}
	SDL_ShowCursor(0); //don't show cursor
	/* Set the window manager title bar */
	SDL_WM_SetCaption("two2", "two2");
}

int main(int argc, char *argv[])
{
	int txthpos=0, txtvpos=0, ii=0, textcounter=0, wordcount=0;
	SDL_bool quitKey = SDL_FALSE;

	int introcounter=0;
	SDL_bool intro=SDL_FALSE;
	char item[128];

	time_t timeStruct;
	time(&timeStruct);
	
	ParseCmdLine(" ");
	if (HELP==SDL_TRUE) {
		MessageBox(NULL, "Usage: blank.exe milisecounds-blank screen for msecs of time", "blank - Usage", MB_OK| MB_SYSTEMMODAL | MB_TOPMOST);
		exit(0);
	}

//	int numtextcount=100;
/* random numbers do not work ...
//not using	srand(time(NULL));
filelog.Log("\n");//init log file
sprintf(str, "");
	for (int j=0; j!=100; ++j)
	{
		textcount[j]= rand()%listcount;
		sprintf(str,"%s, %d", str, textcount[j]);
	}
sprintf(str, "%s\n", str);
filelog.Log(str);
*/


	int delaycount=0;

	init_SDL();

	if (BASELINE==SDL_TRUE) 
	{
		timingLoop(99);
		runBaseline(timeonscreen);
		timingLoop(99);
		exit (0);
	}

	//load wordlist file into memory
	wordcount=LoadList(FileName);
//	int textcount[100];
//	int numtextcount=list1.numitems;//			listcount;
/////////////////////////////////////////////////////////////

	
////////////////////////////////////////////////////////////
	// Load and Prepare the fonts - You don't have to use the IMGlib for this
	Copper = SFont_InitFont(IMG_Load("24P_Copperplate_Blue.png"));
	if(!Copper) {
		fprintf(stderr, "An error occured while loading the font.");
		exit(1);
	}
	Neon = SFont_InitFont(IMG_Load("24P_Arial_NeonYellow.png"));
	if(!Neon) {
		fprintf(stderr, "An error occured while loading the font.");
		exit(1);
	}
	
	//initialize log file and write header
	sprintf(str, "\n"); 
	sendSignal(str, 0);
//	filelog.Log(str);
	sprintf(str, "Start date and time: %s\n", ctime(&timeStruct));
	sendSignal(str, 9);
	sprintf(str, "In the EEG file: 9=start and end of a session, 3=Keypress recorded, 1=word on screen\n");
//	filelog.Log(str);
	sprintf(str, "\nElapsed Time, Program Output, Designation, Start date and time: %s\n", ctime(&timeStruct));
	sendSignal(str, 0);
//	filelog.Log(str);

//	timingLoop(8); //start 	
	Uint32 delaytime=0;
	while(!quitKey && ii < wordcount)	//NUMLOOPS) //While the programs running
	{
		SDL_Event event; //This holds any events

		while(SDL_PollEvent(&event)) //Poll for events e.g (Key Input)
		{
			switch(event.type) //Test to see what type of event it is
			{
				/*case SDL_VIDEORESIZE: //If the window was resized
				{
					Resize(event.resize.w, event.resize.h); //Change OpenGL's view to reflect this
				}
				break;*/
				case SDL_KEYDOWN:
				{
	
					if (event.key.keysym.sym == SDLK_ESCAPE) //This checks for escape key presses
					{
						quitKey = SDL_TRUE; //End the program
					}					
					if (event.key.keysym.sym == SDLK_SPACE) //This checks for escape key presses
					{
						sprintf(str,"%d, SPACE_BAR, Key press\n", now);
						sendSignal(str,3);
						//event needs to finish out its alloted time with a blank screen
//x						textonscreen=SDL_FALSE; //this will end event prematurely
						//blank	screen
						SDL_FillRect ( screen , NULL , 0 ) ;
						// Update the screen
						SDL_UpdateRect(screen, 0, 0, 0, 0);
					}
					if (event.key.keysym.sym == SDLK_LSHIFT) 
					{
						sprintf(str,"%d, LEFT_SHIFT, Key press\n", now);
						sendSignal(str,3);
//x						textonscreen=SDL_FALSE;
						//blank	screen
						SDL_FillRect ( screen , NULL , 0 ) ;
						// Update the screen
						SDL_UpdateRect(screen, 0, 0, 0, 0);
					}
					if (event.key.keysym.sym == SDLK_RSHIFT) 
					{
						sprintf(str,"%d, RIGHT_SHIFT, Key press\n", now);
						sendSignal(str,3);
						
//x						textonscreen=SDL_FALSE;
						//blank	screen
						SDL_FillRect ( screen , NULL , 0 ) ;
						// Update the screen
						SDL_UpdateRect(screen, 0, 0, 0, 0);

					}
				}
				break;
				case SDL_QUIT: //If its a quit message
				{
					quitKey = SDL_TRUE; //End the program
				}
				break;
				default:
				break;
			}
		}
		now = SDL_GetTicks();
		if (textonscreen==SDL_FALSE) 
		{
			if (list1.scriptitem[ii].timeonscreen<=0)
				delaytime = delaycount+3000;
			else
				delaytime = delaycount+list1.scriptitem[ii].timeonscreen*1000;
		} else { 
			if (list1.scriptitem[ii].delayafter<=0)
				delaytime = delaycount+3000;
			else
				delaytime = delaycount+list1.scriptitem[ii].delayafter*1000;
		}
		if (now>=delaytime)	
		{
			if (textonscreen==SDL_FALSE) //textonscreen is a alternating flag - alternates between blank screen and text on screen
			{
				textonscreen=SDL_TRUE;
				SDL_FillRect ( screen , NULL , 0 ) ;//blank screen
				if (intro==SDL_TRUE) {
					if (introcounter >= introcount)
						intro=SDL_FALSE;
					sprintf(item, "%s", introlist[introcounter]);
					timingLoop(2); //write sync code to serial prot
					introcounter++;	
				} else {
					//if (textcounter >= numtextcount)
				//		textcounter=0;
					//string to ouput on screen
//					list1.scriptitem[textcounter].command= list1.scriptitem[textcounter].command.substr(0, str.find_last_not_of("\n \t")-1);
					//chomp(list1.scriptitem[textcounter].command);
					if (strlen(list1.scriptitem[ii].command) > 3 && isalnum((char)list1.scriptitem[ii].command[0] ) )
						sprintf (item, "%s::    %s", removeNL(list1.scriptitem[ii].command), list1.scriptitem[ii].word);
					else
						sprintf (item, "%s", list1.scriptitem[ii].word);
					ii++;
				}		
				//center text
				txthpos=(WIDTH/2)-(SFont_TextWidth(Neon, item)/2);
				txtvpos=(HEIGHT/2)-SFont_TextHeight(Neon)/2;
				//display text
				SFont_Write(screen, FONT, txthpos, txtvpos, item);

				sprintf(str, "%d, %s, Word on screen\n", now, item);	
				sendSignal(str,1);
			
				// Update the screen
				SDL_UpdateRect(screen, 0, 0, 0, 0);
			} else {
				textonscreen=SDL_FALSE;
				//blank screen
				SDL_FillRect ( screen , NULL , 0 ) ;
				// Update the screen
				SDL_UpdateRect(screen, 0, 0, 0, 0);
				//make sure signal resets to 0
				timingLoop(0);
			}
			delaycount=now;
		}		
	}

/*	// a simple text blit to (0/0) with Neon font
	SFont_Write(screen, Neon, 0, 0, "Top Left");
	// License Info...
	SFont_Write(screen, Copper, 60, 120, "SFont by Karl Bartel is GPL'ed!");
	// show some special chars
	SFont_Write(screen, Copper, 300, 260, "@--~!%&'_*,.:;");
	// demonstrates the use of TextWidth
	SFont_Write(screen, Neon, 640-SFont_TextWidth(Neon, "Bottom Right!"),
							  480-SFont_TextHeight(Neon),"Bottom Right!");
*/
//	time(&timeStruct);
//	sprintf(str, "End date and time: %s\n", ctime(&timeStruct));
	sendSignal(str,9);
	// Don't forget to free the fonts
	SFont_FreeFont(Copper);
	SFont_FreeFont(Neon);

	// Bye
	exit(0);
}
char* removeNL(char* str)
{
  int i=0, ii=0;
//  char str[]="C++";
  char tmpstr[255];
  while (str[i])
  {
///    if (isalpha(str[i])) printf ("character %c is alphabetic\n",str[i]);
//    else printf ("character %c is not alphabetic\n",str[i]);
	  if (isalnum(str[i])) {
		  tmpstr[ii]=str[i];
		  ii++;
	  }
    i++;
  }
  tmpstr[ii]=00;
  return tmpstr;
}
void runBaseline(long duration)
{
	SDL_bool quitKey = SDL_FALSE;
	

	time_t timeStruct;
	time(&timeStruct);
	sprintf(str, "\n"); 
	sendSignal(str, 0);
//	filelog.Log(str);
	sprintf(str, "Start date and time: %s\n", ctime(&timeStruct));
	sendSignal(str, 9);
//	filelog.Log(str);
	sprintf(str, "\nElapsed Time; Word; Start date and time: %s\n", ctime(&timeStruct));
	sendSignal(str, 0);
//	filelog.Log(str);
	
	Uint32 delaytime=duration+SDL_GetTicks();//3min+now
printf("delaytime: %d\n", delaytime);
	while(!quitKey && now<=delaytime)	//NUMLOOPS) //While the programs running
	{
		SDL_Event event; //This holds any events

		while(SDL_PollEvent(&event)) //Poll for events e.g (Key Input)
		{
			switch(event.type) //Test to see what type of event it is
			{
				/*case SDL_VIDEORESIZE: //If the window was resized
				{
					Resize(event.resize.w, event.resize.h); //Change OpenGL's view to reflect this
				}
				break;*/
				case SDL_KEYDOWN:
				{
	
					if (event.key.keysym.sym == SDLK_ESCAPE) //This checks for escape key presses
					{
						quitKey = SDL_TRUE; //End the program
					}					

				}
				break;
				case SDL_QUIT: //If its a quit message
				{
					quitKey = SDL_TRUE; //End the program
				}
				break;
				default:
				break;
			}
		}
		now = SDL_GetTicks();
	}
}
/*int setscreen(int width, int height, int depth, bool isfullscreen, SDL_Surface *screen)
//sets the resolution of the new window,
//receives the width and height of the screen, e.g 1024,768 and the bit
depth, plus a bool for whether to make it a fullscree
n window and pointer to the surface to perform this all on.
{

	screen = SDL_SetVideoMode(width, height, depth, SDL_HWSURFACE|SDL_ANYFORMAT);
    //create a screen as a window with the properties of a hardware display
    if ( screen==NULL )
    {
		std::cout<<"Could not set video mode"<<width<<"x"<<height<<"x"<<depth<<" ";
        return -1;
    }

    if ( isfullscreen == true )//if the surface must be full screen then
    {
		SDL_WM_ToggleFullScreen(screen);//the passed surface is upgraded to a fullscreen app
        //this will change the hardware resolution for the graphics card to the width and height of the surface to be made .
    }

    return 0;
}
//end of setscreen

*/

void sendSignal(char* str, int num)//char str[128];
//writes text to pc file and send num to active2 program (parallel port)
{
		filelog.Log(str);
		timingLoop(num);
}
