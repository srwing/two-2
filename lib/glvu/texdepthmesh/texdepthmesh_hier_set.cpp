//------------------------------------------------------------------------------
// File : texdepthmesh_hier_set.cpp
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
// texdepthmesh_hier_set.cpp
//============================================================================
#include "texdepthmesh_hier_set.hpp"

extern int ShowSelected;

TDM_HierarchySet::TDM_HierarchySet()
{
  NumTDMs=0;
  NumTDMsAlloced=0;
  TDMs=NULL;
  TDMHs=NULL;
}

TDM_HierarchySet::~TDM_HierarchySet()
{
  for (int i=0; i<NumTDMsAlloced; i++) 
    { delete TDMs[i]; delete TDMHs[i]; }
  delete[] TDMs;
  delete[] TDMHs;
}

void TDM_HierarchySet::Begin()
{
}

void TDM_HierarchySet::AddTDM(Object *Obj, Camera *Cam, int NumCols, int NumRows, float MinDepthRange, int MinDim)
{
  if (NumTDMs==NumTDMsAlloced) DoubleArray();
  TDMs[NumTDMs] = new TexturedDepthMesh(Obj,Cam,NumCols,NumRows);
  TDMHs[NumTDMs] = new TDM_Hierarchy(TDMs[NumTDMs],MinDepthRange,MinDim);
  NumTDMs++;
}

void TDM_HierarchySet::AddTDM(TexturedDepthMesh *TDM, float MinDepthRange, int MinDim)
{
  if (NumTDMs==NumTDMsAlloced) DoubleArray();
  TDMs[NumTDMs]=TDM;
  TDMHs[NumTDMs] = new TDM_Hierarchy(TDMs[NumTDMs],MinDepthRange,MinDim);
  NumTDMs++;
}

void TDM_HierarchySet::End()
{
  PackArray();
}

long TDM_HierarchySet::GetByteConsumption()
{
    long bytes = sizeof( *this );
    bytes += NumTDMsAlloced * sizeof( TexturedDepthMesh* );
    bytes += NumTDMsAlloced * sizeof( TDM_Hierarchy* );
    for ( int i = 0; i < NumTDMsAlloced; i++ )
        {
        bytes += TDMs[i]->GetByteConsumption();
        bytes += TDMHs[i]->GetByteConsumption();
        }
    return bytes;
}

//----------------------------------------------------------------------------
// Display a selected subset of the TDM set
//----------------------------------------------------------------------------
void TDM_HierarchySet::DisplaySet(TDM_Hierarchy* *tdmhs, int numtdms, Camera *Cam)
{
  for (int i=0; i<numtdms; i++)
    tdmhs[i]->DisplayPts_AdaptiveWithVFC(Cam);

  if ( ShowSelected ) {
      DisplaySelectedCameras( tdmhs, numtdms );
  }
}

void TDM_HierarchySet::DisplaySelectedCameras(TDM_Hierarchy* *tdms, int numtdms)
{
    // Display the COP's of the selected cameras in red.
    float modelview[16];
    float projection[16];
    int   ptsize;
    int   i;

    glGetFloatv( GL_MODELVIEW_MATRIX, modelview );
    glGetFloatv( GL_PROJECTION_MATRIX, projection );

    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    glDisable( GL_STENCIL_TEST );
    glDisable( GL_CLIP_PLANE0 );
    glGetIntegerv( GL_POINT_SIZE , &ptsize );
    glPointSize( ptsize*2 );
    glColor3f( 0.898, 0.117, 0.117 );
    glBegin( GL_POINTS );
    for (i=0; i<numtdms; i++)
        {
        Vec3f cop = tdms[i]->RootTDM->Orig;
        glVertex3f( cop.x, cop.y, cop.z );
        }
    glEnd();
    glPointSize( ptsize );

    for (i=0; i<numtdms; i++)
        {
        tdms[i]->RootTDM->Camera::Display();
        }
#if 1
    glBegin( GL_LINES );
    for (i=0; i<numtdms; i++)
        {
        Vec3f cop = tdms[i]->RootTDM->Orig;
        Vec3f head = tdms[i]->RootTDM->Orig + (tdms[0]->RootTDM->Near * tdms[0]->RootTDM->ViewDirOffAxis())
;
        //glColor3f( 1.0, 1.0, 0.0 );
        glColor3f( 0.117, 0.117, 0.898 );
        glVertex3f( cop.x, cop.y, cop.z );
        //glColor3f( 0.1, 0.1, 0.0 );
        glColor3f( 0.117, 0.117, 0.898 );
        glVertex3f( head.x, head.y, head.z );
        }
    glEnd();
#endif
    glEnable( GL_STENCIL_TEST );
    glEnable( GL_CLIP_PLANE0 );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadMatrixf( projection );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadMatrixf( modelview );
}

//----------------------------------------------------------------------------
// Full-set display routines
//----------------------------------------------------------------------------
void TDM_HierarchySet::DisplayAll(Camera *Cam)
{
  DisplaySet(TDMHs,NumTDMs,Cam);
}

void TDM_HierarchySet::DisplayAllCams()
{
  for (int i=0; i<NumTDMs; i++)
    TDMs[i]->Display();
}

//----------------------------------------------------------------------------
// Display the TDM whose origin is closest to the given camera's origin
//----------------------------------------------------------------------------
void TDM_HierarchySet::Display_ClosestPos(Camera *Cam)
{
  if (NumTDMs<1) return;
  TDM_Hierarchy *ClosestTDMH = TDMHs[0];
  float ClosestDist = (TDMHs[0]->RootTDM->Orig - Cam->Orig).LengthSqr();
  float NewDist;
  for (int i=1; i<NumTDMs; i++)
  {
    NewDist = (TDMHs[i]->RootTDM->Orig - Cam->Orig).LengthSqr();
    if (NewDist < ClosestDist)
    { 
      ClosestDist=NewDist;
      ClosestTDMH=TDMHs[i];
    }
  }
  DisplaySet(&ClosestTDMH,1,Cam);
}

//----------------------------------------------------------------------------
// Display the TDM whose viewing direction is most similar to the given
// camera's viewing direction.
//----------------------------------------------------------------------------
void TDM_HierarchySet::Display_ClosestDir(Camera *Cam)
{
  if (NumTDMs<1) return;
  TDM_Hierarchy *ClosestTDMH = TDMHs[0];
  Vec3f CamViewDir = Cam->ViewDirOffAxis();
  float ClosestCosAng = TDMs[0]->ViewDirOffAxis() * CamViewDir;
  float NewCosAng;
  for (int i=1; i<NumTDMs; i++)
  {
    NewCosAng = TDMs[i]->ViewDirOffAxis() * CamViewDir;
    if (NewCosAng > ClosestCosAng)
    { 
      ClosestCosAng=NewCosAng;
      ClosestTDMH=TDMHs[i];
    }
  }
  DisplaySet(&ClosestTDMH,1,Cam);
}

//----------------------------------------------------------------------------
// Display the N TDM's whose origins are closest to the given camera's origin
// (in order from best to worst)
//----------------------------------------------------------------------------
void TDM_HierarchySet::DisplaySet_ClosestPos(Camera *Cam, int NumToUse)
{
  int i;
  if (NumTDMs<1) return;
  if (NumToUse>NumTDMs) NumToUse=NumTDMs;
  
  TDM_Hierarchy **SortedTDMHs = new TDM_Hierarchy*[NumTDMs];
  float *Dist2 = new float[NumTDMs];
  for (i=0; i<NumTDMs; i++)
  {
    SortedTDMHs[i]=TDMHs[i];
    Dist2[i] = (SortedTDMHs[i]->RootTDM->Orig - Cam->Orig).LengthSqr();
  }

  TDM_Hierarchy *t1;
  float t2;
  for (i=0; i<NumToUse; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (Dist2[j] < Dist2[i])
      {
        t1=SortedTDMHs[i]; SortedTDMHs[i]=SortedTDMHs[j]; SortedTDMHs[j]=t1;
        t2=Dist2[i];       Dist2[i]=Dist2[j];             Dist2[j]=t2;
      }
    
  DisplaySet(SortedTDMHs,NumToUse,Cam);

  delete[] SortedTDMHs;
  delete[] Dist2;
}

//----------------------------------------------------------------------------
// Display the N TDMs whose viewing directions are most similar to the given
// camera's viewing direction (in order from best to worst).
//----------------------------------------------------------------------------
void TDM_HierarchySet::DisplaySet_ClosestDir(Camera *Cam, int NumToUse)
{
  int i;
  if (NumTDMs<1) return;
  if (NumToUse>NumTDMs) NumToUse=NumTDMs;
  
  TDM_Hierarchy **SortedTDMHs = new TDM_Hierarchy*[NumTDMs];
  float *CosAng = new float[NumTDMs];
  Vec3f CamViewDir = Cam->ViewDirOffAxis();
  for (i=0; i<NumTDMs; i++)
  {
    SortedTDMHs[i]=TDMHs[i];
    CosAng[i] = TDMs[i]->ViewDirOffAxis() * CamViewDir;
  }

  TDM_Hierarchy *t1;
  float t2;
  for (i=0; i<NumToUse; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (CosAng[j] > CosAng[i])
      {
        t1=SortedTDMHs[i]; SortedTDMHs[i]=SortedTDMHs[j]; SortedTDMHs[j]=t1;
        t2=CosAng[i];      CosAng[i]=CosAng[j];           CosAng[j]=t2;
      }
    
  DisplaySet(SortedTDMHs,NumToUse,Cam);

  delete[] SortedTDMHs;
  delete[] CosAng;
}

//----------------------------------------------------------------------------
// Display the N "best fit" TDMs for the given camera. This version grabs
// the NumToUse*2 closest in direction and then grabs the NumToUse of those
// closest in position.
//----------------------------------------------------------------------------
void TDM_HierarchySet::DisplaySet_ClosestDirPos(Camera *Cam, int NumToUse)
{
  int i;
  if (NumTDMs<1) return;
  if (NumToUse>NumTDMs) NumToUse=NumTDMs;
  
  TDM_Hierarchy **SortedTDMHs = new TDM_Hierarchy*[NumTDMs];
  float *Quality = new float[NumTDMs];
  TDM_Hierarchy *t1;
  float t2;
  int N;

  // FIRST STORE DIRECTION "QUALITY" FOR EACH TDM
  Vec3f CamViewDir = Cam->ViewDirOffAxis();
  for (i=0; i<NumTDMs; i++)
  {
    SortedTDMHs[i]=TDMHs[i];
    Quality[i] = TDMs[i]->ViewDirOffAxis() * CamViewDir;
  }

  // FIND NumToUse*2 NEAREST DIRECTION TDMS BY SORTING
  N = (NumToUse*2>NumTDMs) ? NumTDMs : NumToUse*2;
  for (i=0; i<N; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (Quality[j] > Quality[i])
      {
        t1=SortedTDMHs[i]; SortedTDMHs[i]=SortedTDMHs[j]; SortedTDMHs[j]=t1;
        t2=Quality[i];     Quality[i]=Quality[j];         Quality[j]=t2;
      }
    
  // STORE NumToUse*2 POSITION DISTANCE "QUALITIES"
  N = (NumToUse*2>NumTDMs) ? NumTDMs : NumToUse*2;
  for (i=0; i<N; i++)
    Quality[i] = (SortedTDMHs[i]->RootTDM->Orig - Cam->Orig).LengthSqr();

  // FIND NumToUse NEAREST POSITION TDMS BY SORTING OVER NEAREST DIRECTION SET
  N = (NumToUse>NumTDMs) ? NumTDMs : NumToUse;
  for (i=0; i<N; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (Quality[j] < Quality[i])
      {
        t1=SortedTDMHs[i]; SortedTDMHs[i]=SortedTDMHs[j]; SortedTDMHs[j]=t1;
        t2=Quality[i];     Quality[i]=Quality[j];         Quality[j]=t2;
      }

  DisplaySet(SortedTDMHs,NumToUse,Cam);

  delete[] SortedTDMHs;
  delete[] Quality;
}

//----------------------------------------------------------------------------
// Uses a weighted average of the normalized direction and position qualities
// to determine the N "best fit" TDMs for the current camera view. DirPos
// is a value in [0,1] where 1 means position is used entirely to determine
// the best and 0 means direction is used.
//----------------------------------------------------------------------------
void TDM_HierarchySet::DisplaySet_ClosestDirPos(Camera *Cam, int NumToUse, float DirWeight)
{
  float PosWeight = 1 - DirWeight;
  
  int i;
  if (NumTDMs<1) return;
  if (NumToUse>NumTDMs) NumToUse=NumTDMs;
  
  TDM_Hierarchy **SortedTDMHs = new TDM_Hierarchy*[NumTDMs];
  float *Dist2 = new float[NumTDMs];
  float *CosAng = new float[NumTDMs];
  float *Quality = new float[NumTDMs];
  float MaxDist=0;
  Vec3f CamViewDir = Cam->ViewDirOffAxis();
  for (i=0; i<NumTDMs; i++)
  {
    SortedTDMHs[i]=TDMHs[i];
    Dist2[i] = (SortedTDMHs[i]->RootTDM->Orig - Cam->Orig).LengthSqr();
    if (Dist2[i] > MaxDist) MaxDist=Dist2[i];
    CosAng[i] = TDMs[i]->ViewDirOffAxis() * CamViewDir;
  }

  // CALCULATE WEIGHTED AVERAGE OF NORMALIZED QUALITIES
  for (i=0; i<NumTDMs; i++)
    Quality[i] = PosWeight * (1-(Dist2[i]/MaxDist)) +
                 DirWeight * ((CosAng[i]+1)*0.5);
  
  TDM_Hierarchy *t1;
  float t2;
  for (i=0; i<NumToUse; i++)  // ONLY LOOP OVER WHAT WE NEED
    for (int j=i+1; j<NumTDMs; j++)
      if (Quality[j] > Quality[i])
      {
        t1=SortedTDMHs[i]; SortedTDMHs[i]=SortedTDMHs[j]; SortedTDMHs[j]=t1;
        t2=Quality[i];     Quality[i]=Quality[j];         Quality[j]=t2;
      }
    
  DisplaySet(SortedTDMHs,NumToUse,Cam);

  delete[] SortedTDMHs;
  delete[] Dist2;
  delete[] CosAng;
  delete[] Quality;
}

//----------------------------------------------------------------------------
// DYNAMIC ARRAY ROUTINES
//----------------------------------------------------------------------------
void TDM_HierarchySet::DoubleArray()
{
  if (NumTDMsAlloced==0) NumTDMsAlloced=1; else NumTDMsAlloced*=2;
  TexturedDepthMesh* *NewTDMs = new TexturedDepthMesh*[NumTDMsAlloced];
  TDM_Hierarchy* *NewTDMHs = new TDM_Hierarchy*[NumTDMsAlloced];
  for (int i=0; i<NumTDMs; i++) 
    { NewTDMs[i]=TDMs[i]; NewTDMHs[i]=TDMHs[i]; }
  delete[] TDMs;
  delete[] TDMHs;
  TDMs=NewTDMs;
  TDMHs=NewTDMHs;
}

void TDM_HierarchySet::PackArray()
{
  NumTDMsAlloced = NumTDMs;
  TexturedDepthMesh* *NewTDMs = new TexturedDepthMesh*[NumTDMsAlloced];
  TDM_Hierarchy* *NewTDMHs = new TDM_Hierarchy*[NumTDMsAlloced];
  for (int i=0; i<NumTDMs; i++) 
    { NewTDMs[i]=TDMs[i]; NewTDMHs[i]=TDMHs[i]; }
  delete[] TDMs;
  delete[] TDMHs;
  TDMs=NewTDMs;
  TDMHs=NewTDMHs;
}
