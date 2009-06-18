/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFrame.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkFrame.h"
#include <algorithm>
#include "vtkImageData.h"

namespace igstk
{

Frame
::Frame()
{
  /** Setup logger */
  m_Logger   = LoggerType::New();
  this->GetLogger()->SetTimeStampFormat( itk::LoggerBase::HUMANREADABLE );
  this->GetLogger()->SetHumanReadableFormat("%Y %b %d, %H:%M:%S");
  this->GetLogger()->SetPriorityLevel( LoggerType::DEBUG);

  /** Direct the application log message to the std::cout */
  itk::StdStreamLogOutput::Pointer m_LogCoutOutput
                                           = itk::StdStreamLogOutput::New();
  m_LogCoutOutput->SetStream( std::cout );
  this->GetLogger()->AddLogOutput( m_LogCoutOutput );
}

Frame
::Frame( const Frame & inputFrame  )
: m_TimeStamp(inputFrame.m_TimeStamp)
{
  m_ImagePtr = inputFrame.m_ImagePtr;
}

Frame
::~Frame()
{
}

void
Frame::SetFrameDimensions(unsigned int width,
                          unsigned int height, unsigned int channels)
{
  m_Width = width;
  m_Height = height;
  m_NumberOfChannels = channels;

  try
    {
    m_ImagePtr = new unsigned char[m_Width * m_Height * m_NumberOfChannels];
    if (m_ImagePtr == NULL)
      {
      igstkLogMacro( FATAL, "igstk::Frame::SetFrameDimensions: "
                      << "Memory could not be allocated (malloc failed)!\n" );
      }
    }
  catch( std::exception& e )
    {
      igstkLogMacro( FATAL, "igstk::Frame::SetFrameDimensions: "
             << "Memory could not be allocated (malloc failed)!\n" << e.what());
    }
}

Frame::TimePeriodType
Frame
::GetStartTime() const
{
  return m_TimeStamp.GetStartTime();
}

Frame::TimePeriodType
Frame
::GetExpirationTime() const
{
  return m_TimeStamp.GetExpirationTime();
}

void
Frame
::SetImagePtr(void * imagePtr, TimePeriodType millisecondsToExpiration )
{
  this->m_ImagePtr = imagePtr;
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
}

void*
Frame
::GetImagePtr()
{
  return this->m_ImagePtr;
}

void
Frame
::SetTimeToExpiration( TimePeriodType millisecondsToExpiration )
{
  m_TimeStamp.SetStartTimeNowAndExpireAfter( millisecondsToExpiration );
}

bool
Frame
::IsValidAtTime( TimePeriodType timeToCheckInMilliseconds ) const
{
  return m_TimeStamp.IsValidAtTime( timeToCheckInMilliseconds );
}

bool
Frame
::IsValidNow() const
{
  return m_TimeStamp.IsValidNow();
}

void
Frame
::Print(std::ostream& os, itk::Indent indent) const
{
  this->PrintHeader( os, indent );
  this->PrintSelf( os, indent.GetNextIndent() );
  this->PrintTrailer( os, indent );
}

/**
 * Define a default print header for all objects.
 */
void
Frame
::PrintHeader(std::ostream& os, itk::Indent indent) const
{
  os << indent << "Frame" << " (" << this << ")\n";
}

/**
 * Define a default print trailer for all objects.
 */
void
Frame
::PrintTrailer(std::ostream& itkNotUsed(os),
                                           itk::Indent itkNotUsed(indent)) const
{
}

/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<(std::ostream& os, const Frame& o)
{
  o.Print(os, 0);
  return os;
}

/** Print object information */
void Frame::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

  this->m_TimeStamp.Print( os, indent );
  os << indent << this->m_ImagePtr << std::endl;
}

} // end namespace igstk
