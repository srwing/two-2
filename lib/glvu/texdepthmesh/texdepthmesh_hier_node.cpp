//------------------------------------------------------------------------------
// File : texdepthmesh_hier_node.cpp
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
// texdepthmesh_hier_node.cpp
//============================================================================

#include "texdepthmesh_hier_node.hpp"
#include "plane.hpp"

TDM_node::TDM_node(TexturedDepthMesh *roottdm, int l, int r, int b, int t)
{
  RootTDM=roottdm;
  L=l; R=r; B=b; T=t;
  for (int i=0; i<4; i++) Child[i]=NULL;
  CalcMinMaxDepths();
}

TDM_node::~TDM_node()
{
  for (int i=0; i<4; i++)
    delete Child[i];
}

int TDM_node::Subdivide(float MinDepthRange, int MinDim)
{
  if (MaxDepth-MinDepth <= MinDepthRange) return(0);
  if (nRows()<=MinDim || nCols()<=MinDim) return(0);
  int xSplit=(R+L)/2, ySplit=(B+T)/2;
  Child[0] = new TDM_node(RootTDM, L,xSplit,   ySplit+1,T);  // TL
  Child[1] = new TDM_node(RootTDM, xSplit+1,R, ySplit+1,T);  // TR
  Child[2] = new TDM_node(RootTDM, L,xSplit,   B,ySplit);    // BL
  Child[3] = new TDM_node(RootTDM, xSplit+1,R, B,ySplit);    // BR
  return(1);
}

void TDM_node::CalcMinMaxDepths()
{
  int iD, iD_X;
  int nCOLS=RootTDM->nCols;
  float *Depth = RootTDM->Depth;

  MinDepth=1;
  MaxDepth=0;
  for (int y=B; y<=T; y++)
  {
    iD = y*nCOLS;
    for (int x=L; x<=R; x++)
    {
      iD_X = iD+x;
      if (Depth[iD_X]>0.0001f || Depth[iD_X]<0.9999f)
      {
        if (Depth[iD_X]<MinDepth) MinDepth=Depth[iD_X];
        else if (Depth[iD_X]>MaxDepth) MaxDepth=Depth[iD_X];
      }      
    }
  }
  MinDepth = LinearizeZ(MinDepth,RootTDM->Near,RootTDM->Far);
  MaxDepth = LinearizeZ(MaxDepth,RootTDM->Near,RootTDM->Far);
}

// CALCS L,R,B,T AT THE ROOT TDM CAMERA NEAR PLANE
void TDM_node::WinBoundsLRBT(float Win[4])
{
  float TDMwidth = RootTDM->wR - RootTDM->wL;
  float TDMheight = RootTDM->wT - RootTDM->wB;
  float TDMnCols_1 = RootTDM->nCols + 1;
  float TDMnRows_1 = RootTDM->nRows + 1;
  Win[0] = RootTDM->wL + (L/TDMnCols_1) * TDMwidth;
  Win[1] = RootTDM->wL + ((R+1)/TDMnCols_1) * TDMwidth;
  Win[2] = RootTDM->wB + (B/TDMnRows_1) * TDMheight;
  Win[3] = RootTDM->wB + ((T+1)/TDMnRows_1) * TDMheight;
}

Vec3f TDM_node::CalcRefPt()  // IN RootTDM CAMERA COORDS
{
  float Win[4];
  WinBoundsLRBT(Win);
  float x=(Win[0]+Win[1])*0.5, y=(Win[2]+Win[3])*0.5;  // MIDPOINT ON VIEWPLANE WINDOW
  float Scale = MinDepth / RootTDM->Near;               // CALC POINT PROJECTED ONTO THE AVDEPTH PLANE
  Vec3f RefPt(x*Scale,y*Scale,-MinDepth);
  return(RefPt);
}

void TDM_node::CalcStepSizes(Vec3f NewCOP, float *Xstep, float *Ystep)
{
  // 1) CALCULATE THE REFERENCE POINT
  Vec3f RefPt = CalcRefPt();

  // 2) XFORM NEW CAMERA VIEW CENTER-OF-PROJECTION INTO OLD VIEW
  NewCOP = RootTDM->WorldToCam(NewCOP);

  // 3) CREATE VECTORS: OLD COP TO REF PT, NEW COP TO REF PT
  Vec3f Old2R = RefPt;
  Vec3f New2R = RefPt - NewCOP;

  // 4) CALCULATE NEW AND OLD DISTANCES FROM REF PT
  float OldDist = Old2R.Length();
  float NewDist = New2R.Length();

float MinDist = MinDepth-MinDepth; // CLAMP NEW DIST TO A MIN DIST
float t = MaxDepth-MinDepth;
if (t<MinDist) MinDist=t;
if (NewDist<MinDist) NewDist=MinDist;

  // 5) CALCULATE DISTANCE AREA-SCALING
  float sqrtDistAreaScale = OldDist/NewDist;  // SQRT OF DIST AREA SCALE (AVOIDS SQRT)

float XangScale=1, YangScale=1;

if (NewDist>MinDist)
{
  // 6) PROJECT VECTORS ONTO XZ AND YZ PLANES AND NORMALIZE
  float l;
  float xzO2R[] = { Old2R.x, Old2R.z };
  float xzN2R[] = { New2R.x, New2R.z };
  l = (float)sqrt(xzO2R[0]*xzO2R[0] + xzO2R[1]*xzO2R[1]);
  xzO2R[0]/=l; xzO2R[1]/=l;
  l = (float)sqrt(xzN2R[0]*xzN2R[0] + xzN2R[1]*xzN2R[1]);
  xzN2R[0]/=l; xzN2R[1]/=l;
  float yzO2R[] = { Old2R.y, Old2R.z };
  float yzN2R[] = { New2R.y, New2R.z };
  l = (float)sqrt(yzO2R[0]*yzO2R[0] + yzO2R[1]*yzO2R[1]);
  yzO2R[0]/=l; yzO2R[1]/=l;
  l = (float)sqrt(yzN2R[0]*yzN2R[0] + yzN2R[1]*yzN2R[1]);
  yzN2R[0]/=l; yzN2R[1]/=l;

  // 7) CALCULATE INDEPENDENT XY ANGULAR AREA-SCALING; CLAMP TO [0,1]
  XangScale = xzO2R[0]*xzN2R[0] + xzO2R[1]*xzN2R[1];  // COS ANGLE BETWEEN Old2R AND New2R IN XZ-PLANE
  YangScale = yzO2R[0]*yzN2R[0] + yzO2R[1]*yzN2R[1];  // COS ANGLE BETWEEN Old2R AND New2R IN YZ-PLANE
  float XangRes = 1 / (float)nCols();
  float YangRes = 1 / (float)nRows(); 
  if (XangScale<XangRes) XangScale=XangRes;
  if (YangScale<YangRes) YangScale=YangRes;
}

  // 8) CALCULATE STEPSIZE SCALE BASED ON WINDOW RESOLUTION (TDM AND WINDOW RES DIFFER)
  float WW = glutGet((GLenum)GLUT_WINDOW_WIDTH);
  float WH = glutGet((GLenum)GLUT_WINDOW_HEIGHT);
  float XwinScale = RootTDM->nRows / WW;
  float YwinScale = RootTDM->nCols / WH;
//XwinScale=YwinScale=1;

  // 9) CALCULATE STEPSIZES FOR X AND Y BASED ON AREA SCALING FACTORS
  *Xstep = XwinScale / (sqrtDistAreaScale*XangScale);
  *Ystep = YwinScale / (sqrtDistAreaScale*YangScale);

  // "FUDGE FACTOR" COMPENSATION
  *Xstep *= 0.9f;
  *Ystep *= 0.9f;
//printf("sqrtDistAreaScale=%f\n",1/sqrtDistAreaScale);
//printf("XangScale=%f, YangScale=%f\n",1/XangScale,1/YangScale);
//printf("Xstep=%f, Ystep=%f\n",*Xstep,*Ystep);
}

void TDM_node::DisplayAsPtsWithColor(float Xstep, float Ystep)
{
 if (Xstep>nCols() || Ystep>nRows()) return;

 float *Depth = RootTDM->Depth;
 unsigned char *Color = RootTDM->Color;

  glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
  GLfloat M[16];
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixf( RootTDM->GetXform_Screen2Obj(M,RootTDM->nCols,RootTDM->nRows) );

  glTranslatef(0.5,0.5,0);
  glBegin(GL_POINTS);

    float Xrange = (float)(R-L+1)-Xstep;  // FILL THE ENTIRE RANGE EVENLY
    float Yrange = (float)(T-B+1)-Ystep;
    int NumInX = (int)(Xrange / Xstep);
    int NumInY = (int)(Yrange / Ystep);
    Xstep = Xrange / (float)NumInX;
    Ystep = Yrange / (float)NumInY;

    int iD, iC;
    int nCOLS=RootTDM->nCols;
    int x,y;
    float X,Y;
    for (y=0, Y=B; y<=NumInY; y++, Y+=Ystep)
    {
      iD = (int)Y * nCOLS;
      iC = iD*3;
      for (x=0, X=L; x<=NumInX; x++, X+=Xstep)
      {
        glColor3ubv(&(Color[iC+((int)X*3)]));
        glVertex3f(X,Y,Depth[iD+(int)X]); 
      }
    }

  glEnd();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();  // LIGHTING
}

// WITH POINT ENLARGEMENT FOR MAGNIFICATION
void TDM_node::DisplayAsPtsWithColor_WithPtMag(float Xstep, float Ystep)
{
  // INCREASE POINTSIZE FOR MAGNIFICATION
  float PointSize=1;
  float MinStep = (Xstep<Ystep)?Xstep:Ystep;
  if (MinStep<1)
  {
    if (Xstep<1) Xstep=1;
    if (Ystep<1) Ystep=1;
    PointSize=1/MinStep;
    PointSize *= 1.3f; // "FUDGE FACTOR" COMPENSATION
  }

  float OldPtSize;
  glGetFloatv(GL_POINT_SIZE,&OldPtSize);
  glPointSize( PointSize );
    DisplayAsPtsWithColor(Xstep,Ystep);
  glPointSize(OldPtSize);
}

void TDM_node::DisplayAsPtsWithColor_Adaptive(Camera *NewView, int UsePtMag)
{
  float Xstep, Ystep;
  CalcStepSizes(NewView->Orig, &Xstep, &Ystep);
  if (UsePtMag)
    DisplayAsPtsWithColor_WithPtMag(Xstep,Ystep);
  else
    DisplayAsPtsWithColor(Xstep,Ystep);
}

//----------------------------------------------------------------------------
// 0=RTN,1=LTN,2=LBN,3=RBN,4=RTF,5=LTF,6=LBF,7=RBF
// (Left,Right, Bottom,Top, Near,Far)
// In order, near pts counter-clockwise starting with right-top-near (RTN) pt
// and then far pts ccw starting with right-top-far (RTF) pt
//----------------------------------------------------------------------------
void TDM_node::CalcVerts(Vec3f V[8])
{
  float nW[4];
  WinBoundsLRBT(nW);
  float fW[4] = { nW[0],nW[1],nW[2],nW[3] };

  float Near = RootTDM->Near;

  // WINDOW EXTENTS ARE DEFINED ON THE NEAR PLANE, CALC NEAR PTS (IN CAM COORDS)
  float NearZ=-MinDepth;
  float S=MinDepth/Near;
  nW[0]*=S; nW[1]*=S; nW[2]*=S; nW[3]*=S;
  V[0].Set(nW[1],nW[3],NearZ);
  V[1].Set(nW[0],nW[3],NearZ);
  V[2].Set(nW[0],nW[2],NearZ);
  V[3].Set(nW[1],nW[2],NearZ);

  // CALC FAR PTS (IN CAM COORDS)
  float FarZ=-MaxDepth;
  S=MaxDepth/Near;
  fW[0]*=S; fW[1]*=S; fW[2]*=S; fW[3]*=S;
  V[4].Set(fW[1],fW[3],FarZ);
  V[5].Set(fW[0],fW[3],FarZ);
  V[6].Set(fW[0],fW[2],FarZ);
  V[7].Set(fW[1],fW[2],FarZ);

  // XFORM FRUSTUM IN CAM COORDS TO WORLD SPACE
  for (int i=0; i<8; i++)
    V[i] = RootTDM->CamToWorld(V[i]); 
}

void TDM_node::CalcPlanes(Vec3f V[8], float P[][4])  // GIVEN THE VERTICES
{
  PlaneEquation(P[0], &(V[2].x),&(V[5].x),&(V[6].x)); // LEFT
  PlaneEquation(P[1], &(V[0].x),&(V[7].x),&(V[4].x)); // RIGHT
  PlaneEquation(P[2], &(V[3].x),&(V[6].x),&(V[7].x)); // BOTTOM
  PlaneEquation(P[3], &(V[1].x),&(V[4].x),&(V[5].x)); // TOP
  PlaneEquation(P[4], &(V[1].x),&(V[2].x),&(V[0].x)); // NEAR
  PlaneEquation(P[5], &(V[4].x),&(V[6].x),&(V[5].x)); // FAR
}

void TDM_node::DisplayNode()
{
  // CALC EIGHT CORNERS OF FRUSTUM (NEAR PTS AND FAR PTS)
  Vec3f V[8];
  CalcVerts(V);

  glPushAttrib(GL_LIGHTING_BIT);
  glDisable(GL_LIGHTING);

  glPushAttrib(GL_POINT_BIT);
  glPointSize(5);
  glColor3f(1,0,0);
  glBegin(GL_POINTS);
    Vec3f RefPt = CalcRefPt();
    RefPt = RootTDM->CamToWorld(RefPt);
    glVertex3fv(&(RefPt.x));
  glEnd();

  DisplayBox(V,0,1,0);

  glPopAttrib();
  glPopAttrib();
}

void TDM_node::DisplayBox(Vec3f V[8], float r, float g, float b)
{
  glColor3f(r,g,b);
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(1.0);

  // DRAW THE FRUSTUM IN WIREFRAME
  glBegin(GL_LINE_LOOP);  // TOP FACE
    glVertex3fv(&(V[4].x)); glVertex3fv(&(V[5].x)); 
    glVertex3fv(&(V[1].x)); glVertex3fv(&(V[0].x));
  glEnd();
  glBegin(GL_LINE_LOOP);  // BOTTOM FACE
    glVertex3fv(&(V[3].x)); glVertex3fv(&(V[2].x));
    glVertex3fv(&(V[6].x)); glVertex3fv(&(V[7].x)); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // LEFT FACE
    glVertex3fv(&(V[1].x)); glVertex3fv(&(V[5].x));
    glVertex3fv(&(V[6].x)); glVertex3fv(&(V[2].x)); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // RIGHT FACE
    glVertex3fv(&(V[0].x)); glVertex3fv(&(V[3].x));
    glVertex3fv(&(V[7].x)); glVertex3fv(&(V[4].x)); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // NEAR FACE
    glVertex3fv(&(V[1].x)); glVertex3fv(&(V[2].x));
    glVertex3fv(&(V[3].x)); glVertex3fv(&(V[0].x)); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // FAR FACE
    glVertex3fv(&(V[4].x)); glVertex3fv(&(V[7].x));
    glVertex3fv(&(V[6].x)); glVertex3fv(&(V[5].x)); 
  glEnd();

  glPopAttrib();
}
