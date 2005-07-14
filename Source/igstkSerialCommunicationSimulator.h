/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationSimulator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicationSimulator_h
#define __igstkSerialCommunicationSimulator_h

#include <fstream>
#include <iomanip>
#include <map>
#include <string>

#include "igstkSerialCommunication.h"

namespace igstk
{

/** \class SerialCommunicationSimulator
 * 
 * \brief This class simulates serial communication via a file.
 * \ingroup Communication
 * \ingroup SerialCommunication
 */

class SerialCommunicationSimulator : public SerialCommunication
{
public:

  typedef std::ifstream   FileType;

  typedef SerialCommunicationSimulator  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(SerialCommunicationSimulator, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);

  /** Set a file name of the input file recorded serial communication stream */
  void SetFileName(const char* filename);

protected:

  typedef SerialCommunication::ResultType ResultType;

  SerialCommunicationSimulator();

  virtual ~SerialCommunicationSimulator();

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenCommunication( void );

  /** Set up data buffer size. */
  virtual ResultType InternalSetUpDataBuffers( void );

  /** Sets up communication on the open port as per the communication parameters. */
  virtual ResultType InternalSetTransferParameters( void );

  /** Closes serial port  */
  virtual ResultType InternalClosePort( void );

  /** Clear buffers and close port */
  virtual ResultType InternalClearBuffersAndClosePort( void );

  /** Send break and restarting
  transmission after a short delay.*/
  virtual void InternalSendBreak( void );

  /** Set the amount of time to wait on a reply from the device before generating a timeout event. */
  virtual ResultType InternalSetTimeoutPeriod( int milliseconds );

  /** Flush output buffer */
  virtual void InternalFlushOutputBuffer( void );

  /** Write data */
  virtual void InternalWrite( void );

  /** Read data */
  virtual void InternalRead( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** The mapping table for the request and response */
  typedef std::map<std::string, std::string> ResponseTableType;

private:

  FileType      m_File;

  std::string        m_FileName;

  ResponseTableType   m_ResponseTable;
};

} // end namespace igstk

#endif // __igstkSerialCommunicationSimulator_h
