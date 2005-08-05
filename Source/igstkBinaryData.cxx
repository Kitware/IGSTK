/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBinaryData.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkBinaryData.h"


namespace igstk
{

/** Constructor */
BinaryData
::BinaryData()
{
}


/** Destructor */
BinaryData
::~BinaryData()
{
}


/** SetSize method resizes/allocates memory */
void 
BinaryData
::SetSize(unsigned int size)
{
  this->m_data.resize(size);
}


/** GetSize method returns the size of data */
unsigned int
BinaryData
::GetSize() const
{
  return this->m_data.size();
}


/** Copy data from an array */
void
BinaryData
::CopyFrom(unsigned char* inputBegin, unsigned int inputLength)
{
  this->SetSize(inputLength);
  unsigned int i;
  for( i = 0; i < inputLength; ++i )
    {
    this->m_data[i] = inputBegin[i];
    }
}


/** Copy data into an array */
void
BinaryData
::CopyTo(unsigned char* output) const
{
  unsigned int i;
  unsigned int size = this->m_data.size();
  for( i = 0; i < this->m_data.size(); ++i )
    {
    output[i] = this->m_data[i];
    }
}


const BinaryData &
BinaryData
::operator=( const BinaryData & inputBinaryData )
{
  this->m_data = inputBinaryData.m_data;
  return *this;
}



bool
BinaryData
::operator==( const BinaryData & inputBinaryData ) const
{
  return (this->m_data == inputBinaryData.m_data);
}



bool
BinaryData
::operator!=( const BinaryData & inputBinaryData ) const
{
  return (this->m_data != inputBinaryData.m_data );
}



bool
BinaryData
::operator<( const BinaryData & inputBinaryData ) const
{
  unsigned int i;
  unsigned int size_left = this->m_data.size();
  unsigned int size_right = inputBinaryData.m_data.size();
  unsigned int size_min = (size_left < size_right)? size_left : size_right;

  for( i = 0; i < size_min; ++i )
    {
    if( this->m_data[i] < inputBinaryData.m_data[i] )
      {
      return true;
      }
    else if( this->m_data[i] > inputBinaryData.m_data[i] )
      {
      return false;
      }
    }
  if( size_left < size_right )
    {
    return true;
    }
  return false;
}


unsigned char
BinaryData
::operator[]( const unsigned int index ) const
{
  return this->m_data[index];
}


unsigned char&
BinaryData
::operator[]( const unsigned int index )
{
  return this->m_data[index];
}


void 
BinaryData
::Print(std::ostream& os, itk::Indent indent) const
{
  this->PrintHeader(os, indent); 
  this->PrintSelf(os, indent.GetNextIndent());
  this->PrintTrailer(os, indent);
}


/**
 * Define a default print header for all objects.
 */
void 
BinaryData
::PrintHeader(std::ostream& os, itk::Indent indent) const
{
  os << indent << "BinaryData" << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void 
BinaryData
::PrintTrailer(std::ostream& itkNotUsed(os), itk::Indent itkNotUsed(indent)) const
{
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<(std::ostream& os, const BinaryData& o)
{
  o.Print(os, 0);
  return os;
}


/** Print object information */
void BinaryData::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

  os << indent << "Data Size: " << this->GetSize() << std::endl;
  os << indent << "Data: " << std::endl;
  os << indent;
  unsigned int i;
  for( i = 0; i < this->m_data.size(); ++i )
    {
    if( this->m_data[i] >= 0x20  &&  this->m_data[i] <= 0x7E )
      {
      os << this->m_data[i];
      }
    else if( this->m_data[i] == '\\' )
      {
      os << "\\\\";
      }
    else
      {
      os << "\\x";
      os.width(2);
      os.fill('0');
      os << std::hex << (int)this->m_data[i];
      }
    }
  os << std::endl;
}


} // end namespace itk
