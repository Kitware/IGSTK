/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationSimulator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicationSimulator_h
#define __igstkSerialCommunicationSimulator_h

#include <fstream>
#include <iomanip>
#include <map>
#include <vector>

#include "igstkBinaryData.h"
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

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( SerialCommunicationSimulator, 
                                 SerialCommunication )

public:

  /** The type for the file that holds simulation data. */
  typedef std::ifstream   FileType;

  /** Set a file name of the input file recorded serial communication stream */
  void SetFileName(const char* filename);

  /** Get the file name for the recorded data */
  const char *GetFileName() const;

protected:

  typedef SerialCommunication::ResultType ResultType;

  /** Constructor */
  SerialCommunicationSimulator();

  /** Destructor */
  ~SerialCommunicationSimulator();

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenPort( void );

  /** Sets up communication on the open port as per the communication
      parameters. */
  virtual ResultType InternalUpdateParameters( void );

  /** Closes serial port  */
  virtual ResultType InternalClosePort( void );

  /** Send a serial break */
  virtual ResultType InternalSendBreak( void );

  /** Sleep for the amount of time specified in milliseconds */
  virtual void InternalSleep( unsigned int milliseconds );

  /** Purge the input and output buffers */
  virtual ResultType InternalPurgeBuffers( void );

  /** Write data */
  virtual ResultType InternalWrite( const char *message,
                                    unsigned int numberOfBytes );

  /** Read data */
  virtual ResultType InternalRead( char *data, unsigned int numberOfBytes,
                                   unsigned int &bytesRead );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** The mapping table type definition for the request and response */
  typedef std::map<BinaryData, std::vector<BinaryData> > ResponseTableType;

  /** The response time type definition : used for response timing */
  typedef std::map<BinaryData, std::vector<double> > ResponseTimeType;

  /** The response counter type definition : counter is used to respond
   *  sequentially */
  typedef std::map<BinaryData, unsigned> ResponseCounterType;

  /** The file that holds the simulation data. */
  FileType  m_File;

  /** The name of the simulation data file. */
  std::string  m_FileName;

  /** A table that maps commands to responses. */
  ResponseTableType  m_ResponseTable;

  /** A table that maps commands to response times. */
  ResponseTimeType m_TimeTable;

  /** A table that maps commands to index number of responses. */
  ResponseCounterType m_CounterTable;

  /** The most recently sent command */
  BinaryData  m_Command;

};

} // end namespace igstk

#endif // __igstkSerialCommunicationSimulator_h
