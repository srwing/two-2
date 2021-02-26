//------------------------------------------------------------------------------
// File : bfftri.cpp
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
// bfftri.cpp
//============================================================================

#include <stdlib.h>
#include <string.h>
#include <object.hpp>
#include <ppm.hpp>
#include <textureobj.hpp>
#include <vrmlreadtools.hpp>
#include "bfftri.hpp"
#include "txtr.hpp"

//----------------------------------------------------------------------------
// CONSTRUCTOR : READ IN BFF MODEL
//----------------------------------------------------------------------------
BffTri::BffTri()
{
  NumTris=NumVerts=NumTexs=0;
  NumTrisAlloced=NumVertsAlloced=NumTexsAlloced=1;
  Tris = new VertTri[NumTrisAlloced];
  Verts = new BffNormVert[NumVertsAlloced];
  Texs = new unsigned int[NumTexsAlloced];
}

BffTri::~BffTri()
{
  delete[] Tris;
  delete[] Verts;
}

//----------------------------------------------------------------------------
// ENTIRE BFF FILE IS A SINGLE TRI OBJECT
//----------------------------------------------------------------------------
void BffTri::ReadBFF(const char *FileName, float Scale)
{
  FILE *fp = fopen(FileName,"r");
  if (fp==NULL) { printf("unable to open [%s]!\n",FileName); exit(1); }
  ReadFromBFF(fp,Scale);// READ IN SINGLE BFF OBJECT
  fclose(fp);

  // DO INITIALIZATION
  CompressArrays();
  CalcExtents();
  CalcAllNormals();
}

//----------------------------------------------------------------------------
// READ A SINGLE OBJECT FROM THE ALREADY OPEN BFF FILE
//----------------------------------------------------------------------------
void BffTri::ReadFromBFF(FILE *fp, float Scale)
{
 // SKIP COMMENTS
 SkipComments(fp);

 // READ IN THE TEXTURE LIST
 int nTexs,i,j;
 char texName[256];
 SkipWord(fp,"texture");
 fscanf(fp, "%d", &nTexs);
 for (i=0; i<nTexs; i++) {
    fscanf(fp, "%s", texName);
    AddTexture(texName);
 }

 // READ IN THE POLY LIST
 char buf[256];
 int nVerts, nPolys;
 int TexId, ObjId;
 int vertex[9]; // KNOW HOW TO HANDLE TRIS AND QUADS ONLY
 SkipWord(fp,"polygon");
 fscanf(fp, "%d", &nPolys);
 for (i=0; i<nPolys; i++) {
    readLine(fp, buf);
    sscanf(buf, "%d %d", &nVerts, &TexId);
    unsigned char flags = 0;
    if (strstr(buf,"NORMALS")) flags |= OBJ_NORMALS;
    if (strstr(buf,"RGB"))     flags |= OBJ_COLORS;
    if (strstr(buf,"UV"))      flags |= OBJ_TEXCOORDS;
    sscanf(strstr(buf, "OBJID="), "OBJID=%d", &ObjId);
    for (j=0; j<nVerts; j++) {
       float x,y,z, r,g,b, u,v;
       if (TexId == -1) {
          fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
          vertex[j] = AddVertex(x,y,z,r,g,b,0,0);
       }
       else {
          fscanf(fp, "%f %f %f %f %f %f %f %f", &x, &y, &z, &r, &g, &b, &u, &v);
          vertex[j] = AddVertex(x,y,z,r,g,b,u,v);
       }
    }

    switch (nVerts) {
          case 3:
             AddTri(TexId, ObjId, vertex[0], vertex[1], vertex[2]);
             break;
          case 4:
             AddTri(TexId, ObjId, vertex[0], vertex[1], vertex[2]);
             AddTri(TexId, ObjId, vertex[0], vertex[2], vertex[3]);
             break;
          default:
             fprintf(
                stderr,
                "ERROR: don't know how to handle polygon with %d vertices\n",
                nVerts
             );
             break;
    }
 }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BffTri::CalcAllNormals()
{
  CalcFaceNormals();
  #ifdef USE_VERTEX_NORMALS
    CalcVertexNormals();
  #endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BffTri::CalcFaceNormals()
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
void BffTri::CalcVertexNormals()
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
void BffTri::CalcExtents()
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
int BffTri::AddVertex(
   float x, float y, float z,
   float r, float g, float b,
   float u, float v
)
{
  if (NumVerts==NumVertsAlloced)  // MUST EXPAND THE ARRAY
  {
    NumVertsAlloced*=2;
    BffNormVert *NewVerts = new BffNormVert[NumVertsAlloced];
    for (int i=0; i<NumVerts; i++) NewVerts[i]=Verts[i];
    delete[] Verts;
    Verts=NewVerts;
  }
  Vec3f V(x,y,z);
  Vec3f C(r,g,b);
  Vec2f UV(u,v);
  Verts[NumVerts].V = V;          // PERFORM A NORMAL ADD
  Verts[NumVerts].C = C;          // PERFORM A NORMAL ADD
  Verts[NumVerts].UV = UV;          // PERFORM A NORMAL ADD
  NumVerts++;
  return NumVerts-1;
}

void BffTri::AddTri(int TexId, int ObjId, int iA, int iB, int iC)
{
  if (NumTris==NumTrisAlloced)  // MUST EXPAND THE ARRAY
  {
    NumTrisAlloced*=2;
    VertTri *NewTris = new VertTri[NumTrisAlloced];
    for (int i=0; i<NumTris; i++) NewTris[i]=Tris[i];
    delete[] Tris;
    Tris=NewTris;
  }
  Tris[NumTris].iA = iA;        // PERFORM A NORMAL ADD
  Tris[NumTris].iB = iB;
  Tris[NumTris].iC = iC;
  if (TexId == -1) {
     Tris[NumTris].TexId = -1;
  }
  else {
     Tris[NumTris].TexId = Texs[TexId];
  }
  Tris[NumTris].ObjId = ObjId;
  NumTris++;
}

void BffTri::AddTexture(char *FileName)
{
  if (NumTexs==NumTexsAlloced)  // MUST EXPAND THE ARRAY
  {
    NumTexsAlloced*=2;
    unsigned int *NewTexs = new unsigned int[NumTexsAlloced];
    for (int i=0; i<NumTexs; i++) NewTexs[i]=Texs[i];
    delete[] Texs;
    Texs=NewTexs;
  }

  /* open file and read texture */
  unsigned char *Color = NULL;
  int Width, Height;
  if (strstr(FileName,".ppm") || strstr(FileName,".PPM")) {
     LoadPPM(FileName,Color,Width,Height);
     Texs[NumTexs++] =
        createTexture(Color,Width,Height,GL_REPEAT,GL_LINEAR, 3);
     delete[] Color;
  }
  else {
     int n_components;
     if (strstr(FileName,".rgb") || strstr(FileName,".RGB"))
        readRGBfile(FileName, &Color, &Width, &Height, &n_components);
     Texs[NumTexs++] =
        createTexture(Color,Width,Height,GL_REPEAT,GL_LINEAR, 4);
     free(Color);
  }

}

void BffTri::CompressArrays()
{
  NumVertsAlloced = NumVerts;
  NumTrisAlloced = NumTris;
  BffNormVert *NewVerts = new BffNormVert[NumVertsAlloced];
  VertTri *NewTris = new VertTri[NumTrisAlloced];
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
void BffTri::PrintToTri(const char* FileName)
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
// PRINTS OUT TO BFF FILE (IN SHARED VERTEX FORMAT W/ VERT NORMALS)
//----------------------------------------------------------------------------
void BffTri::PrintToBFF(const char* FileName)
{
  FILE* fp = fopen(FileName,"w");
  fprintf(fp,"#BFF V1.0 ascii\n\n");   // HEADER
  fprintf(fp,"Separator\n{\n");         // START OF MODEL

  PrintObjToBFF(fp);

  fprintf(fp,"}\n");                   // FINISH THE BFF FILE

  fclose(fp);
}

void BffTri::PrintObjToBFF(FILE *fp)  // WRITES A SINGLE SHARED VERTEX TRI OBJECT
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
