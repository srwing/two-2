//------------------------------------------------------------------------------
// File : vrmltrimodel.cpp
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
// vrmltrimod.cpp
//============================================================================
#include <stdlib.h>
#include <time.h>
#include "vrmltrimodel.hpp"
#include "vrmlreadtools.hpp"

VrmlTriModel::VrmlTriModel()
{
  Objects=NULL;
  NumObjects=0;
}

VrmlTriModel::VrmlTriModel(const char *FileName, float Scale)
{
  Objects=NULL;
  NumObjects=0;
  Read(FileName,Scale);
}

VrmlTriModel::~VrmlTriModel()
{
  for (int i=0; i<NumObjects; i++)
    delete Objects[i];
  delete[] Objects;
}

void VrmlTriModel::UpdateMinMax()
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

void VrmlTriModel::AddObject()
{
  NumObjects++;
  VrmlTri **NewObjects = new VrmlTri*[NumObjects];
  int i;
  for (i=0; i<(NumObjects-1); i++)
    NewObjects[i]=Objects[i];
  delete[] Objects;
  Objects=NewObjects;
  Objects[i] = new VrmlTri;  // CREATE THE NEW VRML OBJECT
}

void VrmlTriModel::Read(const char *FileName, float Scale)
{
  printf("loading vrml model [%s]... ", FileName);
  clock_t Time = clock();

  FILE *fp = fopen(FileName,"r");
  if (fp==NULL) { printf("unable to open [%s]!\n",FileName); exit(1); }
  SkipComments(fp);
  SkipWord(fp,"Separator");
  while ( NextChar(fp) != '}' )
  {
    AddObject();                                    // CREATE NEW VrmlTri MODEL
    Objects[NumObjects-1]->ReadFromVRML(fp,Scale);  // READ IN VRML OBJECT
  }
  fclose(fp);

  // INITIALIZATION
  UpdateMinMax();

  printf("%.2f seconds\n", (clock()-Time)/(float)CLOCKS_PER_SEC);
}

void VrmlTriModel::Write(const char* FileName)
{
  FILE* fp = fopen(FileName,"w");
  fprintf(fp,"#VRML V1.0 ascii\n\n");   // HEADER
  fprintf(fp,"Separator\n{\n");         // START OF MODEL

  for (int i=0; i<NumObjects; i++)
    Objects[i]->PrintObjToVRML(fp);

  fprintf(fp,"}\n");                   // FINISH THE VRML FILE

  fclose(fp);
}

void VrmlTriModel::DisplayStats()
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

void VrmlTriModel::GetTris(float* &tris, int &numtris)
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

void VrmlTriModel::GetTris(double* &tris, int &numtris)
{
  // ALLOC ARRAY ONLY IF tris==NULL; OTHERWISE, MUST HAVE BEEN PREALLOCED
  numtris = GetNumTris();
  if (tris==NULL) tris = new double[numtris*9];  // 9 FLOATS/TRI

  // STUFF ARRAY WITH TRIANGLE VERTEX COMPONENTS
  int k=0;
  Vec3f V;
  double *T = tris;
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


int VrmlTriModel::GetNumTris()
{
  // COUNT NUMBER OF TRIS IN MODEL
  int NumTris=0;
  for (int j=0; j<NumObjects; j++)
    NumTris += Objects[j]->NumTris;
  return(NumTris);
}
