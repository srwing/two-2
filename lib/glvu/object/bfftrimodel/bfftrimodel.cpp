//------------------------------------------------------------------------------
// File : bfftrimodel.cpp
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
// bfftrimod.cpp
//============================================================================
#include <stdlib.h>
#include <time.h>
#include <vrmlreadtools.hpp>
#include "bfftri.hpp"
#include "bfftrimodel.hpp"

BffTriModel::BffTriModel()
{
  Objects=NULL;
  NumObjects=0;
}

BffTriModel::BffTriModel(const char *FileName, float Scale)
{
  Objects=NULL;
  NumObjects=0;
  Read(FileName,Scale);
}

BffTriModel::~BffTriModel()
{
  for (int i=0; i<NumObjects; i++)
    delete Objects[i];
  delete[] Objects;
}

void BffTriModel::UpdateMinMax()
{
  Min = Objects[0]->Min;
  Max = Objects[0]->Max;
  for (int i=1; i<NumObjects; i++)
  {
    if (Objects[i]->Min.x < Min.x) Min.x=Objects[i]->Min.x;
    else if (Objects[i]->Max.x > Max.x) Max.x=Objects[i]->Max.x;
    if (Objects[i]->Min.y < Min.y) Min.y=Objects[i]->Min.y;
    else if (Objects[i]->Max.y > Max.y) Max.y=Objects[i]->Max.y;
    if (Objects[i]->Min.z < Min.z) Min.z=Objects[i]->Min.z;
    else if (Objects[i]->Max.z > Max.z) Max.z=Objects[i]->Max.z;
  }
}

void BffTriModel::AddObject()
{
  int i;
  NumObjects++;
  BffTri **NewObjects = new BffTri*[NumObjects];
  for (i=0; i<(NumObjects-1); i++)
    NewObjects[i]=Objects[i];
  delete[] Objects;
  Objects=NewObjects;
  Objects[i] = new BffTri;  // CREATE THE NEW BFF OBJECT
}

void BffTriModel::Read(const char *FileName, float Scale)
{
  printf("loading bff model [%s]... ", FileName);
  clock_t Time = clock();

  AddObject();                                    // CREATE NEW BFF MODEL
  Objects[NumObjects-1]->ReadBFF(FileName,Scale);   // READ IN BFF OBJECT

  // INITIALIZATION
  UpdateMinMax();

  printf("%.2f seconds\n", (clock()-Time)/(float)CLOCKS_PER_SEC);
}

void BffTriModel::DisplayStats()
{
  printf("NumObjects: %d\n", NumObjects);
  int TotalVerts=0, TotalTris=0;
  for (int i=0; i<NumObjects; i++)
  {
    printf("[%d]  NumVerts: %d  NumTris: %d\n", i, Objects[i]->NumVerts, Objects[i]->NumTris);
    TotalVerts += Objects[i]->NumVerts;
    TotalTris += Objects[i]->NumTris;
  }
  printf("Total Verts: %d\n", TotalVerts);
  printf(" Total Tris: %d\n", TotalTris);
}

void BffTriModel::GetTris(float* &tris, int &numtris)
{
  // ALLOC ARRAY ONLY IF tris==NULL; OTHERWISE, MUST HAVE BEEN PREALLOCED
  numtris = GetNumTris();
  if (tris==NULL) tris = new float[numtris*9];  // 9 FLOATS/TRI

  // STUFF ARRAY WITH TRIANGLE VERTEX COMPONENTS
  int k=0;
  Vec3f V;
  float *T = tris;
  for (int j=0; j<NumObjects; j++)
    for (int i=0; i<Objects[j]->NumTris; i++)
    {
      V = Objects[j]->Verts[ Objects[j]->Tris[i].iA ].V;
      T[k] = V.x;  k++;
      T[k] = V.y;  k++;
      T[k] = V.z;  k++;
      V = Objects[j]->Verts[ Objects[j]->Tris[i].iB ].V;
      T[k] = V.x;  k++;
      T[k] = V.y;  k++;
      T[k] = V.z;  k++;
      V = Objects[j]->Verts[ Objects[j]->Tris[i].iC ].V;
      T[k] = V.x;  k++;
      T[k] = V.y;  k++;
      T[k] = V.z;  k++;
    }
}

int BffTriModel::GetNumTris()
{
  // COUNT NUMBER OF TRIS IN MODEL
  int NumTris=0;
  for (int j=0; j<NumObjects; j++)
    NumTris += Objects[j]->NumTris;
  return(NumTris);
}
