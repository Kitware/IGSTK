/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerConfigurationComposer.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkAuroraTrackerConfigurationComposer_h
#define __igstkAuroraTrackerConfigurationComposer_h

#include "igstkTrackerConfigurationComposer.h"
#include "vtkXMLDataElement.h"

namespace igstk
{
class igstkAuroraTrackerConfigurationComposer : public igstk::igstkTrackerConfigurationComposer 
{
public:
    igstkAuroraTrackerConfigurationComposer();
    ~igstkAuroraTrackerConfigurationComposer();
    bool BuildConfiguration();
private:
    void ProcessSendToIp(vtkXMLDataElement *next);
    void ProcessCommunication(vtkXMLDataElement *next);
    void ProcessTool(vtkXMLDataElement *next);
};

}


#endif
