//------------------------------------------------------------------------------
// File : vrmltri.cpp
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
// vrmltri.cpp
//============================================================================

#include <stdlib.h>
#include "vrmltri.hpp"
#include "vrmlreadtools.hpp"

//----------------------------------------------------------------------------
// CONSTRUCTOR : READ IN VRML MODEL (ver 1.0 - all triangles)
//----------------------------------------------------------------------------
VrmlTri::VrmlTri()
{
  NumTris=NumVerts=0;
  NumTrisAlloced=NumVertsAlloced=1;
  Tris = new SharedVertTri[NumTrisAlloced];
  Verts = new NormVert[NumVertsAlloced];
}

VrmlTri::~VrmlTri()
{
  delete[] Tris;
  delete[] Verts;
}

//----------------------------------------------------------------------------
// ENTIRE VRML FILE IS A SINGLE TRI OBJECT
//----------------------------------------------------------------------------
void VrmlTri::ReadVRML(const char *FileName, float Scale)
{
  FILE *fp = fopen(FileName,"r");
  if (fp==NULL) { printf("unable to open [%s]!\n",FileName); exit(1); }
  SkipComments(fp);
  SkipWord(fp,"Separator");
  ReadFromVRML(fp,Scale);          // READ IN SINGLE VRML OBJECT
  fclose(fp);
}

//----------------------------------------------------------------------------
// READ A SINGLE OBJECT FROM THE ALREADY OPEN VRML FILE
//----------------------------------------------------------------------------
void VrmlTri::ReadFromVRML(FILE *fp, float Scale)
{
  // READ IN THE DIFFUSE COLOR
  SkipWord(fp,"diffuseColor");
  fscanf(fp,"%f %f %f", &Color.x, &Color.y, &Color.z);

  // READ IN THE VERTEX LIST
  Vec3f V;
  SkipWord(fp,"point");
  while ( NextChar(fp) != ']' )
  {
    fscanf(fp,"%f %f %f", &V.x, &V.y, &V.z);
    if (Scale!=1) V*=Scale;
    AddVertex(V);
  }

  // READ IN THE TRIANGLE LIST
  int iA, iB, iC;
  SkipWord(fp,"coordIndex");
  while ( NextChar(fp) != ']' )
  {
    fscanf(fp, "%d %*1s %d %*1s %d %*1s %*d", &iA, &iB, &iC);
    AddTri(iA,iB,iC);
  }
  SkipWord(fp,"}");

  // DO INITIALIZATION
  CompressArrays();
  CalcExtents();
  CalcAllNormals();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void VrmlTri::ReadFromTri(const char *FileName, float Scale)
{
  FILE* fp = fopen(FileName, "r");
  if (fp==NULL) { printf("ERROR: unable to open SharedVertNormTriModel [%s]!\n",FileName); exit(1); }
  fscanf(fp, "%f %f %f", &Color.x, &Color.y, &Color.z); // READ IN COLOR
  fscanf(fp, "%d", &NumVerts);       // READ IN NUM OF VERTS
  Verts = new NormVert[NumVerts];    // ALLOC SHARED VERTEX ARRAY
  int i;
  for (i=0; i<NumVerts; i++)         // READ IN THE VERTICES
  {
    fscanf(fp, "%f %f %f", &Verts[i].V.x, &Verts[i].V.y, &Verts[i].V.z);
    if (Scale!=1) Verts[i].V*=Scale;
    #ifdef USE_VERTEX_NORMALS
      fscanf(fp, "%f %f %f", &Verts[i].N.x, &Verts[i].N.y, &Verts[i].N.z);
    #endif
  }
  fscanf(fp, "%d", &NumTris);        // READ IN NUM OF TRIS
  Tris = new SharedVertTri[NumTris]; // ALLOC TRIANGLE ARRAY
  for (i=0; i<NumTris; i++)          // READ IN TRIS
  {
    fscanf(fp, "%d %d %d", &Tris[i].iA, &Tris[i].iB, &Tris[i].iC);
  }
  fclose(fp);

  CalcExtents();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void VrmlTri::CalcAllNormals()
{
  CalcFaceNormals();
  #ifdef USE_VERTEX_NORMALS
    CalcVertexNormals();
  #endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void VrmlTri::CalcFaceNormals()
{
  for (int i=0; i<NumTris; i++)
  {
    int iA=Tris[i].iA, iB=Tris[i].iB, iC=Tris[i].iC;
    Tris[i].N = (Verts[iB].V - Verts[iA].V) / (Verts[iC].V - Verts[iA].V);
    Tris[i].N.Normalize();
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#ifdef USE_VERTEX_NORMALS
void VrmlTri::CalcVertexNormals()
{
  float *NumSharing = new float[NumVerts]; // COUNTER FOR EACH VERTEX
  Vec3f AB, AC, Normal;
  int i;
  for (i=0; i<NumVerts; i++)
  {
    NumSharing[i]=0;              // INIT COUNTER
    Verts[i].N.Set(0,0,0);        // INIT VERT NORMAL
  }

  int iA,iB,iC;
  for (i=0; i<NumTris; i++)  // LOAD TRIS INTO TriModel
  {
    iA = Tris[i].iA;
    iB = Tris[i].iB;
    iC = Tris[i].iC;

    if (Normal.x!=0 || Normal.y!=0 || Normal.z!=0)
    {
      Verts[iA].N += Tris[i].N;
      Verts[iB].N += Tris[i].N;
      Verts[iC].N += Tris[i].N;
      NumSharing[iA]++;
      NumSharing[iB]++;
      NumSharing[iC]++;
    }
  }

  for (i=0; i<NumVerts; i++)
  {
    if (NumSharing[i] > 1)
      Verts[i].N /= NumSharing[i];
    if (NumSharing[i] > 0)
      Verts[i].N.Normalize();
  }

  delete[] NumSharing;
}
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void VrmlTri::CalcExtents()
{
  Min = Max = Verts[0].V;
  for (int i=1; i<NumVerts; i++)
  {
    if (Verts[i].V.x < Min.x) Min.x=Verts[i].V.x;
    else if (Verts[i].V.x > Max.x) Max.x=Verts[i].V.x;
    if (Verts[i].V.y < Min.y) Min.y=Verts[i].V.y;
    else if (Verts[i].V.y > Max.y) Max.y=Verts[i].V.y;
    if (Verts[i].V.z < Min.z) Min.z=Verts[i].V.z;
    else if (Verts[i].V.z > Max.z) Max.z=Verts[i].V.z;
  }
}

//----------------------------------------------------------------------------
// Dynamic array handling for vertex and face lists
//----------------------------------------------------------------------------
void VrmlTri::AddVertex(Vec3f V)
{
  if (NumVerts==NumVertsAlloced)  // MUST EXPAND THE ARRAY
  {
    NumVertsAlloced*=2;
    NormVert *NewVerts = new NormVert[NumVertsAlloced];
    for (int i=0; i<NumVerts; i++) NewVerts[i]=Verts[i];
    delete[] Verts;
    Verts=NewVerts;
  }
  Verts[NumVerts].V = V;          // PERFORM A NORMAL ADD
  NumVerts++;
}

void VrmlTri::AddTri(int iA, int iB, int iC)
{
  if (NumTris==NumTrisAlloced)  // MUST EXPAND THE ARRAY
  {
    NumTrisAlloced*=2;
    SharedVertTri *NewTris = new SharedVertTri[NumTrisAlloced];
    for (int i=0; i<NumTris; i++) NewTris[i]=Tris[i];
    delete[] Tris;
    Tris=NewTris;
  }
  Tris[NumTris].iA = iA;        // PERFORM A NORMAL ADD
  Tris[NumTris].iB = iB;
  Tris[NumTris].iC = iC;
  NumTris++;
}

void VrmlTri::CompressArrays()
{
  NumVertsAlloced = NumVerts;
  NumTrisAlloced = NumTris;
  NormVert *NewVerts = new NormVert[NumVertsAlloced];
  SharedVertTri *NewTris = new SharedVertTri[NumTrisAlloced];
  int i;
  for (i=0; i<NumVerts; i++) NewVerts[i]=Verts[i];
  for (i=0; i<NumTris; i++) NewTris[i]=Tris[i];
  delete[] Verts;
  delete[] Tris;
  Verts=NewVerts;
  Tris=NewTris;
}

//----------------------------------------------------------------------------
// PRINTS OUT TO TRIANGLE FILE FORMAT
//----------------------------------------------------------------------------
void VrmlTri::PrintToTri(const char* FileName)
{
  int i;
  FILE* fp = fopen(FileName,"w");
  fprintf(fp, "%f %f %f\n\n", Color.x, Color.y, Color.z);
  fprintf(fp, "%d\n", NumVerts);
  for (i=0; i<NumVerts; i++)
  {
    fprintf(fp, "%f %f %f  ", Verts[i].V.x, Verts[i].V.y, Verts[i].V.z);
    #ifdef USE_VERTEX_NORMALS
      fprintf(fp, "%f %f %f\n", Verts[i].N.x, Verts[i].N.y, Verts[i].N.z);
    #endif
  }
  fprintf(fp, "%d\n", NumTris);
  for (i=0; i<NumTris; i++)
    fprintf(fp, "%d %d %d\n", Tris[i].iA, Tris[i].iB, Tris[i].iC);
  fclose(fp);
}

//----------------------------------------------------------------------------
// PRINTS OUT TO VRML FILE (IN SHARED VERTEX FORMAT W/ VERT NORMALS)
//----------------------------------------------------------------------------
void VrmlTri::PrintToVRML(const char* FileName)
{
  FILE* fp = fopen(FileName,"w");
  fprintf(fp,"#VRML V1.0 ascii\n\n");   // HEADER
  fprintf(fp,"Separator\n{\n");         // START OF MODEL

  PrintObjToVRML(fp);

  fprintf(fp,"}\n");                   // FINISH THE VRML FILE

  fclose(fp);
}

void VrmlTri::PrintObjToVRML(FILE *fp)  // WRITES A SINGLE SHARED VERTEX TRI OBJECT
{
  int i;

  // (1) COLOR INFORMATION
  fprintf(fp, " Material{ambientColor .3 .3 .3\n");
  fprintf(fp, "  diffuseColor %f %f %f}\n", Color.x,Color.y,Color.z);

  // (2) SHARED VERT ARRAY
  fprintf(fp, " Coordinate3{point[\n"); 
  int N_1 = NumVerts-1;
  for (i=0; i<N_1; i++)
    fprintf(fp, "  %f %f %f,\n", Verts[i].V.x, Verts[i].V.y, Verts[i].V.z);
  fprintf(fp, "  %f %f %f\n", Verts[N_1].V.x, Verts[N_1].V.y, Verts[N_1].V.z);
  fprintf(fp, " ]}\n");

  #ifdef USE_VERTEX_NORMALS
  // (3) SHARED VERTEX NORMAL ARRAY
  fprintf(fp, " Normal{vector[\n"); // WRITE OUT SHARED VERT ARRAY
  for (i=0; i<N_1; i++)
    fprintf(fp, "  %f %f %f,\n", Verts[i].N.x, Verts[i].N.y, Verts[i].N.z);
  fprintf(fp, "  %f %f %f\n", Verts[N_1].N.x, Verts[N_1].N.y, Verts[N_1].N.z);
  fprintf(fp, " ]}\n");
  #endif

  // (4) FACE ARRAY, W/ INDICES INTO SHARED VERT LIST
  fprintf(fp, " IndexedFaceSet{coordIndex[\n"); // WRITE OUT TRIANGLES
  N_1 = NumTris-1;
  for (i=0; i<N_1; i++)
    fprintf(fp, "  %d,%d,%d,-1,\n", Tris[i].iA, Tris[i].iB, Tris[i].iC);
  fprintf(fp, "  %d,%d,%d,-1\n", Tris[N_1].iA, Tris[N_1].iB, Tris[N_1].iC);
  fprintf(fp, " ]}\n");
}
