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

#ifndef __UTILITYFUNCTIONS_H__
#define __UTILITYFUNCTIONS_H__

#include <vector>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#include <string>
#else
#include <cstdlib>
#include <string.h>
#endif
#include "FL/filename.H"
#include <iostream>
#include "math.h"
#include "MTC.h"

#ifdef _DEBUG
  #pragma warning(disable:4786)
#endif



#define PI 3.1415926535897932384626433832795

#define Cos6Degs 0.994522
#define Cos8Degs 0.990268
#define Cos10Degs 0.985
#define Cos15Degs 0.966
#define Cos20Degs 0.94
#define Cos25Degs 0.906
#define Cos30Degs 0.866
#define Cos40Degs 0.766
#define UnknownX 1E20
using namespace std;

namespace mtUtils
{
  void getCurrPath(char* currPath);
  void getFileNamesFromDirectory(vector<string> &fileNames, const char* dir, bool returnCompletePath);
  double acos(double x);
};

#endif
