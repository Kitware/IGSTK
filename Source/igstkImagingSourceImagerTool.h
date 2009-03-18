/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagingSourceImagerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImagingSourceImagerTool_h
#define __igstkImagingSourceImagerTool_h

#include "igstkImagerTool.h"

namespace igstk
{

class ImagingSourceImager;

/** \class ImagingSourceImagerTool
  * \brief A ImagingSource -specific ImagerTool class.
  *
  * This class provides ImagingSource Converter -specific functionality
  * for ImagerTools.
  *
  * \ingroup Imager
  *
  */

class ImagingSourceImagerTool : public ImagerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ImagingSourceImagerTool, ImagerTool )

  /** Get Imager tool name */
  igstkGetStringMacro( ImagerToolName );

  /** Set imager tool ImagerTool name */
  void RequestSetImagerToolName( const std::string &);

protected:

  ImagingSourceImagerTool();
  virtual ~ImagingSourceImagerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  ImagingSourceImagerTool(const Self&);     //purposely not implemented
  void operator=(const Self&);          //purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ImagerToolNameSpecified );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidImagerToolName );
  igstkDeclareInputMacro( InValidImagerToolName );

  /** Get boolean variable to check if the imager tool is
   * configured or not */
  virtual bool CheckIfImagerToolIsConfigured() const;

  /** Report Invalid ImagerTool name specified*/
  void ReportInvalidImagerToolNameSpecifiedProcessing( );

  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Set ImagerTool name */
  void SetImagerToolNameProcessing();

  std::string     m_ImagerToolName;
  std::string     m_ImagerToolNameToBeSet;

  bool            m_ImagerToolConfigured;

};

} // namespace igstk

#endif  // __igstk_ImagingSourceImagerTool_h_
