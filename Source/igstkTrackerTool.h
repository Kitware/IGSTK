
/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_TrackerTool_h_
#define __igstk_TrackerTool_h_

#include "itkVersor.h"


#include "igstkMacros.h"

namespace igstk
{

/** \class TrackerTool
    \brief Generic implementation of the Tracker tool.

    This class provides a generic implementation of a tool of
    a tracker. This may contain hardware specific details of 
    the tool, along with the fields for position, orientation
    and error associated with the measurement used.
*/

class TrackerTool
{
    typedef itk::Point< double, 3 >  PositionType;
    typedef itk::Versor<double>      OrientationType;
    typedef double                   ErrorType;

public:
    TrackerTool(void);

    ~TrackerTool(void);

    GetMacro( Position, PositionType );
    GetMacro( Orientation, OrientationType );
    GetMacro( Error, ErrorType );

    SetMacro( Position, PositionType );
    SetMacro( Orientation, OrientationType );
    SetMacro( Error, ErrorType );


protected:

    /** Position of the tool */
    PositionType       m_Position;

    /** Orientation of the tool */
    OrientationType    m_Orientation;

    /** Error reported in tool's position/orientation computation by the tracker */
    double             m_Error;
};

}

#endif //__igstk_TrackerTool_h_
