/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVTKLoggerOutputTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>
#include "itkStdStreamLogOutput.h"
#include "itkLogger.h"
#include "igstkVTKLoggerOutput.h"


int igstkVTKLoggerOutputTest( int, char * [] )
{
  try
    {

    // Create an ITK StdStreamLogOutputs
    itk::StdStreamLogOutput::Pointer coutput = itk::StdStreamLogOutput::New();
    itk::StdStreamLogOutput::Pointer foutput = itk::StdStreamLogOutput::New();
    coutput->SetStream(std::cout);
    std::ofstream fout("test_VTKLoggerOutput.txt");
    foutput->SetStream(fout);

    // Create an ITK Logger
    itk::Logger::Pointer logger = itk::Logger::New();

    std::cout << "Testing igstk::VTKLoggerOutput" << std::endl;

    // Setting the logger
    logger->SetName("org.itk.rootLogger");
    logger->SetPriorityLevel(itk::Logger::INFO);
    logger->SetLevelForFlushing(itk::Logger::CRITICAL);

    std::cout << "  Adding console and file stream LogOutputs" << std::endl;
    logger->AddLogOutput(coutput);
    logger->AddLogOutput(foutput);

    // Printing the logger's member variables
    std::cout << "  Name: " << logger->GetName() << std::endl;
    std::cout << "  Priority Level: " << logger->GetPriorityLevel() << std::endl;
    std::cout << "  Level For Flushing: " << logger->GetLevelForFlushing() << std::endl;

    // Create an igstk::VTKLoggerOutput and then test it.
    igstk::VTKLoggerOutput* pOver = igstk::VTKLoggerOutput::New();
    pOver->OverrideVTKWindow();
    pOver->SetLogger(logger);  // redirect messages from ITK OutputWindow -> logger2

    // test message for ITK OutputWindow
    vtkOutputWindow::GetInstance()->DisplayText("** This is from vtkOutputWindow **\n");
    vtkOutputWindow::GetInstance()->DisplayDebugText("** This is from vtkOutputWindow **\n");
    vtkOutputWindow::GetInstance()->DisplayWarningText("** This is from vtkOutputWindow **\n");
    vtkOutputWindow::GetInstance()->DisplayErrorText("** This is from vtkOutputWindow **\n");
    vtkOutputWindow::GetInstance()->DisplayGenericWarningText("** This is from vtkOutputWindow **\n");

    std::cout << *pOver << std::endl;
    }
  catch(...)
    {
    std::cerr << "Exception catched !!" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}


