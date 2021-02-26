//------------------------------------------------------------------------------
// File : texdepthmesh_hier.cpp
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
// texdepthmesh_hier.cpp
//============================================================================

#include "texdepthmesh_hier.hpp"
#include <camutils.hpp>

// RECURSION TERMINATION BASED ON MinDepthRange AND MinDim IN PIXELS
TDM_Hierarchy::TDM_Hierarchy(TexturedDepthMesh *roottdm, float MinDepthRange, int MinDim)
{
  RootTDM=roottdm;
  Root = new TDM_node(RootTDM, 0,RootTDM->nCols-1, 0,RootTDM->nRows-1);
  BuildHierarchy(Root,MinDepthRange,MinDim);
}

TDM_Hierarchy::~TDM_Hierarchy()
{
  delete Root;
}

void TDM_Hierarchy::BuildHierarchy(TDM_node *RootNode, float MinDepthRange, int MinDim)
{
  if ( RootNode->Subdivide(MinDepthRange,MinDim) )
    for (int i=0; i<4; i++)
      BuildHierarchy( RootNode->Child[i], MinDepthRange,MinDim );
}

long TDM_Hierarchy::NumNodes()
{
  return( RecursiveNumNodes(Root) );
}
long TDM_Hierarchy::RecursiveNumNodes(TDM_node *node)
{
  if (node==NULL) return(0);
  return ( RecursiveNumNodes(node->Child[0]) + RecursiveNumNodes(node->Child[1]) +
           RecursiveNumNodes(node->Child[2]) + RecursiveNumNodes(node->Child[3]) + 1 );
}

long TDM_Hierarchy::GetByteConsumption()
{
  return( NumNodes() * sizeof(TDM_node) + sizeof(*this) );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TDM_Hierarchy::DisplayLevel(int Level)
{
  RecursiveDisplayLevel(Root,Level);
}
void TDM_Hierarchy::RecursiveDisplayLevel(TDM_node *node, int Level)
{
  if (node==NULL) return;
  if (Level==0)            // ROOT NODE IS AT LEVEL 0
    node->DisplayNode();
  else
    for (int i=0; i<4; i++)
      RecursiveDisplayLevel( node->Child[i], Level-1 );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TDM_Hierarchy::DisplayLeaves()
{
  RecursiveDisplayLeaves(Root);
}
void TDM_Hierarchy::RecursiveDisplayLeaves(TDM_node *node)
{
  if (node->IsLeaf())
    node->DisplayNode();
  else
    for (int i=0; i<4; i++)
      RecursiveDisplayLeaves( node->Child[i] );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TDM_Hierarchy::DisplayPtsLeaves_Adaptive(Camera *NewView)
{
  RecursiveDisplayPtsLeaves_Adaptive(Root,NewView);
}
void TDM_Hierarchy::RecursiveDisplayPtsLeaves_Adaptive(TDM_node *node, Camera *NewView)
{
  if (node->IsLeaf())
//    node->DisplayAsPtsWithColor(1,1);
    node->DisplayAsPtsWithColor_Adaptive(NewView);
  else
    for (int i=0; i<4; i++)
      RecursiveDisplayPtsLeaves_Adaptive( node->Child[i], NewView );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TDM_Hierarchy::DisplayPts_AdaptiveWithVFC(Camera *NewView)
{
  Vec3f CamV[8];
  float CamP[6][4];
  NewView->CalcVerts(CamV);
  CalcCamPlanes(CamV,CamP);
  RecursiveDisplayPts_AdaptiveWithVFC(Root,NewView,CamV,CamP);
}

void TDM_Hierarchy::RecursiveDisplayPts_AdaptiveWithVFC(TDM_node *node, Camera *NewView, Vec3f CamV[8], float CamP[6][4])
{
  if (node==NULL) return;

  Vec3f NodeV[8]; 
  float NodeP[6][4];
  node->CalcVerts(NodeV);
  node->CalcPlanes(NodeV,NodeP);

  int Overlap = CamCamOverlap(CamV,CamP, NodeV,NodeP);

  if (Overlap==-1) // IN (RECUR ONLY IF MAGNIFYING)
  {
    float Xstep, Ystep;
    node->CalcStepSizes(NewView->Orig, &Xstep, &Ystep);
    float MinStep = (Xstep<Ystep)?Xstep:Ystep;
    int IsMagnification = MinStep<1;

    if (IsMagnification)
    { RecursiveDisplayPtsLeaves_Adaptive(node,NewView); return; }

    if (node->IsLeaf() || !IsMagnification)
    { node->DisplayAsPtsWithColor_WithPtMag(Xstep,Ystep); return; }
  }
  else if (Overlap==0 && node->IsLeaf()) // PARTIAL
  { 
    node->DisplayAsPtsWithColor_Adaptive(NewView); 
    return; 
  }

//  else // OUT
//  { node->DisplayBox(NodeV,0,0,1); return; }

  for (int i=0; i<4; i++)
    RecursiveDisplayPts_AdaptiveWithVFC( node->Child[i], NewView,CamV,CamP );
}

void TDM_Hierarchy::RecursiveDisplayPtsLeaves(TDM_node *node, Camera *NewView)
{
  if (node->IsLeaf())
  {
    node->DisplayAsPtsWithColor_Adaptive(NewView);
//    node->DisplayNode();
  }
  else
    for (int i=0; i<4; i++)
      RecursiveDisplayPtsLeaves( node->Child[i], NewView );
}
