/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformXMLFileWriterBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <itksys/SystemTools.hxx>
#include <algorithm>
#include "igstkTransformXMLFileWriterBase.h"

namespace igstk
{

int 
TransformXMLFileWriterBase::CanWriteFile(const char* name)
{
  //check that we have a valid file name, it is not a directory
  if( itksys::SystemTools::FileIsDirectory( name ) )
    {
    return 0;
    }
  return 1;
}


int 
TransformXMLFileWriterBase::WriteFile()
{ //problem with the given file name 
  if( !CanWriteFile( this->m_Filename.c_str() ) )
    {
    return 0;
    }
  std::ofstream out;
  out.open( this->m_Filename.c_str() );
        //failed openning file
  if( out.fail() )
    {
    return 0;
    }
  out<<"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n\n\n\n";
  out<<"<precomputed_transform>\n\n";
  WriteDescription( out );
  WriteDate( out );
  WriteTransformation( out );
  out<<"</precomputed_transform>\n";

  out.close();
  return 1;
}


void 
TransformXMLFileWriterBase::WriteDescription( std::ofstream &out )
{
  TransformationDescriptionObserver::Pointer descriptionObserver = 
    TransformationDescriptionObserver::New();
  unsigned long observerID = this->m_InputObject->AddObserver(
    igstk::StringEvent(), descriptionObserver );
  this->m_InputObject->RequestTransformDescription();
  this->m_InputObject->RemoveObserver( observerID );

  if( descriptionObserver->GotTransformationDescription() )
    {
    out<<"\t<description>\n";
    out<<"\t"<<descriptionObserver->GetTransformationDescription()<<"\n";  
    out<<"\t</description>\n\n";
    }
}

void 
TransformXMLFileWriterBase::WriteDate( std::ofstream &out )
{
  TransformationDateObserver::Pointer dateObserver = 
    TransformationDateObserver::New();
    unsigned long observerID = this->m_InputObject->AddObserver( 
    igstk::PrecomputedTransformData::TransformDateTypeEvent(), 
    dateObserver );
  this->m_InputObject->RequestComputationDateAndTime();
  this->m_InputObject->RemoveObserver( observerID );

  if( dateObserver->GotTransformationDate() )
    {
    out<<"\t<computation_date>\n";
    out<<"\t"<<dateObserver->GetTransformationDate()<<"\n";  
    out<<"\t</computation_date>\n\n";
    }
}

} //namespace
