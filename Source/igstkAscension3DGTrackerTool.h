/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscension3DGTrackerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAscension3DGTrackerTool_h
#define __igstkAscension3DGTrackerTool_h

#include "igstkTrackerTool.h"

namespace igstk
{
/** \class Ascension3DGTrackerTool
 *
 * \brief Ascension specific TrackerTool class for the 3D Guidance family of trackers.
 *
 * This class is a for providing Ascension-specific functionality
 * for TrackerTools, and also to allow compile-time type enforcement
 * for other classes and functions that specifically require
 * an Ascension tool.
 *
 * \ingroup Tracker
 *
 */

class Ascension3DGTrackerTool : public TrackerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( Ascension3DGTrackerTool, TrackerTool )

  /** Get tracker tool port number */
  igstkGetMacro( PortNumber, unsigned int );

  /** Request setting the port number. */
  void RequestSetPortNumber( unsigned int portNumber );

protected:

  Ascension3DGTrackerTool();
  ~Ascension3DGTrackerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:

  Ascension3DGTrackerTool(const Self&);    //purposely not implemented
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

  /** Report Invalid port number specified*/ 
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


#endif  // __igstk_Ascension3DGTrackerTool_h_
