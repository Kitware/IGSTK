/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkWebcamWinVideoImagerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkWebcamWinVideoImagerTool_h
#define __igstkWebcamWinVideoImagerTool_h

#include "igstkVideoImagerTool.h"

namespace igstk
{

class WebcamWinVideoImager;

/** \class WebcamWinVideoImagerTool
  * \brief A WebcamWin -specific VideoImagerTool class.
  *
  * This class provides WebcamWin  -specific functionality
  * for VideoImagerTools.
  *
  * \ingroup VideoImager
  *
  */

class WebcamWinVideoImagerTool : public VideoImagerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( WebcamWinVideoImagerTool, VideoImagerTool )

  /** Get VideoImager tool name */
  igstkGetStringMacro( VideoImagerToolName );

  /** Set imager tool VideoImagerTool name */
  void RequestSetVideoImagerToolName( const std::string &);

protected:

  WebcamWinVideoImagerTool();
  virtual ~WebcamWinVideoImagerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  WebcamWinVideoImagerTool(const Self&);    //purposely not implemented
  void operator=(const Self&);          //purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( VideoImagerToolNameSpecified );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidVideoImagerToolName );
  igstkDeclareInputMacro( InValidVideoImagerToolName );

  /** Get boolean variable to check if the imager tool is
   * configured or not */
  virtual bool CheckIfVideoImagerToolIsConfigured() const;

  /** Report Invalid VideoImagerTool name specified*/
  void ReportInvalidVideoImagerToolNameSpecifiedProcessing( );

  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Set VideoImagerTool name */
  void SetVideoImagerToolNameProcessing();

  std::string     m_VideoImagerToolName;
  std::string     m_VideoImagerToolNameToBeSet;

  bool            m_VideoImagerToolConfigured;

};

} // namespace igstk

#endif  // __igstk_WebcamWinVideoImagerTool_h_
