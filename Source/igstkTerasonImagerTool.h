/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTerasonImagerTool.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTerasonImagerTool_h
#define __igstkTerasonImagerTool_h

#include "igstkImagerTool.h"


namespace igstk
{

class TerasonImager;

/** \class TerasonImagerTool
  * \brief A TerasonImager-specific ImagerTool class.
  *
  * This class is a for providing TerasonImager-specific functionality
  * for ImagerTools, and also to allow compile-time type enforcement
  * for other classes and functions that specifically require
  * an TerasonImagerTool.
  *
  * \ingroup Imager
  *
  */

class TerasonImagerTool : public ImagerTool
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TerasonImagerTool, ImagerTool )

  /** Get Imager tool name */
  igstkGetStringMacro( ImagerToolName );

  /** Set tracker tool ImagerTool name */
  void RequestSetImagerToolName( const std::string &); 

protected:

  TerasonImagerTool();
  virtual ~TerasonImagerTool();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

private:
  TerasonImagerTool(const Self&);   //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ImagerToolNameSpecified );

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidImagerToolName );
  igstkDeclareInputMacro( InValidImagerToolName );

  /** Get boolean variable to check if the tracker tool is 
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


#endif  // __igstk_TerasonImagerTool_h_
