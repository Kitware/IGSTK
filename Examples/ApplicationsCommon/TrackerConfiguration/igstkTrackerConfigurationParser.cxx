/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerConfigurationParser.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


//#include <ctype.h>
#include "vtkObjectFactory.h"
#include "igstkTrackerConfigurationParser.h"
#include "igstkAuroraTrackerConfigurationComposer.h"


vtkStandardNewMacro (igstkTrackerConfigurationParser);
vtkCxxRevisionMacro (igstkTrackerConfigurationParser, "1.2" );

 
igstkTrackerConfigurationParser::igstkTrackerConfigurationParser()
{
    this->TrackerConfig = NULL;    
    this->Composer = NULL;
    this->HostName = NULL;
}


igstkTrackerConfigurationParser::~igstkTrackerConfigurationParser()
{
    istream *is = this->GetStream();
    if (is)
    {
        delete is;
    }

    if (this->Composer)
    {
        delete this->Composer; 
    }

    if (this->HostName)
    {
        delete this->HostName; 
    }
}



// True: Update successful
// False: Update failed
bool igstkTrackerConfigurationParser::CreateConfiguration()
{
    bool parsed = this->Parse();
    if (! parsed)
    {
        return false;
    }

    std::string value(this->GetRootElement()->GetAttributeValue(0));
    if (value.compare("aurora") == 0) // aurora system
    {
        this->Composer =  new igstk::igstkAuroraTrackerConfigurationComposer; 
    }

    this->Composer->SetParser(this);
    bool done = this->Composer->BuildConfiguration();
    if (! done)
    {
        this->TrackerConfig = NULL; 
        return false;
    }
    else
    {
        this->TrackerConfig = this->Composer->GetTrackerConfiguration();    
        this->SetHostName(this->Composer->GetHostName());
    }
 
    return true;
}

