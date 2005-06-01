/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTool.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkAuroraTool.h"

namespace igstk
{

AuroraTool::AuroraTool() : TrackerTool()
{
  this->ToolType = 0;
  this->ToolRevision = 0;
  this->ToolSerialNumber = 0;
  this->ToolPartNumber = 0;
  this->ToolManufacturer = 0;
}


/** Print Self function */
void AuroraTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->ToolType )
  {
    os << indent << "Tool type: " << this->ToolType << std::endl;
  }
  if( this->ToolRevision )
  {
    os << indent << "Tool revision: " << this->ToolRevision << std::endl;
  }
  if( this->ToolSerialNumber )
  {
    os << indent << "Tool serial number: " << this->ToolSerialNumber << std::endl;
  }
  if( this->ToolPartNumber )
  {
    os << indent << "Tool part number: " << this->ToolPartNumber << std::endl;
  }
  if( this->ToolManufacturer )
  {
    os << indent << "Tool manufacturer: " << this->ToolManufacturer << std::endl;
  }
}


}
