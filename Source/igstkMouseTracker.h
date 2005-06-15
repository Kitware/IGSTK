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
    \brief Implementation of the Mouse TrackerTool class.

*/
class MouseTracker : public igstk::Tracker
{
public:
    typedef MouseTracker                        Self;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;
    typedef Tracker                             Superclass;
    typedef Superclass::TransformType           TransformType;

    /** Method for creation of a reference counted object. */
    igstkNewMacro( MouseTracker );  

//    void Initialize( const char *fileName = NULL );

    void GetTransform(TransformType & transform);

    typedef igstk::TrackerTool                  TrackerToolType;
    typedef igstk::TrackerPort                  TrackerPortType;

    typedef Tracker::ResultType                 ResultType;

    igstkSetMacro( ScaleFactor, double );
    igstkGetMacro( ScaleFactor, double );

protected:

    MouseTracker();

    virtual ~MouseTracker();

    virtual ResultType InternalOpen( void );

    virtual ResultType InternalActivateTools( void );

    virtual ResultType InternalStartTracking( void );

    virtual ResultType InternalUpdateStatus( void );

    virtual ResultType InternalReset( void );

    virtual ResultType InternalStopTracking( void );

    virtual ResultType InternalDeactivateTools( void );

    virtual ResultType InternalClose( void );

    /** Print object information */
    virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

    typedef Transform::TimePeriodType   TimePeriodType;

    TimePeriodType                      m_ValidityTime;

    igstk::TrackerTool::Pointer         m_Tool;

    igstk::TrackerPort::Pointer         m_Port;

    double                              m_ScaleFactor;
};

}

#endif //__igstk_MouseTracker_h_
