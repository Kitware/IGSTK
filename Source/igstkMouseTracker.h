/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_MouseTracker_h_
#define __igstk_MouseTracker_h_

#include "igstkTracker.h"

namespace igstk
{
/** \class MouseTracker
    \brief Implementation of the Mouse Tracker class.

*/

class MouseTracker : public igstk::Tracker
{
public:
    typedef MouseTracker                        Self;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;
    typedef itk::Point< double, 3 >  PositionType;

    /** Method for creation of a reference counted object. */
    igstkNewMacro( MouseTracker );  

    void Initialize( const char *fileName = NULL );

    void GetPosition(PositionType& position);

protected:

    virtual void SetUpToolsProcessing( void );

    /** The "UpdateStatusProcessing" method updates mouse status. */
    virtual void UpdateStatusProcessing( void );

private:

};

}

#endif //__igstk_MouseTracker_h_
