/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVersionTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

//
// Print out the versions of libraries on which IGSTK depends.
//
#include "igstkConfigure.h"

#ifdef FLTK_FOUND
#include "Fl/Fl.H"
#endif

#include "itkVersion.h"
#include "vtkVersion.h"


int main(int,char *[])
{
  // ITK
  // 
  std::cout << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << "ITK Version    = " 
            << itk::Version::GetITKVersion()       << std::endl;
  std::cout << "Major Version  = " 
            << itk::Version::GetITKMajorVersion()  << std::endl;
  std::cout << "Minor Version  = " 
            << itk::Version::GetITKMinorVersion()  << std::endl;
  std::cout << "Patch Version  = " 
            << itk::Version::GetITKBuildVersion()  << std::endl;
  std::cout << "Source Version = " 
            << itk::Version::GetITKSourceVersion() << std::endl;
  //
  // VTK
  // 
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << "VTK Version    = " 
            << vtkVersion::GetVTKVersion()       << std::endl;
  std::cout << "Major Version  = " 
            << vtkVersion::GetVTKMajorVersion()  << std::endl;
  std::cout << "Minor Version  = " 
            << vtkVersion::GetVTKMinorVersion()  << std::endl;
  std::cout << "Patch Version  = " 
            << vtkVersion::GetVTKBuildVersion()  << std::endl;
  std::cout << "Source Version = " 
            << vtkVersion::GetVTKSourceVersion() << std::endl;
  std::cout << std::endl;

#ifdef FLTK_FOUND  
  //
  // FLTK
  // 
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << "FLTK Version    = " << Fl::version() << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << std::endl;
#endif  

  return 0;

}
