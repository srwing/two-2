//------------------------------------------------------------------------------
// File : texdepthmeshset.cpp
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
// texdepthmeshset.cpp
//============================================================================
#include "texdepthmeshset.hpp"

TexturedDepthMeshSet::TexturedDepthMeshSet()
{
  NumTDMs=0;
  NumTDMsAlloced=0;
  TDMs=NULL;
}

TexturedDepthMeshSet::~TexturedDepthMeshSet()
{
  for (int i=0; i<NumTDMsAlloced; i++) delete TDMs[i];
  delete[] TDMs;
}

void TexturedDepthMeshSet::Begin()
{
}

void TexturedDepthMeshSet::AddTDM(Object *Obj, Camera *Cam, int NumCols, int NumRows)
{
  if (NumTDMs==NumTDMsAlloced) DoubleArray();
  TDMs[NumTDMs] = new TexturedDepthMesh(Obj,Cam,NumCols,NumRows);
  NumTDMs++;
}

void TexturedDepthMeshSet::AddTDM(TexturedDepthMesh *TDM)
{
  if (NumTDMs==NumTDMsAlloced) DoubleArray();
  TDMs[NumTDMs]=TDM;
  NumTDMs++;
}

void TexturedDepthMeshSet::End()
{
  PackArray();
}

//----------------------------------------------------------------------------
// Display a selected subset of the TDM set
//----------------------------------------------------------------------------
void TexturedDepthMeshSet::DisplaySet(TexturedDepthMesh* *tdms, int numtdms,
                                      int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  for (int i=0; i<numtdms; i++)
  {
    tdms[i]->UseProjTex = UseProjTex;
    tdms[i]->UseDisplayLists = UseDisplayLists;
    if (UseMesh)
      if (tdms[i]->UseProjTex) 
        if (StepSize==1) tdms[i]->DisplayAsMesh(); else tdms[i]->DisplayAsMesh(StepSize);
      else
        if (StepSize==1) tdms[i]->DisplayAsMeshWithColor(); else tdms[i]->DisplayAsMeshWithColor(StepSize);
    else
      if (tdms[i]->UseProjTex)
        if (StepSize==1) tdms[i]->DisplayAsPts(); else tdms[i]->DisplayAsPts(StepSize);
      else
        if (StepSize==1) tdms[i]->DisplayAsPtsWithColor(); else tdms[i]->DisplayAsPtsWithColor(StepSize);
  }
}

//----------------------------------------------------------------------------
// Full-set display routines
//----------------------------------------------------------------------------
void TexturedDepthMeshSet::DisplayAll(int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  DisplaySet(TDMs,NumTDMs, UseMesh,StepSize,UseProjTex,UseDisplayLists);
}

void TexturedDepthMeshSet::DisplayAllCams()
{
  for (int i=0; i<NumTDMs; i++)
    TDMs[i]->Display();
}

//----------------------------------------------------------------------------
// Display the TDM whose origin is closest to the given camera's origin
//----------------------------------------------------------------------------
void TexturedDepthMeshSet::Display_ClosestPos(Camera *Cam, int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  if (NumTDMs<1) return;
  TexturedDepthMesh *ClosestTDM = TDMs[0];
  float ClosestDist = (TDMs[0]->Orig - Cam->Orig).LengthSqr();
  float NewDist;
  for (int i=1; i<NumTDMs; i++)
  {
    NewDist = (TDMs[i]->Orig - Cam->Orig).LengthSqr();
    if (NewDist < ClosestDist)
    { 
      ClosestDist=NewDist;
      ClosestTDM=TDMs[i];
    }
  }
  DisplaySet(&ClosestTDM,1,UseMesh,StepSize,UseProjTex,UseDisplayLists);
}

//----------------------------------------------------------------------------
// Display the TDM whose viewing direction is most similar to the given
// camera's viewing direction.
//----------------------------------------------------------------------------
void TexturedDepthMeshSet::Display_ClosestDir(Camera *Cam, int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  if (NumTDMs<1) return;
  TexturedDepthMesh *ClosestTDM = TDMs[0];
  float ClosestCosAng = TDMs[0]->Z * Cam->Z;
  float NewCosAng;
  for (int i=1; i<NumTDMs; i++)
  {
    NewCosAng = TDMs[i]->Z * Cam->Z;
    if (NewCosAng > ClosestCosAng)
    { 
      ClosestCosAng=NewCosAng;
      ClosestTDM=TDMs[i];
    }
  }
  DisplaySet(&ClosestTDM,1,UseMesh,StepSize,UseProjTex,UseDisplayLists);
}

//----------------------------------------------------------------------------
// Display the N TDM's whose origins are closest to the given camera's origin
// (in order from best to worst)
//----------------------------------------------------------------------------
void TexturedDepthMeshSet::DisplaySet_ClosestPos(Camera *Cam, int NumToUse, int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  int i;
  if (NumTDMs<1) return;
  if (NumToUse>NumTDMs) NumToUse=NumTDMs;
  
  TexturedDepthMesh **SortedTDMs = new TexturedDepthMesh*[NumTDMs];
  float *Dist2 = new float[NumTDMs];
  for (i=0; i<NumTDMs; i++)
  {
    SortedTDMs[i]=TDMs[i];
    Dist2[i] = (SortedTDMs[i]->Orig - Cam->Orig).LengthSqr();
  }

  TexturedDepthMesh *t1;
  float t2;
  for (i=0; i<NumToUse; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (Dist2[j] < Dist2[i])
      {
        t1=SortedTDMs[i]; SortedTDMs[i]=SortedTDMs[j]; SortedTDMs[j]=t1;
        t2=Dist2[i];      Dist2[i]=Dist2[j];           Dist2[j]=t2;
      }
    
  DisplaySet(SortedTDMs,NumToUse,UseMesh,StepSize,UseProjTex,UseDisplayLists);

  delete[] SortedTDMs;
  delete[] Dist2;
}

//----------------------------------------------------------------------------
// Display the N TDMs whose viewing directions are most similar to the given
// camera's viewing direction (in order from best to worst).
//----------------------------------------------------------------------------
void TexturedDepthMeshSet::DisplaySet_ClosestDir(Camera *Cam, int NumToUse, int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  int i;
  if (NumTDMs<1) return;
  if (NumToUse>NumTDMs) NumToUse=NumTDMs;
  
  TexturedDepthMesh **SortedTDMs = new TexturedDepthMesh*[NumTDMs];
  float *CosAng = new float[NumTDMs];
  for (i=0; i<NumTDMs; i++)
  {
    SortedTDMs[i]=TDMs[i];
    CosAng[i] = TDMs[i]->Z * Cam->Z;
  }

  TexturedDepthMesh *t1;
  float t2;
  for (i=0; i<NumToUse; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (CosAng[j] > CosAng[i])
      {
        t1=SortedTDMs[i]; SortedTDMs[i]=SortedTDMs[j]; SortedTDMs[j]=t1;
        t2=CosAng[i];     CosAng[i]=CosAng[j];         CosAng[j]=t2;
      }
    
  DisplaySet(SortedTDMs,NumToUse,UseMesh,StepSize,UseProjTex,UseDisplayLists);

  delete[] SortedTDMs;
  delete[] CosAng;
}

//----------------------------------------------------------------------------
// Display the N "best fit" TDMs for the given camera. This version grabs
// the NumToUse*2 closest in direction and then grabs the NumToUse of those
// closest in position.
//----------------------------------------------------------------------------
void TexturedDepthMeshSet::DisplaySet_ClosestDirPos(Camera *Cam, int NumToUse, int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  int i;
  if (NumTDMs<1) return;
  if (NumToUse>NumTDMs) NumToUse=NumTDMs;
  
  TexturedDepthMesh **SortedTDMs = new TexturedDepthMesh*[NumTDMs];
  float *Quality = new float[NumTDMs];
  TexturedDepthMesh *t1;
  float t2;
  int N;

  // FIRST STORE DIRECTION "QUALITY" FOR EACH TDM
  for (i=0; i<NumTDMs; i++)
  {
    SortedTDMs[i]=TDMs[i];
    Quality[i] = TDMs[i]->Z * Cam->Z;
  }

  // FIND NumToUse*2 NEAREST DIRECTION TDMS BY SORTING
  N = (NumToUse*2>NumTDMs) ? NumTDMs : NumToUse*2;
  for (i=0; i<N; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (Quality[j] > Quality[i])
      {
        t1=SortedTDMs[i]; SortedTDMs[i]=SortedTDMs[j]; SortedTDMs[j]=t1;
        t2=Quality[i];    Quality[i]=Quality[j];       Quality[j]=t2;
      }
    
  // STORE NumToUse*2 POSITION DISTANCE "QUALITIES"
  N = (NumToUse*2>NumTDMs) ? NumTDMs : NumToUse*2;
  for (i=0; i<N; i++)
    Quality[i] = (SortedTDMs[i]->Orig - Cam->Orig).LengthSqr();

  // FIND NumToUse NEAREST POSITION TDMS BY SORTING OVER NEAREST DIRECTION SET
  N = (NumToUse>NumTDMs) ? NumTDMs : NumToUse;
  for (i=0; i<N; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (Quality[j] < Quality[i])
      {
        t1=SortedTDMs[i]; SortedTDMs[i]=SortedTDMs[j]; SortedTDMs[j]=t1;
        t2=Quality[i];    Quality[i]=Quality[j];       Quality[j]=t2;
      }

  DisplaySet(SortedTDMs,NumToUse,UseMesh,StepSize,UseProjTex,UseDisplayLists);

  delete[] SortedTDMs;
  delete[] Quality;
}

//----------------------------------------------------------------------------
// Uses a weighted average of the normalized direction and position qualities
// to determine the N "best fit" TDMs for the current camera view. DirPos
// is a value in [0,1] where 1 means position is used entirely to determine
// the best and 0 means direction is used.
//----------------------------------------------------------------------------
void TexturedDepthMeshSet::DisplaySet_ClosestDirPos(Camera *Cam, int NumToUse, float DirWeight, int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  float PosWeight = 1 - DirWeight;
  
  int i;
  if (NumTDMs<1) return;
  if (NumToUse>NumTDMs) NumToUse=NumTDMs;
  
  TexturedDepthMesh **SortedTDMs = new TexturedDepthMesh*[NumTDMs];
  float *Dist2 = new float[NumTDMs];
  float *CosAng = new float[NumTDMs];
  float *Quality = new float[NumTDMs];
  float MaxDist=0;
  for (i=0; i<NumTDMs; i++)
  {
    SortedTDMs[i]=TDMs[i];
    Dist2[i] = (SortedTDMs[i]->Orig - Cam->Orig).LengthSqr();
    if (Dist2[i] > MaxDist) MaxDist=Dist2[i];
    CosAng[i] = TDMs[i]->Z * Cam->Z;
  }

  // CALCULATE WEIGHTED AVERAGE OF NORMALIZED QUALITIES
  for (i=0; i<NumTDMs; i++)
    Quality[i] = PosWeight * (1-(Dist2[i]/MaxDist)) +
                 DirWeight * ((CosAng[i]+1)*0.5);
  
  TexturedDepthMesh *t1;
  float t2;
  for (i=0; i<NumToUse; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (Quality[j] > Quality[i])
      {
        t1=SortedTDMs[i]; SortedTDMs[i]=SortedTDMs[j]; SortedTDMs[j]=t1;
        t2=Quality[i];    Quality[i]=Quality[j];       Quality[j]=t2;
      }
    
  DisplaySet(SortedTDMs,NumToUse,UseMesh,StepSize,UseProjTex,UseDisplayLists);

  delete[] SortedTDMs;
  delete[] Dist2;
  delete[] CosAng;
  delete[] Quality;
}

//----------------------------------------------------------------------------
// DYNAMIC ARRAY ROUTINES
//----------------------------------------------------------------------------
void TexturedDepthMeshSet::DoubleArray()
{
  if (NumTDMsAlloced==0) NumTDMsAlloced=1; else NumTDMsAlloced*=2;
  TexturedDepthMesh* *NewTDMs = new TexturedDepthMesh*[NumTDMsAlloced];
  for (int i=0; i<NumTDMs; i++) NewTDMs[i]=TDMs[i];
  delete[] TDMs;
  TDMs=NewTDMs;
}

void TexturedDepthMeshSet::PackArray()
{
  NumTDMsAlloced = NumTDMs;
  TexturedDepthMesh* *NewTDMs = new TexturedDepthMesh*[NumTDMsAlloced];
  for (int i=0; i<NumTDMs; i++) NewTDMs[i]=TDMs[i];
  delete[] TDMs;
  TDMs=NewTDMs;
}
