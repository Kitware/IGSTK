/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*    Modified by:
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/
#ifndef __XPOINT_H__
#define __XPOINT_H__

#include "MTC.h"
#include "Collection.h"
#include "MCamera.h"
#include "Facet.h"
#include "Xform3D.h"
#include "Persistence.h"

class MTXPoint
{
public:
  MTXPoint(int h=0);
  ~MTXPoint();
  inline int getHandle(){ return m_handle; };

  int getIndex();
  int setIndex(int Index);

  int Position3D(double* x, double* y, double*z);
  int Position2D(double* x0, double* y0, double* x1, double* y1, double* x2, double* y2);

  double Distance();

  //void XPstruct(XPointType XPL, XPointType XPR);


private:
  int m_handle;
  bool ownedByMe;

};

#endif
