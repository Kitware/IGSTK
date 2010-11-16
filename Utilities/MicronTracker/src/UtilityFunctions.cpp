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

#ifndef WIN32 
#include <unistd.h>
#endif
#include "UtilityFunctions.h"


/****************************/
/** Copies the current path in the passed argument (currPath). */
void mtUtils::getCurrPath(char* currPath)
{
  const short buffer = 255;
  char currDir[255];

  if( getcwd(currDir, buffer) == NULL )
    strcpy(currPath, "ERROR");
  else
    strcpy(currPath,currDir);
}

/****************************/
// This function assumes the passed dir argument (the passed directory) resides on the current directory.
// It just finds the file names, not the subdirectories
void mtUtils::getFileNamesFromDirectory(vector<string> &fileNames, const char* dir, bool returnCompletePath)
{
  char currentFolderPath[255];
  getCurrPath(currentFolderPath);
  dirent** list;
  if ( dir != NULL )
  {
#ifdef WIN32
    strcat(currentFolderPath, "\\");
    strcat(currentFolderPath, dir);
    strcat(currentFolderPath, "\\");
#else
    strcat(currentFolderPath, "/");
    strcat(currentFolderPath, dir);
    strcat(currentFolderPath, "/");
#endif
  }

  unsigned int num = fl_filename_list(currentFolderPath, &list);
  
  for (unsigned int i=0; i<num; i++)
  {
    string fileName = currentFolderPath;
    // Don't add the subdirectories to the vector
    if (!fl_filename_isdir(list[i]->d_name))
      returnCompletePath ? fileNames.push_back(fileName + list[i]->d_name) : fileNames.push_back(list[i]->d_name);
  }
}

/****************************/
/** */
double mtUtils::acos(double x)
{
  if (x==1)
    return 0;
  else if (x==-1)
    return PI / 2.0;
  else 
    return atan((double) ( -x / sqrt(-x * x + 1)) + 2 * atan(1.0));
}
