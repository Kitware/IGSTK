/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCompressedDVImagerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCompressedDVImagerTool_h
#define __igstkCompressedDVImagerTool_h

#include "igstkImagerTool.h"

namespace igstk
{

class CompressedDVImager;

/** \class CompressedDVImagerTool
  * \brief A CompressedDV -specific ImagerTool class.
  *
  * This class provides CompressedDV  -specific functionality
  * for ImagerTools.
  *
  * \ingroup Imager
  *
  */

class CompressedDVImagerTool : public ImagerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CompressedDVImagerTool, ImagerTool )

  /** Get Imager tool name */
  igstkGetStringMacro( ImagerToolName );

  /** Set imager tool ImagerTool name */
  void RequestSetImagerToolName( const std::string &);

protected:

  CompressedDVImagerTool();
  virtual ~CompressedDVImagerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  CompressedDVImagerTool(const Self&);    //purposely not implemented
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

#endif  // __igstk_CompressedDVImagerTool_h_
