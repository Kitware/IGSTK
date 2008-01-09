/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformSocketListenerTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkSocketCommunicator.h"
#include "vtkSocketController.h"


int igstkTransformSocketListenerTest(int argc, char * argv[])
{
  if( argc < 3 )
    {
    std::cerr << "Usage : " << std::endl;
    std::cerr << argv[0] << " portNumber numberOfTransformsExpected" << std::endl;
    return EXIT_FAILURE;
    }

  const int scMsgLength = 12; // 9 components of the rotation matrix, 3 translation components.

  vtkSocketController* contr = vtkSocketController::New();
  contr->Initialize();

  vtkSocketCommunicator* comm = vtkSocketCommunicator::New();


  int port = atoi( argv[1] );
  std::cout << " Port = " << port << std::endl;

  // Establish connection
  if (!comm->WaitForConnection(port))
    {
    cerr << "Client error: Could not connect to the server." << endl;
    comm->Delete();
    contr->Delete();
    return EXIT_FAILURE;
    }

  // Test receiving some supported types of arrays
  double data[scMsgLength];

  const unsigned int numberOfTransformsExpected = atoi( argv[2] );

  for(unsigned int i=0; i<numberOfTransformsExpected; i++)
    {
    int tag = 17;
    if (!comm->Receive(data, scMsgLength, 1, tag))
      {
      cerr << "Client error: Error sending data." << endl;
      comm->Delete();
      contr->Delete();
      return EXIT_FAILURE;
      }
    std::cout << i << " : ";
    for(unsigned int k=0; k<scMsgLength; k++)
      {
      std::cout << data[k] << "  ";
      }
    std::cout << std::endl;
    char data2 = 1; 
    if (!comm->Send(&data2, 1, 1, tag))
    {
      cerr << "Client error: Error sending message." << endl;
    }

    }

    comm->Delete();
    contr->Delete();

  return EXIT_SUCCESS;
}
