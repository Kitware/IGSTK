/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/
#include "Vector.h"
#include "UtilityFunctions.h"
#include "MTC.h"



/****************************/
/** Constructor */
Vector::Vector(int handle)
{
  // If a handle is provided to this class, don't create a new one
  if (handle != 0)
    this->m_handle = handle;
  else
    this->m_handle = Vector_New();
  this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
Vector::~Vector()
{
  if (this->m_handle != 0 && this->ownedByMe )
    Vector_Free(this->m_handle);
}
int Vector::Handle()
{
  return ( m_handle);

}

/****************************/
/** */
int Vector::getEndPos2x3(double* result)
{
  int r = Vector_EndPosGet(this->m_handle, result );
  return r;
}

/****************************/
/** */
int Vector::setEndPos2x3(double* newPos2x3)
{
  int result = mtOK;
  //result = Vector_EndPosSet(this->m_handle, newPos2x3);
  (void) newPos2x3; // Get rid of unused variable warnings
  return result;
}

/****************************/
/** */
int Vector::getEndXPoints(double* result)
{
  int r = Vector_EndXPointsGet(this->m_handle, result);
  return r;
}
