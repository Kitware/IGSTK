/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMultipleOutput.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_MultipleOutput_h_
#define __igstk_MultipleOutput_h_


#include <iostream>
#include <fstream>
#include <set>

namespace igstk
{

/** \class MultipleOutput
    \brief Class MultipleOutput allows writing simultaneously to multiple
    streams.  Note that the class derives from std::streambuf and contains a
    std::set<> of std::ostream.
*/

class MultipleOutput : virtual public std::streambuf
{
private:
  
  typedef std::ostream               StreamType;
  typedef std::set< StreamType * >   ContainerType;

public:

  /** Constructor */
  MultipleOutput();

  /** Destructor */
  ~MultipleOutput();


  /** Register a additional output stream into the list of ostreams to write
   * to. The messages will be sent to the streams in the same order that the
   * streams have been added here.  */
  void AddOutputStream( StreamType & output );


  /** Broadcast a flush operation to all the output streams */
  void Flush();

  /** Operator that will receive different input types and will forward  them
   * to the multiple outputs. */
  template <class T>
  MultipleOutput& operator << ( T tt )
  {
    ContainerType::iterator itr = m_Output.begin();
    ContainerType::iterator end = m_Output.end();
    while( itr != end )
      {
      *(*itr) << tt ;
      ++itr;
      }
    return *this;
  }


private:
  
    ContainerType        m_Output;

};

}

#endif //__igstk_MultipleOutput_h_
