	

#ifndef LOADFILE_H
#define LOADFILE_H
//#include <conio.h>
//#include <windows.h>
//#include <stdlib.h>//The stdlib header
//#include <stdio.h>//The stdio header

#ifdef __cplusplus
extern "C"
{
#endif		// __cplusplus 
	typedef struct tagSCRIPTITEM
	{
	//assume tab separated in order: word, timeonscreen, delayafter, command
	//i.e. Won	2	3	don't speak
		char word[255];
		int timeonscreen, delayafter; //in secounds
		char command[255];
	} SCRIPTITEM;

	typedef struct tagLISTITEM
	{
		int numitems;
		SCRIPTITEM* scriptitem;
	} LISTITEM;

	void readstr(FILE *f,char *string);
//	void LoadList();
	int LoadList(char fname[256]);
	void LoadIntroList();
	int countLines(FILE* filein);

#ifdef __cplusplus
}
#endif		// __cplusplus 

#endif

