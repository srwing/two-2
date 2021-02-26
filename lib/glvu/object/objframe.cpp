//------------------------------------------------------------------------------
// File : objframe.cpp
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

//==========================================================================
// objframe.cpp
//==========================================================================

#include <mat44.hpp>
#include "objframe.hpp"


ObjectFrame::ObjectFrame(Object *ObjPtr)
{
  Obj = ObjPtr;
  X.Set(1,0,0);
  Y.Set(0,1,0);
  Z.Set(0,0,1);
  O.Set(0,0,0);
  Scale=1;
  DisplayListID=0;
  numChildren = 0;
  parent = NULL;
  child = NULL;
  rightSibling = NULL;
  leftSibling = NULL;
  mass = 0.0;
  units = 1.0;
  velocity = Vec3f(0, 0, 0);
  totalForce = Vec3f(0, 0, 0);
  moving = false;
}

void ObjectFrame::TranslateObj(Vec3f T)
{
  O += T;
}

void ObjectFrame::ScaleObj(float scale)
{
  Scale *= scale;
}

void ObjectFrame::RotateObj(float Ang, Vec3f Axis)  // Ang IN DEGREES, ABOUT ORIGIN
{
  float M[16];
  GetRotationMatrix(M,Ang,Axis);          // CREATE ROTATION MATRIX
  X.Set( M[0]*X.x + M[4]*X.y + M[8]*X.z,  // APPLY MATRIX XFORM TO AXES
         M[1]*X.x + M[5]*X.y + M[9]*X.z,
         M[2]*X.x + M[6]*X.y + M[10]*X.z );
  Y.Set( M[0]*Y.x + M[4]*Y.y + M[8]*Y.z,
         M[1]*Y.x + M[5]*Y.y + M[9]*Y.z,
         M[2]*Y.x + M[6]*Y.y + M[10]*Y.z );
  Z.Set( M[0]*Z.x + M[4]*Z.y + M[8]*Z.z,
         M[1]*Z.x + M[5]*Z.y + M[9]*Z.z,
         M[2]*Z.x + M[6]*Z.y + M[10]*Z.z );
}

void ObjectFrame::RotateObjAboutMinMaxCntr(float Ang, Vec3f Axis)  
{
  RotateObj(Ang,Axis,worldMinMaxCntr());
}

void ObjectFrame::ScaleObj(float scale, Vec3f Pt)
{
  TranslateObj(-Pt);
  ScaleObj(scale);
  TranslateObj(Pt);
}

// Ang IN DEGREES, ROTATES ABOUT AXIS THROUGH Pt DEFINED IN LOCAL FRAME
void ObjectFrame::RotateObj(float Ang, Vec3f Axis, Vec3f Pt)  
{
  TranslateObj(-Pt);

  float M[16];
  GetRotationMatrix(M,Ang,Axis);          // CREATE ROTATION MATRIX
  X.Set( M[0]*X.x + M[4]*X.y + M[8]*X.z,  // APPLY MATRIX XFORM TO AXES
         M[1]*X.x + M[5]*X.y + M[9]*X.z,
         M[2]*X.x + M[6]*X.y + M[10]*X.z );
  Y.Set( M[0]*Y.x + M[4]*Y.y + M[8]*Y.z,
         M[1]*Y.x + M[5]*Y.y + M[9]*Y.z,
         M[2]*Y.x + M[6]*Y.y + M[10]*Y.z );
  Z.Set( M[0]*Z.x + M[4]*Z.y + M[8]*Z.z,
         M[1]*Z.x + M[5]*Z.y + M[9]*Z.z,
         M[2]*Z.x + M[6]*Z.y + M[10]*Z.z );
  O.Set( M[0]*O.x + M[4]*O.y + M[8]*O.z,
         M[1]*O.x + M[5]*O.y + M[9]*O.z,
         M[2]*O.x + M[6]*O.y + M[10]*O.z );

  TranslateObj(Pt);
}

//----------------------------------------------------------------------------
// Set the object frame coordinate frame to be the identity: object-space
// "equals" world space.
//----------------------------------------------------------------------------
void ObjectFrame::LoadIdentity()
{
  X.Set(1,0,0);
  Y.Set(0,1,0);
  Z.Set(0,0,1);
  O.Set(0,0,0);
  Scale=1;
}

//----------------------------------------------------------------------------
// Returns the object-space or world-space Min-Max bounding box center.
//----------------------------------------------------------------------------
Vec3f ObjectFrame::objMinMaxCntr()
{
  if (Obj == NULL) return Vec3f(0, 0, 0);

  Vec3f objCentr = (Obj->Min + Obj->Max) * 0.5f;
  return( objCentr );
}

Vec3f ObjectFrame::worldMinMaxCntr()
{
  Vec3f objCentr = objMinMaxCntr();
  Vec3f worldCentr = X*objCentr.x + Y*objCentr.y + Z*objCentr.z + O;
  if (parent != NULL) {
    Mat44f M;
    parent->GetObj2WorldXform(M);
    worldCentr = M*worldCentr;
  }
  return( worldCentr );
}

//----------------------------------------------------------------------------
// Applies an OpenGL style postmult/col vector xform to the coordinate frame.
// Only rotates, translates, and uniform scales are allowed.
//----------------------------------------------------------------------------
void ObjectFrame::Xform(float M[16])
{
  X.Set( X.x*M[0] + X.y*M[4] + X.z*M[8],
         X.x*M[1] + X.y*M[5] + X.z*M[9],
         X.x*M[2] + X.y*M[6] + X.z*M[10] );
  Y.Set( Y.x*M[0] + Y.y*M[4] + Y.z*M[8],
         Y.x*M[1] + Y.y*M[5] + Y.z*M[9],
         Y.x*M[2] + Y.y*M[6] + Y.z*M[10] );
  Z.Set( Z.x*M[0] + Z.y*M[4] + Z.z*M[8],
         Z.x*M[1] + Z.y*M[5] + Z.z*M[9],
         Z.x*M[2] + Z.y*M[6] + Z.z*M[10] );
  O.Set( O.x*M[0] + O.y*M[4] + O.z*M[8] + M[12],
         O.x*M[1] + O.y*M[5] + O.z*M[9] + M[13],
         O.x*M[2] + O.y*M[6] + O.z*M[10] + M[14] );

  // MUST RENORMALIZE AXES TO FIND THE UNIFORM SCALE
  float scale = X.Length();
  X /= scale;
  Y /= scale;
  Z /= scale;

  // SCALE THE WINDOW EXTENTS AND THE NEAR/FAR PLANES
  Scale=scale;
};

//--------------------------------------------------------------------------
// Returns the 4x4 matrix (16 floats - OpenGL style) that will Xform the
// object contained in the original AABB into the current pos, orientation,
// and scale of the box. M is column major order (1st col: 0,1,2,3).
//   A composite matrix is formed as follows:
//   C = Translation * Rotation * Scaling (using column vectors/pre-multiplication)
//   WorldPt = C * ModelPt
// The corresponding inverse Xform is also provided (GetWorld2ObjXform)
//--------------------------------------------------------------------------
float* ObjectFrame::GetObj2WorldXform(float *M)
{
  // CREATE SCALED VERSION OF ROT AXES
  Vec3f sX=X*Scale, sY=Y*Scale, sZ=Z*Scale;

  if (parent == NULL) {
    M[0]=sX.x; M[4]=sY.x; M[8]=sZ.x;  M[12]=O.x;
    M[1]=sX.y; M[5]=sY.y; M[9]=sZ.y;  M[13]=O.y;
    M[2]=sX.z; M[6]=sY.z; M[10]=sZ.z; M[14]=O.z;
    M[3]=0;    M[7]=0;    M[11]=0;    M[15]=1;
    return(M);
  }
  else {
    Mat44f M44, Mparent;
    M44[0]=sX.x; M44[4]=sY.x; M44[8]=sZ.x;  M44[12]=O.x;
    M44[1]=sX.y; M44[5]=sY.y; M44[9]=sZ.y;  M44[13]=O.y;
    M44[2]=sX.z; M44[6]=sY.z; M44[10]=sZ.z; M44[14]=O.z;
    M44[3]=0;    M44[7]=0;    M44[11]=0;    M44[15]=1;
    parent->GetObj2WorldXform(Mparent);
    (Mparent*M44).CopyInto(M);
    return(M);
  }
}

float* ObjectFrame::GetWorld2ObjXform(float *M)
{
  if (Scale<=0) { printf("Too small obj scale!\n"); Scale=1; }
  float invScale = 1/Scale;
  Vec3f sX=X*invScale, sY=Y*invScale, sZ=Z*invScale;
  if (parent == NULL) {
    M[0]=sX.x; M[4]=sX.y; M[8]=sX.z;  M[12]=-(O*sX);
    M[1]=sY.x; M[5]=sY.y; M[9]=sY.z;  M[13]=-(O*sY);
    M[2]=sZ.x; M[6]=sZ.y; M[10]=sZ.z; M[14]=-(O*sZ);
    M[3]=0;    M[7]=0;    M[11]=0;    M[15]=1;
    return(M);
  }
  else {
    Mat44f M44, Mparent;
    M44[0]=sX.x; M44[4]=sX.y; M44[8]=sX.z;  M44[12]=-(O*sX);
    M44[1]=sY.x; M44[5]=sY.y; M44[9]=sY.z;  M44[13]=-(O*sY);
    M44[2]=sZ.x; M44[6]=sZ.y; M44[10]=sZ.z; M44[14]=-(O*sZ);
    M44[3]=0;    M44[7]=0;    M44[11]=0;    M44[15]=1;
    parent->GetWorld2ObjXform(Mparent);
    (M44*Mparent).CopyInto(M);
    return(M);
  }
}

//--------------------------------------------------------------------------
// Ray-Object OBB/BOUNDING BOX intersection test. returns In-Out HitTimes
// Requires ray start and direction. HitTimes are distance to hitpoints in
// units of the length of Dir. The HitPt = Start + Dir*HitTime.
//--------------------------------------------------------------------------
int ObjectFrame::RayIsect(Vec3f &Start, Vec3f &Dir, float &InT, float &OutT)
{
  if (Obj != NULL) {
    InT=-99999, OutT=99999;    // INIT INTERVAL T-VAL ENDPTS TO -/+ "INFINITY"
    float NewInT, NewOutT;     // STORAGE FOR NEW T VALUES
    float MinD, MaxD;          // SLAB PLANE D VALS (DIST ALONG NORMAL TO PLANE)
    float NdotDir, NdotStart;  // STORAGE FOR REPEATED CALCS NEEDED FOR NewT CALC
    Vec3f OldStart, OldDir;
  
    // to allow hierarchical picking!
    if (parent != NULL) { // translate ray into space of object's parent
      OldStart = Start; OldDir = Dir;
      Vec3f Dest = Start + Dir;
      Mat44f trans;
      parent->GetWorld2ObjXform(trans);
      Dest = trans*Dest;
      Start = trans*Start;
      Dir = Dest - Start;
      Dir.Normalize();
    }
  
    // CALC CORNER EXTREME VERTICES LBF (-,-,-) AND RTN (+,+,+)
    Vec3f sMin=Obj->Min*Scale, sMax=Obj->Max*Scale;  // SCALE AABB
    Vec3f Cntr = (sMax+sMin)*0.5;            // GRAB CENTER (OBJ COORDS)
    Cntr = X*Cntr.x + Y*Cntr.y + Z*Cntr.z;   // CONVERT CENTER TO WORLD COORDS
    Cntr += O;                               // TRANSLATE WITH ORIGIN OF FRAME
    Vec3f Dim = (sMax-sMin)*0.5;             // FIND RADII (HALF DIMENSIONS)
    Vec3f sX=X*Dim.x, sY=Y*Dim.y, sZ=Z*Dim.z;  // DIM-SCALED AXES
    Vec3f AllNeg = Cntr-sX-sY-sZ;
    Vec3f AllPos = Cntr+sX+sY+sZ;

    // X-SLAB (PARALLEL PLANES PERPENDICULAR TO X-AXIS) INTERSECTION (Xaxis is Normal)
    NdotDir = X*Dir;        // CALC DOT PROD OF PLANE NORMAL AND RAY DIR
    NdotStart = X*Start;    // CALC DOT PROD OF PLANE NORMAL AND RAY START PT
    MinD = X*AllNeg;        // CALC D-VAL FOR FIRST PLANE OF SLAB (use LBF)
    MaxD = X*AllPos;        // CALC D-VAL FOR SECOND PLANE OF SLAB (use RTN)
    if (NdotDir == 0)       // CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
    {
      if ((MinD < MaxD) && ((NdotStart < MinD) || (NdotStart > MaxD))) {
        if (parent != NULL) {Start = OldStart; Dir = OldDir;}
        return(0); 
      }
      else if ((NdotStart < MaxD) || (NdotStart > MinD)) {
        if (parent != NULL) { Start = OldStart; Dir = OldDir; }
        return(0); 
      }    
    }
    else
    {
      NewInT = (MinD - NdotStart) / NdotDir;
      NewOutT = (MaxD - NdotStart) / NdotDir;
      if (NewOutT > NewInT) { if (NewInT>InT) InT=NewInT;  if (NewOutT<OutT) OutT=NewOutT; }
      else { if (NewOutT > InT) InT=NewOutT;  if (NewInT < OutT) OutT=NewInT; }
      if (InT > OutT) {
        if (parent != NULL) { Start = OldStart; Dir = OldDir; }
        return(0);
      }
    }

    // Y-SLAB (PARALLEL PLANES PERPENDICULAR TO Y-AXIS) INTERSECTION (Yaxis is Normal)
    NdotDir = Y*Dir;        // CALC DOT PROD OF PLANE NORMAL AND RAY DIR
    NdotStart = Y*Start;    // CALC DOT PROD OF PLANE NORMAL AND RAY START PT
    MinD = Y*AllNeg;        // CALC D-VAL FOR FIRST PLANE OF SLAB (use LBF)
    MaxD = Y*AllPos;        // CALC D-VAL FOR SECOND PLANE OF SLAB (use RTN)
    if (NdotDir == 0)       // CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
    {
      if ((MinD < MaxD) && ((NdotStart < MinD) || (NdotStart > MaxD))) {
        if (parent != NULL) {Start = OldStart; Dir = OldDir;}
        return(0); 
      }
      else if ((NdotStart < MaxD) || (NdotStart > MinD)) {
        if (parent != NULL) { Start = OldStart; Dir = OldDir; }
        return(0); 
      }    
    }
    else
    {
      NewInT = (MinD - NdotStart) / NdotDir;
      NewOutT = (MaxD - NdotStart) / NdotDir;
      if (NewOutT > NewInT) { if (NewInT>InT) InT=NewInT;  if (NewOutT<OutT) OutT=NewOutT; }
      else { if (NewOutT > InT) InT=NewOutT;  if (NewInT < OutT) OutT=NewInT; }
      if (InT > OutT) {
        if (parent != NULL) { Start = OldStart; Dir = OldDir; }
        return(0);
      }
    }

    // Z-SLAB (PARALLEL PLANES PERPENDICULAR TO Z-AXIS) INTERSECTION (Zaxis is Normal)
    NdotDir = Z*Dir;        // CALC DOT PROD OF PLANE NORMAL AND RAY DIR
    NdotStart = Z*Start;    // CALC DOT PROD OF PLANE NORMAL AND RAY START PT
    MinD = Z*AllNeg;        // CALC D-VAL FOR FIRST PLANE OF SLAB (use LBF)
    MaxD = Z*AllPos;        // CALC D-VAL FOR SECOND PLANE OF SLAB (use RTN)
    if (NdotDir == 0)       // CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
    {
      if ((MinD < MaxD) && ((NdotStart < MinD) || (NdotStart > MaxD))) {
        if (parent != NULL) {Start = OldStart; Dir = OldDir;}
        return(0); 
      }
      else if ((NdotStart < MaxD) || (NdotStart > MinD)) {
        if (parent != NULL) { Start = OldStart; Dir = OldDir; }
        return(0); 
      }    
    }
    else
    {
      NewInT = (MinD - NdotStart) / NdotDir;
      NewOutT = (MaxD - NdotStart) / NdotDir;
      if (NewOutT > NewInT) { if (NewInT>InT) InT=NewInT;  if (NewOutT<OutT) OutT=NewOutT; }
      else { if (NewOutT > InT) InT=NewOutT;  if (NewInT < OutT) OutT=NewInT; }
      if (InT > OutT) {
        if (parent != NULL) { Start = OldStart; Dir = OldDir; }
        return(0);
      }
    }

    if (parent != NULL) {
      Start = OldStart; Dir = OldDir;
    }

    // CHECK IF INTERSECTIONS ARE "AT OR BEYOND" THE START OF THE RAY
    if (InT>=0) return(1);
    if (OutT>=0) return(1);
  }
  return(0);
}

//----------------------------------------------------------------------------
// Returns a 4x4 OpenGL-style rotation matrix.
//----------------------------------------------------------------------------
float* ObjectFrame::GetRotationMatrix(float *M, float DegAng, Vec3f Axis)
{
  #define TORADS 0.0174532
  float RadAng = DegAng*TORADS;
  float ca=(float)cos(RadAng),
        sa=(float)sin(RadAng);
  if (Axis.x==1 && Axis.y==0 && Axis.z==0)  // ABOUT X-AXIS
  {
   M[0]=1; M[4]=0;  M[8]=0;   M[12]=0;
   M[1]=0; M[5]=ca; M[9]=-sa; M[13]=0;
   M[2]=0; M[6]=sa; M[10]=ca; M[14]=0;
   M[3]=0; M[7]=0;  M[11]=0;  M[15]=1;
  }
  if (Axis.x==0 && Axis.y==1 && Axis.z==0)  // ABOUT Y-AXIS
  {
   M[0]=ca;  M[4]=0; M[8]=sa;  M[12]=0;
   M[1]=0;   M[5]=1; M[9]=0;   M[13]=0;
   M[2]=-sa; M[6]=0; M[10]=ca; M[14]=0;
   M[3]=0;   M[7]=0; M[11]=0;  M[15]=1;
  }
  if (Axis.x==0 && Axis.y==0 && Axis.z==1)  // ABOUT Z-AXIS
  {
   M[0]=ca; M[4]=-sa; M[8]=0;  M[12]=0;
   M[1]=sa; M[5]=ca;  M[9]=0;  M[13]=0;
   M[2]=0;  M[6]=0;   M[10]=1; M[14]=0;
   M[3]=0;  M[7]=0;   M[11]=0; M[15]=1;
  }
  else                                      // ARBITRARY AXIS
  {
   Axis.Normalize();
   float x=Axis.x, y=Axis.y, z=Axis.z;
   float x2=x*x, y2=y*y, z2=z*z;
   M[0]=x2+ca*(1-x2); M[4]=(x*y)+ca*(-x*y)+sa*(-z); M[8]=(x*z)+ca*(-x*z)+sa*y;
   M[1]=(x*y)+ca*(-x*y)+sa*z; M[5]=y2+ca*(1-y2); M[9]=(y*z)+ca*(-y*z)+sa*(-x);
   M[2]=(x*z)+ca*(-x*z)+sa*(-y); M[6]=(y*z)+ca*(-y*z)+sa*x; M[10]=z2+ca*(1-z2);
   M[12]=M[13]=M[14]=M[3]=M[7]=M[11]=0;
   M[15]=1;
  }
  return(M);
}

//----------------------------------------------------------------------------
// Get vertices of the oriented bounding box (OBB) in world space. The axis-
// aligned bounding box (AABB) in object space is xformed to world space.
//----------------------------------------------------------------------------
void ObjectFrame::GetOBBverts(Vec3f *V)
{
  // USE AABB TO FIND CENTER AND DIMENSIONS OF OBB
  Vec3f sMin, sMax;
  if (Obj != NULL) {
    sMin=Obj->Min*Scale; sMax=Obj->Max*Scale;    // SCALE AABB
  }
  else {
    sMin = Vec3f(0, 0, 0); sMax = Vec3f(0, 0, 0);
  }

  Vec3f Cntr = (sMax+sMin)*0.5;            // GRAB CENTER (OBJ COORDS)
  Cntr = X*Cntr.x + Y*Cntr.y + Z*Cntr.z;   // CONVERT CENTER TO WORLD COORDS
  Cntr += O;                               // TRANSLATE WITH ORIGIN OF FRAME
  Vec3f Dim = (sMax-sMin)*0.5;             // FIND RADII (HALF DIMENSIONS)

  // CALCULATE EIGHT OBB VERTICES
  Vec3f dX=X*Dim.x, dY=Y*Dim.y, dZ=Z*Dim.z, Rpt;
  Rpt=dX+dY+Cntr; V[0]=Rpt+dZ; V[4]=Rpt-dZ; // Right-Top-Near, Right-Top-Far
  Rpt=dY-dX+Cntr; V[1]=Rpt+dZ; V[5]=Rpt-dZ; // Left-Top-Near, Left-Top-Far
  Rpt=Cntr-dX-dY; V[2]=Rpt+dZ; V[6]=Rpt-dZ; // Left-Bottom-Near, Left-Bottom-Far
  Rpt=dX-dY+Cntr; V[3]=Rpt+dZ; V[7]=Rpt-dZ; // Right-Bottom-Near, Right-Bottom-Far

  if (parent != NULL) { // have to translate from parent to world space
    Mat44f M;
    parent->GetObj2WorldXform(M);
    for (int i = 0; i < 8; i++)
      V[i] = M*V[i];
  }
}

//----------------------------------------------------------------------------
// Gives a crude bounding sphere surrounding the oriented bounding box.
//----------------------------------------------------------------------------
void ObjectFrame::GetBoundingSphere(Vec3f *Cntr, float *Rad)
{
  Vec3f Dim;
  if (parent == NULL) {
    // USE AABB TO FIND CENTER AND DIMENSIONS OF BOUNDING SPHERE
    Vec3f sMin, sMax;
    if (Obj != NULL) {
      sMin=Obj->Min*Scale; sMax=Obj->Max*Scale; // SCALE AABB
    }
    else {
      sMin = Vec3f(0, 0, 0); sMax = Vec3f(0, 0, 0);
    }

    *Cntr = (sMax+sMin)*0.5;                        // GRAB CENTER (OBJ COORDS)
    *Cntr = X*Cntr->x + Y*Cntr->y + Z*Cntr->z;      // CONVERT CENTER TO WORLD COORDS
    *Cntr += O;                                     // TRANSLATE WITH ORIGIN OF FRAME
    Dim = (sMax-sMin)*0.5;                    // FIND RADII (HALF DIMENSIONS)
  }
  else {
    Mat44f M;
    parent->GetObj2WorldXform(M);
    // USE AABB TO FIND CENTER AND DIMENSIONS OF BOUNDING SPHERE
    Vec3f sMin, sMax;
    if (Obj != NULL) {
      sMin=Obj->Min*Scale; sMax=Obj->Max*Scale; // SCALE AABB
    }
    else {
      sMin = Vec3f(0, 0 ,0); sMax = Vec3f(0, 0, 0);
    }
    *Cntr = (sMax+sMin)*0.5;                        // GRAB CENTER (OBJ COORDS)
    *Cntr = X*Cntr->x + Y*Cntr->y + Z*Cntr->z;      // CONVERT CENTER TO WORLD COORDS
    *Cntr += (M*O);                                 // TRANSLATE WITH ORIGIN OF FRAME
    Dim = (M*sMax-M*sMin)*0.5;                    // FIND RADII (HALF DIMENSIONS)
  }
  // USE LENGTH OF AABB DIAGONAL VECTOR TO DETERMINE RADIUS
  *Rad = Dim.Length();
}

//----------------------------------------------------------------------------
// Draw the frame coordinate system axes (object space axes).
//----------------------------------------------------------------------------
void ObjectFrame::DrawAxes(float LineWidth, float scale)
{
  // MAKE HALF SIZE OF BOUNDING BOX
  if (Obj != NULL) {
    Vec3f sMin=Obj->Min*Scale, sMax=Obj->Max*Scale;  // SCALE AABB
    Vec3f Dim = (sMax-sMin)*0.5;                     // FIND RADII (HALF DIMENSIONS)
    Dim *= scale;                                    // ADDITIONAL SCALING
    Vec3f V;

    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(LineWidth);
      glBegin(GL_LINES);
        V = O + X*Dim.x;
        glColor3f(1,0,0); glVertex3fv(&O.x); glVertex3fv(&V.x);
        V = O + Y*Dim.y;
        glColor3f(0,1,0); glVertex3fv(&O.x); glVertex3fv(&V.x);
        V = O + Z*Dim.z;
        glColor3f(0,0,1); glVertex3fv(&O.x); glVertex3fv(&V.x);
      glEnd();
    glPopAttrib();
    glPopAttrib();
  }
}

//----------------------------------------------------------------------------
// Writes/reads frame to/from an already open file.
// NOTE: (MJH 10/16/99) This is not hierarchical!
//----------------------------------------------------------------------------
void ObjectFrame::WriteToFile(FILE *fp)
{
    if (fp==NULL) { printf("ERROR (WriteToFile) fp==NULL\n"); return; }
    fprintf(fp,"%f %f %f ", X.x,X.y,X.z);
    fprintf(fp,"%f %f %f ", Y.x,Y.y,Y.z);
    fprintf(fp,"%f %f %f ", Z.x,Z.y,Z.z);
    fprintf(fp,"%f %f %f ", O.x,O.y,O.z);
    fprintf(fp,"%f\n", Scale);
}

int ObjectFrame::ReadFromFile(FILE *fp)
{
  if (feof(fp)) return(0);
  if (fp==NULL) { printf("ERROR (ReadFromFile) fp==NULL\n"); return(0); }
  fscanf(fp,"%f %f %f ", &X.x,&X.y,&X.z);
  fscanf(fp,"%f %f %f ", &Y.x,&Y.y,&Y.z);
  fscanf(fp,"%f %f %f ", &Z.x,&Z.y,&Z.z);
  fscanf(fp,"%f %f %f ", &O.x,&O.y,&O.z);
  fscanf(fp,"%f\n", &Scale);
  return(1);
}


//----------------------------------------------------------------------------
// Display routines
//----------------------------------------------------------------------------
void ObjectFrame::DrawOBB(float r, float g, float b)
{
  if (Obj != NULL) {
    // IF OBJECT IS SCALED, WE MUST TURN ON AUTO NORMAL NORMALIZATION
    if (Scale!=1) { glPushAttrib(GL_TRANSFORM_BIT); glEnable(GL_NORMALIZE); }

    // PERFORM OBJECT TRANSFORMATION
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    GLfloat M[16];
    glMultMatrixf(GetObj2WorldXform(M));

      Obj->DrawAABB(r,g,b);
  
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    if (Scale!=1) glPopAttrib();
  }
}

void ObjectFrame::DrawBoundingSphere(float r, float g, float b)
{
  Vec3f Cntr;
  float Rad;
  GetBoundingSphere(&Cntr,&Rad);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  if (parent != NULL) {
    Mat44f M;
    glMultMatrixf(parent->GetObj2WorldXform(M));
  }
  glTranslatef(Cntr.x,Cntr.y,Cntr.z);
    glutWireSphere(Rad,12,12);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void ObjectFrame::Display(unsigned int Attribs, int UseDisplayLists)
{ 
  // IF OBJECT IS SCALED, WE MUST TURN ON AUTO NORMAL NORMALIZATION
  if ((Attribs & OBJ_NORMALS) && Scale!=1) 
    { glPushAttrib(GL_TRANSFORM_BIT); glEnable(GL_NORMALIZE); }

  // PERFORM OBJECT-TO-WORLD TRANSFORMATION
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  GLfloat M[16];
  glMultMatrixf(GetObj2WorldXform(M));

  // START DISPLAY LIST GENERATION
  GLuint tID;
  if (UseDisplayLists)   
    if (DisplayListID==0)   
      { tID=glGenLists(1); glNewList(tID,GL_COMPILE_AND_EXECUTE); }
    else
      { glCallList(DisplayListID); 
        if ((Attribs & OBJ_NORMALS) && Scale!=1) glPopAttrib();
        return; }
  else
    if (DisplayListID!=0)
      { glDeleteLists(DisplayListID,1); DisplayListID=0; }

  // NORMAL RENDERING (if this frame has an object);
  if (Obj != NULL) Obj->Display(Attribs); 

  // END DISPLAY LIST GENERATION
  if (UseDisplayLists && DisplayListID==0)
    { DisplayListID=tID; glEndList(); }

  if (child != NULL) { // recursively display the children
    ObjectFrame *current;
    for (current = child; current; current = current->rightSibling)
      current->RecursiveDisplay(Attribs, UseDisplayLists);
  }

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  if ((Attribs & OBJ_NORMALS) && Scale!=1) 
    glPopAttrib();
}


//----------------------------------------------------------------------------
// Get world-space triangles. Uses object's GetTris routine and then transforms
// all vertices into world space. If object equals world, no xform is required
// since the Obj2World matrix will be the identity. If Tris==NULL after calling 
// this routine, then it was unable to get tris (because not implemented for the
// object, etc). Interface is the same as Object.GetTris(...) (see for details).
// If object-space tris are needed (no Obj2World Xform), then user should call 
// ObjectFrame.Obj->GetTris(). Often, the Tris array may already be alloced
// by the user (may want to pack multiple object tris in a single array); this
// routine will check Tris, if != NULL, then an array will be alloced.
// WARNING: be sure to set to NULL if allocation is required.
// A separate routine is provided to just get the number of tris.
//----------------------------------------------------------------------------
void ObjectFrame::GetTris(float* &Tris, int &NumTris)
{
  // If this frame has no object, return
  if (Obj == NULL) return;

  // GET THE OBJECT-SPACE TRIS
  Obj->GetTris(Tris,NumTris);

  // CHECK IF ABLE TO GET THE TRIS
  if (Tris==NULL || NumTris==0) return;

  // GET XFORM MATRIX (object2world)
  float M[16];
  GetObj2WorldXform(M);

  // XFORM TRIS FROM OBJECT-SPACE TO WORLD-SPACE (SHOULD AVOID IF UNNECESSARY,
  // WHEN OBJ==WORLD OR WHEN M IS THE IDENTITY). USER CAN JUST CALL .Obj->GetTris()
  float x,y,z;
  float *T = Tris;  // SAVE PTR TO TRIS (THE START OF THE FLOAT ARRAY)
  for (int i=0; i<(NumTris*9); i+=3)
  {
    x=T[i];                                     // COPY THE VERTEX
    y=T[i+1]; 
    z=T[i+2];
    T[i]   = M[0]*x + M[4]*y + M[8]*z + M[12];  // XFORM VERTEX
    T[i+1] = M[1]*x + M[5]*y + M[9]*z + M[13];
    T[i+2] = M[2]*x + M[6]*y + M[10]*z + M[14];
  }
}

int ObjectFrame::GetNumTris()
{
  if (Obj == NULL) return 0;
  return( Obj->GetNumTris() );
}

//----------------------------------------------------------------------------
// Calculates the min and max extents of the object in world space
//----------------------------------------------------------------------------
void ObjectFrame::GetWorldMinMax(Vec3f &Min, Vec3f &Max)
{
  Vec3f V[8];
  GetOBBverts(V);
  Min = Max = V[0];
  for (int i=1; i<8; i++)
    V[i].UpdateMinMax(Min,Max);
}

// hierarchical stuff

ObjectFrame* ObjectFrame::GetRoot()
{
  if (parent == NULL) return (this);
  ObjectFrame *current = parent;
  while (current->parent != NULL) current = current->GetParent();
  return current;
}

void ObjectFrame::AttachChild(ObjectFrame *newChild)
{
  if (newChild->parent != NULL) // must detach from old parent
    newChild->parent->DetachChild(newChild);

  // account for location in new parent's space
  Mat44f M, MnewParent;
  newChild->GetObj2WorldXform(M); // transform to world
  GetWorld2ObjXform(MnewParent);
  newChild->LoadIdentity();
  newChild->Xform(MnewParent*M);

  if (child == NULL) {
    child = newChild;
    newChild->parent = this;
  }
  else {
    ObjectFrame *current;
    for(current = child; current->rightSibling; current = current->rightSibling);
    current->rightSibling = newChild;
    newChild->leftSibling = current;
    newChild->parent = this;
  } 

  numChildren++;
}

void ObjectFrame::DetachChild(ObjectFrame *detached)
{
  if (detached->parent != this) { // not my child, not my problem!
    fprintf(stderr, "ObjectFrame::DetachChild: Error, invalid child\n");
    return;
  }

  // transform back to world space
  Mat44f M, Mparent;
  detached->GetObj2ParentXform(M);
  detached->LoadIdentity();
  GetObj2WorldXform(Mparent);
  detached->Xform(Mparent*M);

  if (detached->leftSibling == NULL) { // this is the head of the list
    detached->parent->child = detached->rightSibling;
    if (detached->rightSibling) {
      detached->rightSibling->leftSibling = NULL;
      detached->rightSibling = NULL;
    }
  }
  else {
    detached->leftSibling->rightSibling = detached->rightSibling;
    if (detached->rightSibling)
      detached->rightSibling->leftSibling = detached->leftSibling;
    detached->leftSibling = detached->rightSibling = NULL;
  }
  detached->parent = NULL;
  numChildren--;
}

float* ObjectFrame::GetObj2ParentXform(float *M)
{
  Vec3f sX=X*Scale, sY=Y*Scale, sZ=Z*Scale;    // CREATE SCALED VERSION OF ROT AXES
  M[0]=sX.x; M[4]=sY.x; M[8]=sZ.x;  M[12]=O.x;
  M[1]=sX.y; M[5]=sY.y; M[9]=sZ.y;  M[13]=O.y;
  M[2]=sX.z; M[6]=sY.z; M[10]=sZ.z; M[14]=O.z;
  M[3]=0;    M[7]=0;    M[11]=0;    M[15]=1;
  return(M);
}

float* ObjectFrame::GetParent2ObjXform(float *M)
{
  if (Scale<=0) { printf("Too small obj scale!\n"); Scale=1; }
  float invScale = 1/Scale;
  Vec3f sX=X*invScale, sY=Y*invScale, sZ=Z*invScale;
  M[0]=sX.x; M[4]=sX.y; M[8]=sX.z;  M[12]=-(O*sX);
  M[1]=sY.x; M[5]=sY.y; M[9]=sY.z;  M[13]=-(O*sY);
  M[2]=sZ.x; M[6]=sZ.y; M[10]=sZ.z; M[14]=-(O*sZ);
  M[3]=0;    M[7]=0;    M[11]=0;    M[15]=1;
  return(M);
}

void ObjectFrame::RecursiveDisplay(unsigned int Attribs, int UseDisplayLists)
{ 
  // IF OBJECT IS SCALED, WE MUST TURN ON AUTO NORMAL NORMALIZATION
  if ((Attribs & OBJ_NORMALS) && Scale!=1) 
    { glPushAttrib(GL_TRANSFORM_BIT); glEnable(GL_NORMALIZE); }

  // PERFORM OBJECT-TO-PARENT TRANSFORMATION (aggregate transform)
  //glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  GLfloat M[16];
  glMultMatrixf(GetObj2ParentXform(M));
  
  // START DISPLAY LIST GENERATION
  GLuint tID;
  if (UseDisplayLists)   
    if (DisplayListID==0)   
      { tID=glGenLists(1); glNewList(tID,GL_COMPILE_AND_EXECUTE); }
    else
      { glCallList(DisplayListID); 
        if ((Attribs & OBJ_NORMALS) && Scale!=1) glPopAttrib();
        return; }
  else
    if (DisplayListID!=0)
      { glDeleteLists(DisplayListID,1); DisplayListID=0; }

  // NORMAL RENDERING
  if (Obj != NULL) Obj->Display(Attribs); 

  // END DISPLAY LIST GENERATION
  if (UseDisplayLists && DisplayListID==0)
    { DisplayListID=tID; glEndList(); }

  if (child != NULL) { // recursively display the children
    ObjectFrame *current;
    for (current = child; current; current = current->rightSibling)
      current->RecursiveDisplay(Attribs, UseDisplayLists);
  }

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  if ((Attribs & OBJ_NORMALS) && Scale!=1) 
    glPopAttrib();
}

//----------------------------------------------------------------------------
// Given a RAY (start and direction) in world coordinates emanating from
// the center-of-projection through a pixel on the screen, returns a pointer to
// the first successor object int the hierarchy hit (NULL if no object is hit) 
//
// NOTE: (MJH 10/16/99) if you are using an objMgr for all objframes in this hierarchy, 
// this function is not necessary.  Untested as of 10/16/99
//----------------------------------------------------------------------------
ObjectFrame* ObjectFrame::SelectHier(Vec3f &Start, Vec3f &Dir, float &InT, float &OutT)
{
  #define INFINITY 99999
  float NearestHitT=INFINITY, RecInT, RecOutT;
  ObjectFrame *NearestObj=NULL;
  ObjectFrame *current;
  // recursively find intersections with children
  for (current = child; current; current = current->rightSibling) {
    current->SelectHier(Start, Dir, RecInT, RecOutT);
    if (RecInT>=0 && RecInT<NearestHitT) { 
      NearestHitT=InT; NearestObj=current; 
      InT = RecInT; OutT = RecOutT; 
    }
    if (RecOutT>=0 && RecOutT<NearestHitT) { 
      NearestHitT=OutT; NearestObj=current; 
      InT = RecInT; OutT = RecOutT;
    }
  }
  // find intersection with self
  if ( RayIsect(Start,Dir,RecInT, RecOutT) ) {
    if (RecInT>=0 && RecInT<NearestHitT) { 
      NearestHitT=InT; NearestObj=this; 
      InT = RecInT; OutT = RecOutT; 
    }
    if (RecOutT>=0 && RecOutT<NearestHitT) { 
      NearestHitT=OutT; NearestObj=this; 
      InT = RecInT; OutT = RecOutT;
    }
  }
  return(NearestObj);
}

void ObjectFrame::UpdatePosition(float deltaTime)
{
  Vec3f a(totalForce.x/mass, totalForce.y/mass, totalForce.z/mass);
  O += velocity*deltaTime + a*deltaTime*deltaTime;
  velocity += a*deltaTime;
}
