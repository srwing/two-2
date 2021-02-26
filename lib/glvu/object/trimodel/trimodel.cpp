//------------------------------------------------------------------------------
// File : trimodel.cpp
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
// trimodel.cpp
//============================================================================

#include <stdlib.h>
#include <time.h>
#include "trimodel.hpp"

clock_t Time;

TriModel::TriModel()
{
  NumTris=0;
  NumTrisAlloced=1;
  Tris = new Tri[NumTrisAlloced];
}

TriModel::TriModel(const char *FileName, int IsBinaryFile, float Scale)
{
  if (IsBinaryFile)
    ReadBinary(FileName);
  else
    Read(FileName,Scale);
}

TriModel::TriModel(const char *FileName, unsigned char r, unsigned char g, unsigned char b, float Scale)
{
  Read(FileName,r,g,b,Scale);
}

TriModel::~TriModel()
{
  if (Tris) delete[] Tris;
}

void TriModel::Read(const char *FileName, float Scale)
{
  unsigned int HexColor, RedMask=0xff0000, GreenMask=0xff00, BlueMask=0xff;

  FILE* fp = fopen(FileName,"r");
  if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }

  fscanf(fp, "%d", &NumTris);   // READ IN # OF TRIANGLES
  printf("Reading in %s (%d triangles). . .\n", FileName, NumTris);
  Tris = new Tri[NumTris];      // ALLOCATE THE TRIANGLES ARRAY
  for (int i=0; i<NumTris; i++) // READ IN THE TRIS
  {
    fscanf(fp, "%f %f %f", &Tris[i].A.x, &Tris[i].A.y, &Tris[i].A.z);
    fscanf(fp, "%f %f %f", &Tris[i].B.x, &Tris[i].B.y, &Tris[i].B.z);
    fscanf(fp, "%f %f %f", &Tris[i].C.x, &Tris[i].C.y, &Tris[i].C.z);

    fscanf(fp,"%x",&HexColor);  // READ IN "HEX" COLOR
    Tris[i].Color[0] = (unsigned char)((HexColor & RedMask) >> 16);
    Tris[i].Color[1] = (unsigned char)((HexColor & GreenMask) >> 8);
    Tris[i].Color[2] = (unsigned char)(HexColor & BlueMask);

    // SCALE VERTICES AND CALCULATE NORMALS (COULD CALL CalcNormals)
    if (Scale!=1) { Tris[i].A*=Scale; Tris[i].B*=Scale; Tris[i].C*=Scale; }
    Tris[i].N = (Tris[i].B-Tris[i].A) / (Tris[i].C-Tris[i].A);
    Tris[i].N.Normalize();
  }
  fclose(fp);

  UpdateMinMax(); // CALC MIN AND MAX EXTENTS OF THE MODEL
}

void TriModel::Read(const char *FileName, unsigned char r, unsigned char g,  unsigned char b, float Scale)
{
  FILE* fp = fopen(FileName,"r");
  if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }

  fscanf(fp, "%d", &NumTris);   // READ IN # OF TRIANGLES
  printf("Reading in %s (%d triangles). . .\n", FileName, NumTris);
  Tris = new Tri[NumTris];      // ALLOCATE THE TRIANGLES ARRAY
  for (int i=0; i<NumTris; i++) // READ IN THE TRIS
  {
    fscanf(fp, "%f %f %f", &Tris[i].A.x, &Tris[i].A.y, &Tris[i].A.z);
    fscanf(fp, "%f %f %f", &Tris[i].B.x, &Tris[i].B.y, &Tris[i].B.z);
    fscanf(fp, "%f %f %f", &Tris[i].C.x, &Tris[i].C.y, &Tris[i].C.z);

    Tris[i].Color[0] = r;
    Tris[i].Color[1] = g;
    Tris[i].Color[2] = b;

    // SCALE VERTICES AND CALCULATE NORMALS (COULD CALL CalcNormals)
    if (Scale!=1) { Tris[i].A*=Scale; Tris[i].B*=Scale; Tris[i].C*=Scale; }
    Tris[i].N = (Tris[i].B-Tris[i].A) / (Tris[i].C-Tris[i].A);
    Tris[i].N.Normalize();
  }
  fclose(fp);

  UpdateMinMax(); // CALC MIN AND MAX EXTENTS OF THE MODEL
}

void TriModel::Write(const char *FileName)
{
  printf("Writing to %s (%d triangles). . .\n", FileName, NumTris);

  unsigned int HexColor;

  FILE* fp = fopen(FileName,"w");
  if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }

  fprintf(fp, "%d\n", NumTris);
  for (int i=0; i<NumTris; i++)
  {
    fprintf(fp, "%f %f %f ", Tris[i].A.x, Tris[i].A.y, Tris[i].A.z);
    fprintf(fp, "%f %f %f ", Tris[i].B.x, Tris[i].B.y, Tris[i].B.z);
    fprintf(fp, "%f %f %f ", Tris[i].C.x, Tris[i].C.y, Tris[i].C.z);

    HexColor = (((unsigned int)Tris[i].Color[0])<<16) +
               (((unsigned int)Tris[i].Color[1])<<8) +
               ((unsigned int)Tris[i].Color[2]);
    fprintf(fp,"%x\n",HexColor);
  }
  fclose(fp);
}

void TriModel::ReadBinary(const char *FileName)
{
  FILE* fp = fopen(FileName,"rb");
  if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }
  fread(&NumTris,sizeof(int),1,fp);
  printf("Reading in %s (%d triangles). . .\n", FileName, NumTris);
  Tris = new Tri[NumTris];
  fread(Tris,sizeof(Tri),NumTris,fp);
  fclose(fp);
  UpdateMinMax(); // CALC MIN AND MAX EXTENTS OF THE MODEL
}

void TriModel::WriteBinary(const char *FileName)
{
  printf("Writing to %s (%d triangles). . .\n", FileName, NumTris);
  FILE* fp = fopen(FileName,"wb");
  if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }
  fwrite(&NumTris,sizeof(int),1,fp);
  fwrite(Tris,sizeof(Tri),NumTris,fp);
  fclose(fp);
}

void TriModel::BeginModel()
{
  printf("creating triangle model... ");
  Time = clock();
}

void TriModel::AddTri(Vec3f A, Vec3f B, Vec3f C, Vec3f Color)
{
  if (NumTris==NumTrisAlloced)  // MUST EXPAND THE ARRAY
  {
    NumTrisAlloced*=2;
    Tri *NewTris = new Tri[NumTrisAlloced];
    for (int i=0; i<NumTris; i++) NewTris[i]=Tris[i];
    delete[] Tris;
    Tris=NewTris;
  }
  Tris[NumTris].A = A;        // PERFORM A NORMAL ADD
  Tris[NumTris].B = B;
  Tris[NumTris].C = C;
  Tris[NumTris].Color[0] = (unsigned char)(Color.x * 255);
  Tris[NumTris].Color[1] = (unsigned char)(Color.y * 255);
  Tris[NumTris].Color[2] = (unsigned char)(Color.z * 255);
  NumTris++;
}

void TriModel::EndModel()
{
  CompressArrays();
  UpdateMinMax();
  CalcNormals();

  printf("%d tris... %.2f seconds\n", NumTris, (clock()-Time)/(float)CLOCKS_PER_SEC);
}

void TriModel::CompressArrays()
{
  NumTrisAlloced = NumTris;
  Tri *NewTris = new Tri[NumTrisAlloced];
  for (int i=0; i<NumTris; i++) NewTris[i]=Tris[i];
  delete[] Tris;
  Tris=NewTris;
}

void TriModel::UpdateMinMax()
{
  Min = Max = Tris[0].A;
  for (int i=0; i<NumTris; i++)
  {
    Tris[i].A.UpdateMinMax(Min,Max);
    Tris[i].B.UpdateMinMax(Min,Max);
    Tris[i].C.UpdateMinMax(Min,Max);
  }
}

void TriModel::CalcNormals()
{
  for (int i=0; i<NumTris; i++)
  {
    Tris[i].N = (Tris[i].B-Tris[i].A) / (Tris[i].C-Tris[i].A);
    Tris[i].N.Normalize();
  }
}

void TriModel::ReverseNormals()
{
  for (int i=0; i<NumTris; i++)
  {
    Tris[i].N = -Tris[i].N;
  }
}

// MUST STUFF A FLAT ARRAY OF FLOATS WITH TRIANGLE VERTICES
void TriModel::GetTris(float* &tris, int &numTris)
{
  // ALLOC ARRAY ONLY IF tris==NULL; OTHERWISE, MUST HAVE BEEN PREALLOCED
  numTris = NumTris;
  if (tris==NULL) tris = new float[NumTris*9];  // 9 FLOATS/TRI

  // STUFF ARRAY WITH TRIANGLE VERTEX COMPONENTS
  int j=0;
  float *T = tris;
  for (int i=0; i<NumTris; i++)
  {
    T[j] = Tris[i].A.x;   j++;
    T[j] = Tris[i].A.y;   j++; 
    T[j] = Tris[i].A.z;   j++; 
    T[j] = Tris[i].B.x;   j++;
    T[j] = Tris[i].B.y;   j++; 
    T[j] = Tris[i].B.z;   j++; 
    T[j] = Tris[i].C.x;   j++;
    T[j] = Tris[i].C.y;   j++; 
    T[j] = Tris[i].C.z;   j++; 
  }
}

// double version
// MUST STUFF A FLAT ARRAY OF FLOATS WITH TRIANGLE VERTICES
void TriModel::GetTris(double* &tris, int &numTris)
{
  // ALLOC ARRAY ONLY IF tris==NULL; OTHERWISE, MUST HAVE BEEN PREALLOCED
  numTris = NumTris;
  if (tris==NULL) tris = new double[NumTris*9];  // 9 DOUBLES/TRI

  // STUFF ARRAY WITH TRIANGLE VERTEX COMPONENTS
  int j=0;
  double *T = tris;
  for (int i=0; i<NumTris; i++)
  {
    T[j] = (double)Tris[i].A.x;   j++;
    T[j] = (double)Tris[i].A.y;   j++; 
    T[j] = (double)Tris[i].A.z;   j++; 
    T[j] = (double)Tris[i].B.x;   j++;
    T[j] = (double)Tris[i].B.y;   j++; 
    T[j] = (double)Tris[i].B.z;   j++; 
    T[j] = (double)Tris[i].C.x;   j++;
    T[j] = (double)Tris[i].C.y;   j++; 
    T[j] = (double)Tris[i].C.z;   j++; 
  }
}

void TriModel::GetTriData(float* &data, int &numTris, int &numValuesPerTri,
                          unsigned int Attribs)
{
  // ALLOC ARRAY ONLY IF data==NULL; OTHERWISE, MUST HAVE BEEN PREALLOCED
  numTris = NumTris;
  
  switch(Attribs) {
  case OBJ_NORMALS:
    {
      numValuesPerTri = 3;
      if (data == NULL)
        data = new float[NumTris*3];  // 3 FLOATS/NORMAL
      int j = 0;
      for (int i=0; i < NumTris; i++) {
        data[j] = Tris[i].N.x;  j++;
        data[j] = Tris[i].N.y;  j++;
        data[j] = Tris[i].N.z;  j++;
      }
      break;
    }
  case OBJ_COLORS:
    {
      numValuesPerTri = 3;
      if (data == NULL) 
        data = new float[NumTris*3];  // 3 FLOATS/COLOR
      int j = 0;
      for (int i=0; i < NumTris; i++) {
        data[j] = Tris[i].Color[0]; j++;
        data[j] = Tris[i].Color[1]; j++;
        data[j] = Tris[i].Color[2]; j++;
      }
      break;
    }
  default:
    numValuesPerTri = 0;
    numTris = 0;  
    break;
  }
}

int TriModel::GetNumTris()
{
  return( NumTris );
}
