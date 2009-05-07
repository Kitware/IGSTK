/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCompressedDVVideoImagerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCompressedDVVideoImagerTool_h
#define __igstkCompressedDVVideoImagerTool_h

#include "igstkVideoImagerTool.h"

namespace igstk
{

class CompressedDVVideoImager;

/** \class CompressedDVVideoImagerTool
  * \brief A CompressedDV -specific VideoImagerTool class.
  *
  * This class provides CompressedDV  -specific functionality
  * for VideoImagerTools.
  *
  * \ingroup VideoImager
  *
  */

class CompressedDVVideoImagerTool : public VideoImagerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CompressedDVVideoImagerTool, VideoImagerTool )

  /** Get VideoImager tool name */
  igstkGetStringMacro( VideoImagerToolName );

  /** Set imager tool VideoImagerTool name */
  void RequestSetVideoImagerToolName( const std::string &);

protected:

  CompressedDVVideoImagerTool();
  virtual ~CompressedDVVideoImagerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  CompressedDVVideoImagerTool(const Self&);    //purposely not implemented
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

#endif  // __igstk_CompressedDVVideoImagerTool_h_
