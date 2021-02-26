//load data
#include <stdio.h>
#include <stdlib.h> // for MAX_PATH
#include <direct.h> // for getcwd (in C:\Program Files\Microsoft Visual Studio\VC98\Include)
#include <string.h>
#include "loadlist.h"
//#include "sdlimage.h"
#include "SDL_image.h"
#include <cstdlib>


//char** list;
//int listcount;
LISTITEM list1;
SDL_Surface** images;
int imagecounter; 

char** introlist;
int introcount;
extern char SessionName[255];
//char *current_directory(char *path)
char *current_directory()
{
	char buffer[255];
	getcwd(buffer, _MAX_PATH);
	return buffer;
}

void readstr(FILE *f,char *string)
{
	string[0]='\0';//clear so that reading blank line doesn't 
	do				//return passed in value
	{
		fgets(string, 255, f);
//skip comments and blanks
	} while ((string[0] == '/') || (string[0] == '\n') || (string[0] == '#'));
	return;
}
int countLines(FILE* filein)
{
	int numlines=0;
	char oneline[255]; 
	//count num words in wordliist file
	while (!feof(filein))
	{
		if( ferror( filein ) )      {
			perror( "Read error" );
			break;
		}
		
		readstr(filein,oneline);
/*		if (strchr(oneline,'\t')) {
			numlines+=2;
		} else {*/
			numlines++;
		//}
	}
	return (numlines);
}
int LoadList(char fname[256])
{
	imagecounter=0;
	
	char word[210];
	int numlines=0;

	FILE *filein;
	char oneline[255]; char* buffer;
	filein=fopen(fname, "rt");

	numlines=countLines(filein);

	//need to be able to use images
	images=new SDL_Surface*[4];

	list1.scriptitem = new SCRIPTITEM[numlines];//reads last line twice??
	list1.numitems = numlines;
///////////////////////////////////////////ex
/*	script1.scriptitem = new SCRIPTITEM[numRitems];//reads last line twice??
	script1.numitems = numRitems;
	int i = 0;
	while(i<numRitems)

	{

		readstr(filein,oneline);

		switch (oneline[0]) {

		case 'T': //load new texture

		break;

		case 'M': //move camera

			sscanf(oneline, "%c %f %f %f %f %f", &command, &xang, &yang, &zang, &frames, &velocity);

			script1.scriptitem[i].command = command;//=M

			script1.scriptitem[i].xang = xang;
*/
///////////////////////////////////////ex
//	list1=new char*[numlines];
//	listcount=numlines;
	filein = freopen(fname, "rt", filein);				// File To Load World Data From
	for (int i =0; i < list1.numitems; i++)
	{
		readstr(filein,oneline);
////////////////////////////////////////////
//assume tab separated in order: word, timeonscreen, delayafter, command
//i.e. Won	2	3	don't speak
//task 4 test ...
//splits lines with tab, or nl
//don't know how many cols there are. each line could have different number
		int colcounter = 1;
		buffer=strtok(oneline,",\t\n");
		while (buffer != NULL) {
			switch (colcounter) {
			case 1:
				strcpy(list1.scriptitem[i].word,buffer);				
				break;
			case 2:
				list1.scriptitem[i].timeonscreen=atoi(buffer);
				break;
			case 3:
				list1.scriptitem[i].delayafter=atoi(buffer);
				break;
			case 4:
				strcpy(list1.scriptitem[i].command,buffer);				
				break;
			default:
				break;
			}
			colcounter++;			
			buffer=strtok(NULL,",\t");
		}

//assume tab separated in order: word, timeonscreen, delayafter, command
//i.e. Won	2	3	don't speak
//task 4 test ...
//splits lines with tab, or nl
//don't know how many cols there are. each line could have different number
/*		buffer=strtok(oneline,"\t\n");
//		buffer=strtok(oneline,"\n");
		while (buffer != NULL) {
			list1[w]=new char[sizeof(buffer)];
			strcpy(list1[w],buffer);
			buffer=strtok(NULL,"\t");
//			buffer=strtok(NULL,"\n");
			w++;
		}
*/
////////////////////
////////////////////////////////////////////
/*old method
		sscanf(oneline, "%s", word);
		//remove newline chars
		strcpy(oneline,strtok(oneline,"\n"));

			list[w]=new char[sizeof(oneline)];

			strcpy(list[w],oneline);
			w++;
*/
//		}
	}
	fclose(filein);
	return(i);
}


void LoadIntroList()
{
	imagecounter=0;
	
	char word[210];
	int numitems=0;

	FILE *filein;
	char oneline[255];
	filein = fopen("intro.txt", "rt");	// File To Load World Data From
	//count num words in wordliist file
	while (!feof(filein))
	{
		if( ferror( filein ) )      {
			perror( "Read error" );
			break;
		}
		readstr(filein,oneline);
		numitems++;
	}

	images=new SDL_Surface*[4];

	introlist=new char*[numitems];
	introcount=numitems;
	filein = freopen("intro.txt", "rt", filein);				// File To Load World Data From

	for (int i =0; i < numitems; i++)
	{
		readstr(filein,oneline);
		sscanf(oneline, "%s", word);
		//remove newline chars
		strcpy(oneline,strtok(oneline,"\n"));

/*if (_strnicmp( oneline, ".jpg", 4)) {
			list[i]=new char[sizeof(oneline)+sizeof("images\\")];
			strcpy(list[i],	"images\\");
			strcat(list[i], oneline);
			images[imagecounter]= load_image( list[i] ); 
			imagecounter++;
//			background = load_image( "background.bmp" );
//Now the images are loaded using the image loading function we made.
//Apply the background to the screen 
//			apply_surface( 0, 0, background, screen ); 
		} else {
*/
			introlist[i]=new char[sizeof(oneline)];

			strcpy(introlist[i],oneline);
//		}
	}
	fclose(filein);
	return;
}

void LoadListBAK(char fname[256])
{
	/*
	char  aWord[100];   // a temporary place to hold neword
char* words[1000];  // array of pointers to c-strings
int   n = 0;        // number of words
. . .
while (cin >> aWord) {
    int len = strlen(aWord) + 1;    // how much space is needed
    char* newSpace = new char[len]; // allocate with new
    strcpy(newSpace, aWord);        // copy to new space
    words[n] = newSpace;            // save pointer
    n++;
}*/


	imagecounter=0;
	
	char word[210];
	int numitems=0, listcount=0;
	char** list;

	FILE *filein;
	char oneline[255]; char buffer[255];
//	filein = fopen("wordlist.txt", "rt");	// File To Load World Data From
	filein=fopen(fname, "rt");
	//count num words in wordliist file
	while (!feof(filein))
	{
		if( ferror( filein ) )      {
			perror( "Read error" );
			break;
		}
		readstr(filein,oneline);
		numitems++;
	}
//To read characters to end of data, use a loop such as either
//
//        char ch;
//        while( ch = getchar(), ch >= 0 ) /* ch < 0 indicates end-of-data */
//
//or
//
//        char ch;
//        while( scanf( "%c", &ch ) == 1 ) /* one character read */

	images=new SDL_Surface*[4];

	list=new char*[numitems];
	listcount=numitems;
	filein = freopen(fname, "rt", filein);				// File To Load World Data From

	for (int i =0; i < numitems; i++)
	{
		readstr(filein,oneline);
////////////////////////////////////////////
//task 4 test ...
		if (strchr(oneline,'\t')) {
			sscanf(buffer,"%s\t%s",word,word);
			int x = 10;
		}
////////////////////////////////////////////
		sscanf(oneline, "%s", word);
		//remove newline chars
		strcpy(oneline,strtok(oneline,"\n"));

/*if (_strnicmp( oneline, ".jpg", 4)) {
			list[i]=new char[sizeof(oneline)+sizeof("images\\")];
			strcpy(list[i],	"images\\");
			strcat(list[i], oneline);
			images[imagecounter]= load_image( list[i] ); 
			imagecounter++;
//			background = load_image( "background.bmp" );
//Now the images are loaded using the image loading function we made.
//Apply the background to the screen 
//			apply_surface( 0, 0, background, screen ); 
		} else {
*/
			list[i]=new char[sizeof(oneline)];

			strcpy(list[i],oneline);
//		}
	}
	fclose(filein);
	return;
}
