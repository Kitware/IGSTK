/*=========================================================================

  Program:   Open IGT Link Library
  Module:    igstkOpenIGTLinkTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef WIN32
#define NOMINMAX
#ifdef USE_RAWINPUT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#endif
#endif

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <math.h>

#include <iostream>

#include "igtlMath.h"
#include "igtlOSUtil.h"
//#include "ScannerSim.h"

#include "AcquisitionTrackingSimulator.h"
#include "TransferOpenIGTLink.h"


void PrintUsage(const char* name)
{
    std::cerr << "Invalid arguments!" << std::endl;
    std::cerr << "Usage: " << name << "<fps> s [port]" << std::endl;
    std::cerr << "Usage: " << name << "<fps> c <hostname> [port]" << std::endl;
    std::cerr << "    <fps>           : Frame rate (frames per second)." << std::endl;
    std::cerr << "    <type>          : 's': server mode / 'c': client mode" << std::endl;
    std::cerr << "    <hostname>      : <Client mode only> hostname" << std::endl;
    std::cerr << "    <port>          : Port # (default: 18944)" << std::endl;
}


int igstkOpenIGTLinkTest( int argc, char * argv [] )
{
  float fps;
  bool  server;
  char* hostname;
  int   port = 18944;

  if (argc < 3)
    {
    PrintUsage(argv[0]);
    exit(-1);
    }

  // frame rate
  fps  = atof(argv[1]);

  // mode: server or client?
  if (strcmp(argv[2], "s") == 0)
    {
    server = true;
    }
  else if (strcmp(argv[2], "c") == 0)
    {
    server = false;
    }
  else
    {
    PrintUsage(argv[0]);
    exit(-1);
    }

  // hostname and port number
  if (server)
    {
    if (argc < 3 || argc > 4) // illegal number of arguments
      {
      PrintUsage(argv[0]);
      exit(-1);
      }
    if (argc == 4)
      {
      port = atoi(argv[3]);
      }
    }
  else // client mode
    {
    if (argc < 4 || argc > 5) // illegal number of arguments
      {
      PrintUsage(argv[0]);
      exit(-1);
      }
    hostname = argv[3];
    if (argc == 5)
      {
      port = atoi(argv[4]);
      }
    }

  std::cerr << "Creating new Tracker..." << std::endl;

  AcquisitionTrackingSimulator* acquisition = new AcquisitionTrackingSimulator;
  TransferOpenIGTLink*  transfer  = new TransferOpenIGTLink;

  acquisition->SetPostProcessThread(dynamic_cast<Thread*>(transfer));
  acquisition->SetFrameRate(fps);

  if (server)
    {
    transfer->SetServerMode(port);
    }
  else
    {
    transfer->SetClientMode(hostname, port);
    }

  transfer->SetAcquisitionThread(acquisition);
  acquisition->Run();
  transfer->Run();

  // run 100 sec
  igtl::Sleep(100*1000);

  acquisition->Stop();
  transfer->Stop();
  transfer->Disconnect();

}

