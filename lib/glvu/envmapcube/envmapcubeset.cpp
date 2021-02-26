//------------------------------------------------------------------------------
// File : envmapcubeset.cpp
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
// envmapcubeset.cpp
//============================================================================
#include "envmapcubeset.hpp"

EnvMapCubeSet::EnvMapCubeSet()
{
  NumEMCs=0;
  NumEMCsAlloced=0;
  EMCs=NULL;
}

EnvMapCubeSet::~EnvMapCubeSet()
{
  delete[] EMCs;
}

void EnvMapCubeSet::Begin()
{
}

void EnvMapCubeSet::AddEMC(Object *Obj, Vec3f Cntr, float HalfDim, int N)
{
  if (NumEMCs==NumEMCsAlloced) DoubleArray();
  EMCs[NumEMCs] = new EnvMapCube(Obj,Cntr,HalfDim,N);
  NumEMCs++;
}

void EnvMapCubeSet::AddEMC(EnvMapCube *EMC)
{
  if (NumEMCs==NumEMCsAlloced) DoubleArray();
  EMCs[NumEMCs]=EMC;
  NumEMCs++;
}

void EnvMapCubeSet::End()
{
  PackArray();
}

void EnvMapCubeSet::DisplayAll(int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  for (int i=0; i<NumEMCs; i++)
  {
    EMCs[i]->SetAllFacesVisible();
    EMCs[i]->Display(UseMesh,StepSize,UseProjTex,UseDisplayLists);
  }
}

void EnvMapCubeSet::DisplayAll(Camera *Cam, int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  for (int i=0; i<NumEMCs; i++)
  {
    EMCs[i]->FindVisibleCubeFaces(Cam);
    EMCs[i]->Display(UseMesh,StepSize,UseProjTex,UseDisplayLists);
  }
}

void EnvMapCubeSet::DisplayAllCubes(float Color[3])
{
  for (int i=0; i<NumEMCs; i++)
    EMCs[i]->DisplayCube(Color);
}

void EnvMapCubeSet::DisplaySet(EnvMapCube* *emcs, int numemcs,
                               int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists)
{
  for (int i=0; i<numemcs; i++)
    emcs[i]->Display(UseMesh,StepSize,UseProjTex,UseDisplayLists);
}

void EnvMapCubeSet::DisplaySet_ClosestPos(Camera *Cam, int NumToUse, int UseMesh, int StepSize, int UseProjTex, int UseDisplayLists, int VisCullOn)
{
  int i;
  if (NumEMCs<1) return;
  if (NumToUse>NumEMCs) NumToUse=NumEMCs;
  
  EnvMapCube **SortedEMCs = new EnvMapCube*[NumEMCs];
  float *Dist2 = new float[NumEMCs];
  for (i=0; i<NumEMCs; i++)
  {
    SortedEMCs[i]=EMCs[i];
    Dist2[i] = (SortedEMCs[i]->Cntr - Cam->Orig).LengthSqr();
  }

  EnvMapCube *t1;
  float t2;
  for (i=0; i<NumToUse; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumEMCs; j++)
      if (Dist2[j] < Dist2[i])
      {
        t1=SortedEMCs[i]; SortedEMCs[i]=SortedEMCs[j]; SortedEMCs[j]=t1;
        t2=Dist2[i];      Dist2[i]=Dist2[j];           Dist2[j]=t2;
      }

  if (VisCullOn)
    for (i=0; i<NumToUse; i++)
      EMCs[i]->FindVisibleCubeFaces(Cam);
  else
    for (i=0; i<NumToUse; i++)
      EMCs[i]->SetAllFacesVisible();
    
  DisplaySet(SortedEMCs,NumToUse,UseMesh,StepSize,UseProjTex,UseDisplayLists);

  delete[] SortedEMCs;
  delete[] Dist2;
}

//----------------------------------------------------------------------------
// DYNAMIC ARRAY ROUTINES
//----------------------------------------------------------------------------
void EnvMapCubeSet::DoubleArray()
{
  if (NumEMCsAlloced==0) NumEMCsAlloced=1; else NumEMCsAlloced*=2;
  EnvMapCube* *NewEMCs = new EnvMapCube*[NumEMCsAlloced];
  for (int i=0; i<NumEMCs; i++) NewEMCs[i]=EMCs[i];
  delete[] EMCs;
  EMCs=NewEMCs;
}

void EnvMapCubeSet::PackArray()
{
  NumEMCsAlloced = NumEMCs;
  EnvMapCube* *NewEMCs = new EnvMapCube*[NumEMCsAlloced];
  for (int i=0; i<NumEMCs; i++) NewEMCs[i]=EMCs[i];
  delete[] EMCs;
  EMCs=NewEMCs;
}
