/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkRealtimeClock.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkRealtimeClock_h
#define __itkRealtimeClock_h

#include <itkMacro.h>
#include <itkObject.h>
#include <itkObjectFactory.h>

namespace itk
{

/** \class RealtimeClock
* \brief The RealtimeClock provides a timestamp from a real-time clock
*
* This class represents a real-time clock object 
* and provides a timestamp in platform-independent format.
*
*/

class RealtimeClock : public itk::Object
{
  
public:
  
  typedef RealtimeClock Self;
  typedef itk::Object Superclass;
  typedef itk::SmartPointer< Self > Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Method for defining the name of the class */
  itkTypeMacro(RealtimeClock, Object);

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Returns a timestamp in seconds   e.g. 52.341243 seconds */
  double GetTimestamp() const;

  /** Returns the frequency of a clock */
  itkGetConstMacro(Frequency, double);

protected:

  /** constructor */
  RealtimeClock();

  /** destructor */
  virtual ~RealtimeClock();

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:
  
  double m_Frequency;

};

} // end of namespace itk


#endif  // __itkRealtimeClock_h
