/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationTokens.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicationTokens_h
#define __igstkSerialCommunicationTokens_h

#include "igstkToken.h"

namespace igstk
{

/** \class SerialCommunicationBaudRate
 * 
 * \brief Defines a compile-time token for every baud rate.
 *
 * This class enforces safety on the values that are admisible as baud rates
 * for the serial port configuration. The actual Baud Rate value is assigned at
 * construction time and cannot be changed.
 *
 * \ingroup Communication
 */
class SerialCommunicationBaudRate : public Token
{
  public:

    typedef unsigned long int BaudRateType;

    typedef SerialCommunicationBaudRate Self;

  public:
    SerialCommunicationBaudRate( BaudRateType rate )
      {
      m_BaudRate = rate;  
      }

    ~SerialCommunicationBaudRate() 
      {}
    
    BaudRateType Get() const
      { 
      return m_BaudRate; 
      }
    
   const Self & operator=( const Self & token )
      { m_BaudRate = token.m_BaudRate; } 

   bool operator!=( const Self & token )
      { return m_BaudRate != token.m_BaudRate; }
        
   bool operator==( const Self & token )
      { return m_BaudRate == token.m_BaudRate; }
        
  private:
    
    BaudRateType m_BaudRate;

};

template< unsigned long NRate >
class SerialCommunicationBaudRateValued : public SerialCommunicationBaudRate
{
  public:
    SerialCommunicationBaudRateValued<NRate>():SerialCommunicationBaudRate(NRate) {};
};






/** \class SerialCommunicationDataBitsSize
 * 
 * \brief Defines a compile-time token for every size of data bits
 *
 * This class enforces safety on the values that are admisible as data bits size
 * for the serial port configuration. The actual data size value is assigned at
 * construction time and cannot be changed.
 *
 * \ingroup Communication
 */
class SerialCommunicationDataBitsSize : public Token
{
  public:

    typedef unsigned int DataBitsSizeType;

    typedef SerialCommunicationDataBitsSize Self;

  public:
    SerialCommunicationDataBitsSize( DataBitsSizeType rate )
      {
      m_DataBitsSize = rate;  
      }

    ~SerialCommunicationDataBitsSize() 
      {}
    
    DataBitsSizeType Get() const
      { 
      return m_DataBitsSize; 
      }
    
   const Self & operator=( const Self & token )
      { m_DataBitsSize = token.m_DataBitsSize; } 

   bool operator!=( const Self & token )
      { return m_DataBitsSize != token.m_DataBitsSize; }
        
   bool operator==( const Self & token )
      { return m_DataBitsSize == token.m_DataBitsSize; }
        
  private:
    
    DataBitsSizeType  m_DataBitsSize;

};

template< unsigned int NDataSize >
class SerialCommunicationDataBitsSizeValued : public SerialCommunicationDataBitsSize
{
  public:
    SerialCommunicationDataBitsSizeValued< NDataSize >():SerialCommunicationDataBitsSize(NDataSize) {};
};



/** \class SerialCommunicationParity
 * 
 * \brief Defines a compile-time token for the parity
 *
 * This class enforces safety on the values that are admisible as parity for
 * the serial port configuration. The actual parity value is assigned at
 * construction time and cannot be changed.
 *
 * \ingroup Communication
 */
class SerialCommunicationParity : public Token
{
  public:

    typedef unsigned int ParityType;

    typedef SerialCommunicationParity Self;

  public:
    SerialCommunicationParity( ParityType parity )
      {
      m_Parity = parity;  
      }

    ~SerialCommunicationParity() 
      {}
    
    ParityType Get() const
      { 
      return m_Parity; 
      }
    
   const Self & operator=( const Self & token )
      { m_Parity = token.m_Parity; } 

   bool operator!=( const Self & token )
      { return m_Parity != token.m_Parity; }
        
   bool operator==( const Self & token )
      { return m_Parity == token.m_Parity; }
        
  private:
    
    ParityType  m_Parity;

};

template< unsigned int NParity >
class SerialCommunicationParityValued : public SerialCommunicationParity
{
  public:
    SerialCommunicationParityValued< NParity >():SerialCommunicationParity(NParity) {};
};


} // end namespace igstk

#endif // __igstkSerialCommunicationBaudRate_h

