//------------------------------------------------------------------------------
// File : pgm.cpp
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
// pgm.hpp : Portable Grayscale Map image format module
//============================================================================

#include <stdio.h>

#define PGM_VERBOSE 0

//----------------------------------------------------------------------------
// READS AN IMAGE IN FROM A PPM FILE. RETURNS THE COLOR RGB ARRAY AND DIMENSIONS
// PERFORMS AUTO-ALLOCATION OF Color ARRAY IF SET TO NULL BEFORE CALLING; OTHERWISE
// ASSUMES THAT COLOR HAS BEEN PRE-ALLOCED.
//----------------------------------------------------------------------------
void LoadPGM(const char *FileName, unsigned char* &GrayBuffer, int &Width, int &Height)
{
  FILE* fp = fopen(FileName, "rb");
  if (fp==NULL) 
    { printf("PGM ERROR (LoadPGM): unable to open %s!\n",FileName);
      GrayBuffer=NULL; Width=0; Height=0; return; }
  int c,s;
  do{ do { s=fgetc(fp); } while (s!='\n'); } while ((c=fgetc(fp))=='#');
  ungetc(c,fp);
  fscanf(fp, "%d %d\n255\n", &Width, &Height);
#if PGM_VERBOSE
  printf("Reading %dx%d PGM [%s]. . .\n", Width, Height, FileName);
#endif
  int NumComponents = Width*Height;
  if (GrayBuffer==NULL) GrayBuffer = new unsigned char[NumComponents];
  fread(GrayBuffer,NumComponents,1,fp);
  fclose(fp);
}

//----------------------------------------------------------------------------
// Writes an unsigned byte RGB color array out to a PPM file.
//----------------------------------------------------------------------------
void WritePGM(const char *FileName, unsigned char* GrayBuffer, int Width, int Height)
{
  FILE* fp = fopen(FileName, "wb");
  if (fp==NULL) { printf("PGM ERROR (WritePGM) : unable to open %s!\n",FileName); return; }
#if PGM_VERBOSE
  printf("Writing %dx%d PGM [%s]. . .\n", Width, Height, FileName);
#endif
  fprintf(fp, "P5\n%d %d\n255\n", Width, Height);
  fwrite(GrayBuffer,1,Width*Height,fp);
  fclose(fp);
}
