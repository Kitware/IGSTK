/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTrackerToolNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkFlockOfBirdsTrackerToolNew_h
#define __igstkFlockOfBirdsTrackerToolNew_h

#include "igstkTrackerTool.h"

namespace igstk
{
/** \class FlockOfBirdsTrackerToolNew
 *
 * \brief A FlockOfBirds-specific TrackerTool class.
 *
 * This class is a for providing FlockOfBirds-specific functionality
 * for TrackerTools, and also to allow compile-time type enforcement
 * for other classes and functions that specifically require
 * an FlockOfBirds tool.
 *
 * \ingroup Tracker
 *
 */

class FlockOfBirdsTrackerToolNew : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( FlockOfBirdsTrackerToolNew, TrackerTool )

  /** Get tracker tool port number */
  igstkGetMacro( PortNumber, unsigned int );

  /** Request setting the port number. The tool can only be attached to a
   * tracker after its port number has been defined. Valid port numbers are
   * in [1,4]. */
  void RequestSetPortNumber( unsigned int portNumber );

protected:

  FlockOfBirdsTrackerToolNew();
  ~FlockOfBirdsTrackerToolNew();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:

  FlockOfBirdsTrackerToolNew(const Self&);    //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( PortNumberSpecified );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidPortNumber );
  igstkDeclareInputMacro( InValidPortNumber );

  /** Get boolean variable to check if the tracker tool is 
  * configured or not */
  virtual bool CheckIfTrackerToolIsConfigured() const;

  /** Report Invalid bird name specified*/ 
  void ReportInvalidPortNumberSpecifiedProcessing( );

  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Set bird name */
  void SetPortNumberProcessing();

  unsigned int    m_PortNumber;
  unsigned int    m_PortNumberToBeSet;

  bool            m_TrackerToolConfigured;

};  


} // namespace igstk


#endif  // __igstk_FlockOfBirdsTrackerToolNew_h_
