/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/
#include "Collection.h"


MTCollection::Collection::Collection(int h)
{
  // If a handle is already passed to this class, then use that handle and don't create a new one.
  if (h != 0)
    this->m_handle = h;
  else
    this->m_handle = Collection_New();

  this->ownedByMe = TRUE;
}

/****************************/
/** Destructor */
MTCollection::Collection::~Collection()
{
  if (this->m_handle != 0 && this->ownedByMe == true)
    Collection_Free(this->m_handle);
}

/****************************/
/** Add an item to the Collection */
void MTCollection::Collection::add(int val)
{
  Collection_Add(this->m_handle, val, 0);
}

/****************************/
/** Remove an item from the Collection */
void MTCollection::Collection::remove(int idx)
{
  Collection_Remove(this->m_handle, idx);
}

/****************************/
/** Return the count of the items in the Collection */
int MTCollection::Collection::count()
{
  int result = Collection_Count(m_handle);
  return result;
}

/****************************/
/** Returns the integer item  of index  idx in the Collection  */
int MTCollection::Collection::itemI(int idx)
{
  int intResult = 0;
  double dblResult = 0;
  Collection_Item(this->m_handle, idx, &intResult, &dblResult);
  return intResult;
}

/****************************/
/** Returns the integer item  of index  idx in the Collection  */
double MTCollection::Collection::itemD(int idx)
{
  int intResult = 0;
  double dblResult = 0;
  Collection_Item(this->m_handle, idx, &intResult, &dblResult);
  return dblResult;
}
