//------------------------------------------------------------------------------
// File : nurbs.cpp
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

//===========================================================================
// nurbs.cpp
//===========================================================================

#include <stdlib.h>
#include "nurbs.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NURBSobj::NURBSobj()
{
}

NURBSobj::~NURBSobj()
{ 
  delete[] NurbsSurfs; 
  if (pNurbs) gluDeleteNurbsRenderer(pNurbs);
}

//---------------------------------------------------------------------------
// Reads in the NURBS model from a file (Subodh's format) and creates an
// OpenGL NURBS object directly. User can specify NURBS or BEZIER input.
//---------------------------------------------------------------------------
void NURBSobj::ReadInNurbsObject(const char* FileName, int SurfaceType)
{
  FILE* fp = fopen(FileName,"r");          // OPEN THE NURBS MODEL FILE
  if (fp==NULL) { printf("ERROR: unable to open NURBS file!\n"); exit(1); }
  fscanf(fp, "%d", &NumSurfs);             // ALLOC ARRAY OF NURBS SURFACES
  NurbsSurfs = new NURBSsurface[NumSurfs];
  int i;
  for (i=0; i<NumSurfs; i++)               // READ IN THE NURBS SURFACES
    NurbsSurfs[i].ReadInSurface(fp, SurfaceType);
  CalcAABB();                              // CALC THE EXTENTS THE ENTIRE NURBS OBJ
  InitNurbsObject();                       // INIT THE NURBS OBJECT
  fclose(fp);                              // CLOSE THE NURBS MODEL FILE
}


//---------------------------------------------------------------------------
// Calc the extents of an Axis-Aligned bounding box around the NURBS model.
// NOTE: assumes that the individual surfaces have proper extents defined.
//---------------------------------------------------------------------------
void NURBSobj::CalcAABB()
{
  Min[0]=NurbsSurfs[0].Min[0]; Min[1]=NurbsSurfs[0].Min[1]; Min[2]=NurbsSurfs[0].Min[2]; 
  Max[0]=NurbsSurfs[0].Max[0]; Max[1]=NurbsSurfs[0].Max[1]; Max[2]=NurbsSurfs[0].Max[2]; 
  for (int i=1; i<NumSurfs; i++)
  {
         if (NurbsSurfs[i].Min[0] < Min[0]) Min[0]=NurbsSurfs[i].Min[0];
    else if (NurbsSurfs[i].Max[0] > Max[0]) Max[0]=NurbsSurfs[i].Max[0];
         if (NurbsSurfs[i].Min[1] < Min[1]) Min[1]=NurbsSurfs[i].Min[1];
    else if (NurbsSurfs[i].Max[1] > Max[1]) Max[1]=NurbsSurfs[i].Max[1];
         if (NurbsSurfs[i].Min[2] < Min[2]) Min[2]=NurbsSurfs[i].Min[2];
    else if (NurbsSurfs[i].Max[2] > Max[2]) Max[2]=NurbsSurfs[i].Max[2];
  }
}


//---------------------------------------------------------------------------
// Initialize the NURBS object with OpenGL
//---------------------------------------------------------------------------
void NURBSobj::InitNurbsObject()
{
  // CREATE A NEW OPENGL NURBS RENDERER
  pNurbs = gluNewNurbsRenderer();

  // INITIALIZE NURBS PROPERTIES
  gluNurbsProperty(pNurbs, (GLenum)GLU_SAMPLING_TOLERANCE, 25.0);
  gluNurbsProperty(pNurbs, (GLenum)GLU_DISPLAY_MODE, GLU_FILL);
//  gluNurbsProperty(pNurbs, (GLenum)GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
  gluNurbsProperty(pNurbs, (GLenum)GLU_CULLING, GL_TRUE);
}


//---------------------------------------------------------------------------
// DISPLAY THE NURBS OBJECT
//---------------------------------------------------------------------------
void NURBSobj::Display_NO_ATTRIBS()
{
  int AutoNormalWasNotEnabled = !glIsEnabled(GL_AUTO_NORMAL);  // NO NORMALS!
  if (!AutoNormalWasNotEnabled) glDisable(GL_AUTO_NORMAL);
  for (int i=0; i<NumSurfs; i++) 
    NurbsSurfs[i].Display_NO_ATTRIBS(pNurbs);
  if (!AutoNormalWasNotEnabled) glEnable(GL_AUTO_NORMAL);
}

void NURBSobj::Display_COLORS()
{
  int AutoNormalWasNotEnabled = !glIsEnabled(GL_AUTO_NORMAL);  // NO NORMALS!
  if (!AutoNormalWasNotEnabled) glDisable(GL_AUTO_NORMAL);
  for (int i=0; i<NumSurfs; i++) 
    NurbsSurfs[i].Display_COLORS(pNurbs);
  if (!AutoNormalWasNotEnabled) glEnable(GL_AUTO_NORMAL);
}

void NURBSobj::Display_NORMALS()
{
  int AutoNormalWasNotEnabled = !glIsEnabled(GL_AUTO_NORMAL);  // NORMALS!
  if (AutoNormalWasNotEnabled) glEnable(GL_AUTO_NORMAL);
  for (int i=0; i<NumSurfs; i++) 
    NurbsSurfs[i].Display_NO_ATTRIBS(pNurbs);
  if (AutoNormalWasNotEnabled) glDisable(GL_AUTO_NORMAL);
}

void NURBSobj::Display_COLORS_NORMALS()
{
  int AutoNormalWasNotEnabled = !glIsEnabled(GL_AUTO_NORMAL);  // NORMALS!
  if (AutoNormalWasNotEnabled) glEnable(GL_AUTO_NORMAL);
  for (int i=0; i<NumSurfs; i++) 
    NurbsSurfs[i].Display_COLORS(pNurbs);
  if (AutoNormalWasNotEnabled) glDisable(GL_AUTO_NORMAL);
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NURBSsurface::NURBSsurface() 
{
}

NURBSsurface::~NURBSsurface()
{ 
  delete[] Uknots; 
  delete[] Vknots; 
  delete[] CtrlPts;
  delete[] TrimLoops;
}

//---------------------------------------------------------------------------
// reads in a single NURBS (or BEZIER) surface from the given file.
//---------------------------------------------------------------------------
void NURBSsurface::ReadInSurface(FILE* fp, int SurfaceType)
{
  int i, Udeg, Vdeg;
  fscanf(fp, "%d %d", &Udeg, &Vdeg);              // READ IN U AND V DEGREES
  Uorder = Udeg+1;                                // STORE THE U AND V ORDERS
  Vorder = Vdeg+1;

  // ----NURBS SURFACES REQUIRE KNOT VECTORS AND # OF CTRL PTS----
  if (SurfaceType==NURBS_SURF)
  {
    fscanf(fp, "%d", &NumKnotsU);                 // READ IN # OF KNOTS IN U
    Uknots = new float[NumKnotsU];                // ALLOC AND READ IN THE ARRAY OF U KNOTS
    for (i=0; i<NumKnotsU; i++)
      fscanf(fp, "%f", &Uknots[i]);
    fscanf(fp, "%d", &NumKnotsV);                 // READ IN # OF KNOTS IN V
    Vknots = new float[NumKnotsV];                // ALLOC AND READ IN THE ARRAY OF V KNOTS
    for (i=0; i<NumKnotsV; i++)
      fscanf(fp, "%f", &Vknots[i]);
    fscanf(fp,"%d %d",&NumCtrlPtsU,&NumCtrlPtsV); // READ IN # OF CTRL PTS IN U AND V
  }

  // ----BEZIER SURFACES NEED NOT SPECIFY KNOTS, BUT THEY MUST STILL BE CREATED----
  else
  {
    NumKnotsU = Uorder*2;                         // FOR BEZIER, NumKnots = 2 * Order
    NumKnotsV = Vorder*2;
    Uknots = new float[NumKnotsU];                // ALLOCATE KNOT ARRAYS
    Vknots = new float[NumKnotsV];
    for (i=0; i<Uorder; i++) Uknots[i]=0;         // FOR BEZIER, KNOT VALUES ARE SYMMETRIC
    for (; i<NumKnotsU; i++) Uknots[i]=1;         // (0's ON THE LEFT, 1's ON THE RT)
    for (i=0; i<Vorder; i++) Vknots[i]=0;
    for (; i<NumKnotsV; i++) Vknots[i]=1;
    NumCtrlPtsU = Uorder;                         // FOR BEZIER, # CTRL PTS = ORDER
    NumCtrlPtsV = Vorder;
  }

  NumCtrlPts = NumCtrlPtsU * NumCtrlPtsV;         // ALLOC AND READ IN THE ARRAY OF CTRL PTS
  CtrlPts = new Vec4fv[NumCtrlPts];
  for (i=0; i < NumCtrlPts; i++)
    fscanf(fp, "%f %f %f %f", &CtrlPts[i][0],&CtrlPts[i][1],
                              &CtrlPts[i][2],&CtrlPts[i][3]);

  unsigned int Tmp;                               // READ IN SURFACE COLOR
  fscanf(fp, "%d", &Tmp); Color[0]=(unsigned char)Tmp;
  fscanf(fp, "%d", &Tmp); Color[1]=(unsigned char)Tmp;
  fscanf(fp, "%d", &Tmp); Color[2]=(unsigned char)Tmp;

  fscanf(fp, "%d", &NumTrimLoops);                // READ IN # OF TRIM LOOPS
  TrimLoops = new NURBStrimLoop[NumTrimLoops];    // ALLOC ARRAY OF TRIM LOOPS
  for (i=0; i < NumTrimLoops; i++)                // READ IN THE TRIM LOOPS
    TrimLoops[i].ReadInTrimLoop(fp,SurfaceType);

  CalcAABB();                                     // CALC EXTENTS OF THE SURF BASED ON CTRL PTS
//Print();
}


//---------------------------------------------------------------------------
// Calculate extents of an Axis-Aligned bounding box around the surface.
//---------------------------------------------------------------------------
void NURBSsurface::CalcAABB()
{
  Min[0]=CtrlPts[0][0]; Min[1]=CtrlPts[0][1]; Min[2]=CtrlPts[0][2];
  Max[0]=CtrlPts[0][0]; Max[1]=CtrlPts[0][1]; Max[2]=CtrlPts[0][2];
  for (int i=1; i < NumCtrlPts; i++)
  {
         if (CtrlPts[i][0]<Min[0]) Min[0]=CtrlPts[i][0];
    else if (CtrlPts[i][0]>Max[0]) Max[0]=CtrlPts[i][0];
         if (CtrlPts[i][1]<Min[1]) Min[1]=CtrlPts[i][1];
    else if (CtrlPts[i][1]>Max[1]) Max[1]=CtrlPts[i][1];
         if (CtrlPts[i][2]<Min[2]) Min[2]=CtrlPts[i][2];
    else if (CtrlPts[i][2]>Max[2]) Max[2]=CtrlPts[i][2];
  }
}


//---------------------------------------------------------------------------
// Displays the NURBS surface using GLU NURBS routine. This routine requires
// a pointer to the GLU NURBS renderer (there is one per NURBS object)
// that must have been initialized elsewhere (NURBSobj::InitNurbsObject).
// FastDisplay()
//---------------------------------------------------------------------------
void NURBSsurface::Display_NO_ATTRIBS(GLUnurbsObj* pNurbs)
{
  gluBeginSurface(pNurbs);
    gluNurbsSurface(pNurbs,
      NumKnotsU, Uknots,               // U KNOTS
      NumKnotsV, Vknots,               // V KNOTS
      NumCtrlPtsV * 4,                 // STEPSIZE TO NEXT CTRL PT IN U
      4,                               // STEPSIZE TO NEXT CTRL PT IN V
      &CtrlPts[0][0],                  // ARRAY OF CTRL POINTS
      Uorder,                          // ORDER IN U
      Vorder,                          // ORDER IN V
      GL_MAP2_VERTEX_4);               // TYPE OF SURFACE
    for (int i=0; i<NumTrimLoops; i++) // DEFINE TRIMMING CURVES
      TrimLoops[i].Display(pNurbs);
  gluEndSurface(pNurbs);
}

void NURBSsurface::Display_COLORS(GLUnurbsObj* pNurbs)
{
  glColor3ubv(Color);
  Display_NO_ATTRIBS(pNurbs);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void NURBSsurface::Print()
{
  int i;
  printf("%d %d\n", Uorder, Vorder);
  printf("%d\n", NumKnotsU);
  for (i=0; i<NumKnotsU; i++)
    printf("%f ", Uknots[i]);   
  printf("\n");
  printf("%d\n", NumKnotsV);
  for (i=0; i<NumKnotsV; i++)
    printf("%f ", Vknots[i]);
  printf("\n");
  printf("%d %d\n", NumCtrlPtsU, NumCtrlPtsV);
  for (i=0; i < NumCtrlPts; i++)
    printf("%f %f %f %f\n", CtrlPts[i][0],CtrlPts[i][1], 
                          CtrlPts[i][2],CtrlPts[i][3]);
  printf("%d %d %d\n", Color[0],Color[1],Color[2]);
  printf("%d\n", NumTrimLoops);
  for (i=0; i<NumTrimLoops; i++)
    TrimLoops[i].Print();
  printf("---\n");
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NURBStrimLoop::NURBStrimLoop() 
{
}

NURBStrimLoop::~NURBStrimLoop()
{ 
  delete[] TrimSegs; 
}

//---------------------------------------------------------------------------
// READS in an entire trimming curve loop which can be composed of multiple
// curve segments (NURBStrimSeg)
//---------------------------------------------------------------------------
void NURBStrimLoop::ReadInTrimLoop(FILE* fp, int SurfaceType)
{
  fscanf(fp, "%d", &NumSegsPerLoop);  // # OF CURVE SEGS FORMING TRIM LOOP
  TrimSegs = new NURBStrimSeg[NumSegsPerLoop];  // ALLOC ARRAY OF CURVE SEGS
  int i;
  for (i=0; i<NumSegsPerLoop; i++)    // READ IN THE CURVE SEGMENTS
    TrimSegs[i].ReadInTrimSeg(fp,SurfaceType);
}


//---------------------------------------------------------------------------
// ADDS A TRIMMING CURVE TO A NURBS DEFINITION (must have a ptr to the NURBS
// RENDERER). This is called from within NURBSsurface::Display()
//---------------------------------------------------------------------------
void NURBStrimLoop::Display(GLUnurbsObj* pNurbs)
{
  gluBeginTrim(pNurbs);               // START THE TRIMMING CURVE LOOP
    for (int i=0; i<NumSegsPerLoop; i++)  // ADD EACH LOOP SEGMENT
      TrimSegs[i].Display(pNurbs);    // TO FORM A CLOSED TRIM LOOP
  gluEndTrim(pNurbs);                 // END THE TRIMMING CURVE LOOP
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void NURBStrimLoop::Print()
{
  printf(" %d\n", NumSegsPerLoop);
  for (int i=0; i<NumSegsPerLoop; i++)
    TrimSegs[i].Print();
}


//---------------------------------------------------------------------------
// READS in a single trimming curve segment (NURBS or piecewise linear).
// SurfaceType is needed here because NURBS surfaces can have Piecewise-linear
// or NURBS trim curves and BEZIER surfs can have PWL or rational BEZIER trim curves.
//---------------------------------------------------------------------------
void NURBStrimSeg::ReadInTrimSeg(FILE* fp, int SurfaceType)
{
  int i, Deg;
  fscanf(fp, "%d", &iAdjPatch);     // INDEX OF ADJ PATCH (-1 IF UNKNOWN)
  fscanf(fp, "%d", &iAdjTrim);      // INDEX OF ADJ TRIM CURVE (-1 IF UNKNOWN)
  fscanf(fp, "%d", &Deg);           // DEGREE OF TRIMMING CURVE (for NURBS)
  Order = Deg+1;                    // **BUT** STORE THE ORDER
  fscanf(fp, "%d", &Type);          // TYPE OF TRIM (2 IF NURBS, 1 IF PWL)

  //--- NURBS SURFACE TRIMMING CURVES ---
  if (SurfaceType==NURBS_SURF)
  {
    if (Type==NURBS_TRIM)             // ONLY NURBS TRIM CURVES HAVE KNOTS
    {
      fscanf(fp, "%d", &NumKnots);    // # OF KNOTS (for NURBS)
      Knots = new float[NumKnots];    // ALLOC ARRAY OF KNOTS (for NURBS)
      for (i=0; i<NumKnots; i++)
        fscanf(fp, "%f", &Knots[i]);  // READ IN ARRAY OF KNOTS (for NURBS)
    }
    fscanf(fp, "%d", &NumCtrlPts);    // # OF CTRL POINTS
    CtrlPts = new Vec3fv[NumCtrlPts]; // ALLOC ARRAY OF CTRL PTS (3D)
    if (Type==PWL_TRIM)               // PIECEWISE LINEAR TRIM CURVE:
      for (i=0; i<NumCtrlPts; i++)    // HAVE 2D CTRL PTS
        fscanf(fp, "%f %f", &CtrlPts[i][0], &CtrlPts[i][1]);
    else                              // NURBS TRIM CURVE:
      for (i=0; i<NumCtrlPts; i++)    // HAVE 3D (rational) CTRL PTS
        fscanf(fp, "%f %f %f", &CtrlPts[i][0], &CtrlPts[i][1], &CtrlPts[i][2]);
  }

  //--- BEZIER SURFACE TRIMMING CURVES ---
  else // SurfaceType==BEZIER_SURF
  {
     NumCtrlPts = Order;                     // FOR BEZIER, # CTRL PTS = ORDER
     CtrlPts = new Vec3fv[NumCtrlPts];       // ALLOC ARRAY OF CTRL PTS (3D)
     if (Type==PWL_TRIM)                     // PIECEWISE LINEAR TRIM CURVE
     {
        for (i=0; i<NumCtrlPts; i++)         // HAVE 2D CTRL PTS
          fscanf(fp, "%f %f", &CtrlPts[i][0], &CtrlPts[i][1]);
     }
     else                                    // BEZIER TRIM CURVE (MUST CNVRT TO NURBS)
     {
        for (i=0; i<NumCtrlPts; i++)         // HAVE 3D (rational) CTRL PTS
          fscanf(fp, "%f %f %f", &CtrlPts[i][0], &CtrlPts[i][1], &CtrlPts[i][2]);
        NumKnots = Order*2;                  // FOR BEZIER, NumKnots = 2 * Order
        Knots = new float[NumKnots];         // ALLOCATE KNOT ARRAYS
        for (i=0; i<Order; i++) Knots[i]=0;  // FOR BEZIER, KNOT VALUES ARE SYMMETRIC
        for (; i<NumKnots; i++) Knots[i]=1;  // (0's ON THE LEFT, 1's ON THE RT)
     }
  }
}

//---------------------------------------------------------------------------
// ADDS A TRIMMING CURVE SEGMENT TO A TRIM LOOP DEF (must have a ptr to the NURBS
// RENDERER). This is called from within NURBSsurface::Display()
//---------------------------------------------------------------------------
void NURBStrimSeg::Display(GLUnurbsObj* pNurbs)
{
  if (Type==PWL_TRIM)                 // PIECEWISE LINEAR CURVE

    gluPwlCurve(pNurbs,               // PTR TO NURBS RENDERER
      NumCtrlPts,                     // # OF 2D CTRL PTS
      &CtrlPts[0][0],                 // ARRAY OF CTRL PTS
      3,                              // INCR TO NEXT CTRL PT (3 TO BE GENERAL)
      (GLenum)GLU_MAP1_TRIM_2);       // TYPE OF CURVE OUTPUT (2D)

  else                                // NURBS TRIMMING CURVE

    gluNurbsCurve(pNurbs,             // PTR TO NURBS RENDERER
      NumKnots,                       // # OF KNOTS
      Knots,                          // ARRAY OF KNOT VALUES
      3,                              // INCR TO NEXT CTRL PT
      &CtrlPts[0][0],                 // ARRAY OF CTRL PTS (# DEFINED BY KNOTS)
      Order,                          // ORDER OF CURVE
      GL_MAP1_VERTEX_3);              // TYPE OF CURVE OUTPUT (2D RATIONAL - 3D)
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void NURBStrimSeg::Print()
{
  int i;
  printf("\t%d %d\n", iAdjPatch, iAdjTrim);
  printf("\t%d %d\n", Order, Type);
  if (Type==PWL_TRIM)
  {
    for (i=0; i < NumCtrlPts; i++)
      printf("\t%f %f\n", CtrlPts[i][0], CtrlPts[i][1]);
  }
  else if (Type==NURBS_TRIM)
  {
    for (i=0; i < NumCtrlPts; i++)
      printf("\t%f %f %f\n", CtrlPts[i][0], CtrlPts[i][1], CtrlPts[i][2]);
    printf("\t%d", Order);
    printf("\t");
    for (i=0; i < NumKnots; i++)
      printf("%f ", Knots[i]);
    printf("\n");
  }
}
