/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBinaryData.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkBinaryData_H
#define __igstkBinaryData_H


#include <itkObject.h>
#include <vector>

namespace igstk 
{
 
/** \class BinaryData 
 *
 * \brief BinaryData representing a binary data chunk.
 * 
 * This class represents a binary data chunk.
 * 
 * This class can be used for storing binary stream during communication.
 *
 **/

class BinaryData
{
public:

  /** Data container type definition */
  typedef std::vector<unsigned char>  ContainerType;
  
public:

  /** Constructor */
  BinaryData();

  /** Destructor */
  virtual ~BinaryData();

  /** SetSize method resizes/allocates memory */
  void SetSize(unsigned int size);

  /** GetSize method returns the size of data */
  unsigned int GetSize() const;

  /** Get a data container (returns constant) */
  const ContainerType& GetData() const { return this->m_data; }

  /** Get a data container */
  ContainerType& GetData() { return this->m_data; }

  /** Copy data from an array */
  void CopyFrom(unsigned char* inputBegin, unsigned int inputLength);

  /** Copy data into an array */
  void CopyTo(unsigned char* output) const;

  /** Assign the values of one BinaryData to another */
  const BinaryData & operator=( const BinaryData & inputBinaryData );

  /** operator== redefinition */
  bool operator==( const BinaryData & inputBinaryData ) const;

  /** operator!= redefinition */
  bool operator!=( const BinaryData & inputBinaryData ) const;

  /** operator< redefinition */
  bool operator<( const BinaryData & inputBinaryData ) const ;

  /** operator[] redefinition */
  unsigned char operator[]( const unsigned int index ) const;

  /** operator[] redefinition (returns reference) */
  unsigned char& operator[]( const unsigned int index );

  /** Method for printing the member variables of this class to an ostream */
  void Print(std::ostream& os, itk::Indent indent) const;

protected:

  /** Method for printing the header to an ostream */
  void PrintHeader(std::ostream& os, itk::Indent indent) const;

  /** Method for printing the trailer to an ostream */
  void PrintTrailer(std::ostream& itkNotUsed(os), itk::Indent itkNotUsed(indent)) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** data container */
  ContainerType   m_data;

};

/** operator<< redefinition */
std::ostream& operator<<(std::ostream& os, const igstk::BinaryData& o);
}



#endif

