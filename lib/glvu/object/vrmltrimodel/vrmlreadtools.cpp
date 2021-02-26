//------------------------------------------------------------------------------
// File : vrmlreadtools.cpp
//------------------------------------------------------------------------------
// GLVU : Copyright 1997 - 2002 
//        The University of North Carolina at Chapel Hill
//------------------------------------------------------------------------------
// Permission to use, copy, modify, distribute and sell this software and its 
// documentation for any purpose is hereby granted without fee, provided that 
// the above copyright notice appear in all copies and that both that copyright 
// notice and this permission notice appear in supporting documentation. 
// Binaries may be compiled with this software without any royalties or 
// restrictions. 
//
// The University of North Carolina at Chapel Hill makes no representations 
// about the suitability of this software for any purpose. It is provided 
// "as is" without express or implied warranty.

//============================================================================
// vrmlreadtools.cpp
//============================================================================
#include "vrmlreadtools.hpp"
#include <ctype.h>

int SkipWhitespace(FILE *fp)
{
  int c;
  do { c=fgetc(fp); } while (isspace(c) && c!=EOF);
  if (c==EOF) return(0);
  ungetc(c,fp);
  return(1);
}

int SkipToEOL(FILE *fp)
{
  int c;
  do { c=fgetc(fp); } while (c!='\n' && c!=EOF);
  if (c==EOF) return(0);
  return(1);
}

int SkipComments(FILE *fp)
{
  char c;
  while(SkipWhitespace(fp))
  {
    c=fgetc(fp);
    if (c=='#')
      { if (!SkipToEOL(fp)) return(0); }
    else 
      { ungetc(c,fp); return(1); }
  }
  return(0);
}

// SKIPS THE STRING Word AND REMOVES IT FORM fp
int SkipWord(FILE *fp, char *Word)
{
  int c, i=0;
  fpos_t CurrPos;
  do 
  { 
    c=fgetc(fp); 
    if (c==EOF) return(0);
    if (c==Word[i])           // WORD CHAR FOUND
    {
      if (i==0) fgetpos(fp, &CurrPos);  // SAVE FILE POS
      i++;
    }
    else
    {
      if (i>0)
      {
        fsetpos(fp, &CurrPos);   // RESET FILE POS
        i=0;
      }
    }
  } while (Word[i]);

  return(1);
}

void readLine(FILE *fp, char *buf)
{
   while (fgets(buf,256,fp)) {
      if (buf[0] != '\n' && buf[0] != '\0' && buf[0] != '#')
         break;
   }
}


// CHECKS IF THE NEXT NON-WHITESPACE CHAR IS c. c IS NOT REMOVED FROM FP
int NextCharIs(FILE *fp, char c)  
{
  SkipWhitespace(fp);
  char d = fgetc(fp);
  ungetc(d,fp);
  return(c==d);
}

// GRABS THE NEXT NON-WHITESPACE CHAR. IT IS REMOVED FROM FP
char NextChar(FILE *fp)
{
  char c;
  fscanf(fp, "%1s", &c);
  return(c);
}


void PrintRestOfFile(FILE *fp)
{
  int c;
  while ((c=fgetc(fp)) != EOF)
    putchar(c);
}

void PrintNextN(FILE *fp, int N)
{
  int c;
  while ((c=fgetc(fp)) != EOF && N>0)
  {
    putchar(c);
    N--;
  }
}
