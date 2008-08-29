/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerConfigurationComposer.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkTrackerConfigurationComposer_h
#define __igstkTrackerConfigurationComposer_h


#include "vtkXMLDataElement.h"
#include "vtkXMLDataParser.h"
#include "igstkTrackerConfiguration.h"


namespace igstk
{
class igstkTrackerConfigurationComposer 
{
public:
    igstkTrackerConfigurationComposer();
    virtual ~igstkTrackerConfigurationComposer();
    virtual bool BuildConfiguration() {return false;};
    void SetParser(vtkXMLDataParser *p) {this->Parser = p;};
    igstk::TrackerConfiguration *GetTrackerConfiguration() {return TrackerConfig;};    

protected:
    char *ReadElementValue(vtkXMLDataElement *element);
    char *ElementValue;
    igstk::TrackerConfiguration *TrackerConfig;    
    vtkXMLDataParser *Parser;

};

}


#endif
