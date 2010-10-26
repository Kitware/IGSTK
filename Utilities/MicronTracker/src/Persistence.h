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
#ifndef __PERSISTENCE_H__
#define  __PERSISTENCE_H__

#include "MTC.h"

class Persistence
{
public:
  Persistence();
  ~Persistence();
  inline int getHandle() { return m_handle; };

  // Set/Get path
  void setPath(const char* path);
  //char* getPath();
  void setSection(const char* section);
  int retrieveInt(const char* name, int defaultVal);
  double retrieveDouble(const char* name, double defaultVal);

  void saveInt(const char* name, int val);
  void saveDouble(const char* name, double val);

private:
  int m_handle;
  bool ownedByMe;
};

#endif
