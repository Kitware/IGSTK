/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerConfigurationParser.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkTrackerConfigurationParser_h
#define __igstkTrackerConfigurationParser_h


#include "vtkXMLDataParser.h"
#include "igstkTrackerConfiguration.h"
#include "igstkTrackerConfigurationComposer.h"


class igstkTrackerConfigurationParser : public vtkXMLDataParser 
{
public:
    enum TRACKING_SYSTEM {
    POLARIS_VICRA = 0, 
    POLARIS_HYBRID = 1, 
    AURORA = 2, 
    MICRON = 3, 
    FLOCK_OF_BIRDS = 4
    };


    static igstkTrackerConfigurationParser* New ( );
    vtkTypeRevisionMacro (igstkTrackerConfigurationParser, vtkXMLDataParser);

    vtkGetObjectMacro(TrackerConfig, igstk::TrackerConfiguration);

    vtkGetStringMacro(HostName);
    vtkSetStringMacro(HostName);


    // True: Update successful
    // False: Update failed
    bool CreateConfiguration();

protected:
    igstk::TrackerConfiguration *TrackerConfig;    
    igstk::igstkTrackerConfigurationComposer *Composer;
    char *HostName;

    igstkTrackerConfigurationParser();
    virtual ~igstkTrackerConfigurationParser();


};



#endif
