/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFrame.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkFrame_h
#define __igstkFrame_h

#include "itkVector.h"
#include "itkVersor.h"

#include "igstkTimeStamp.h"
#include "igstkMacros.h"
#include "itkStdStreamLogOutput.h"

class vtkImageData;

namespace igstk
{

/** \class Frame
 *  \brief Frame from an external input device.
 *
 * It is intended to be used as a
 * communication vehicle between imagers and spatial objects. The class
 * contains a TimeStamp defining the validity period for the information in the
 * frame.
 *
 * All the set methods require an argument that defines the number of
 * milliseconds for which the stored information is considered to be valid.
 * The validity period will be counted from the moment the Set method was
 * invoked.
 *
 * \sa TimeStamp
 *
 * */
class Frame
{
public:

  friend class VideoImager;
  friend class VideoImagerTool;

  igstkLoggerMacro();

  typedef TimeStamp::TimePeriodType   TimePeriodType;

  igstkSetMacro( Width, unsigned int );
  igstkGetMacro( Width, unsigned int );

  igstkSetMacro( Height, unsigned int );
  igstkGetMacro( Height, unsigned int );

  igstkSetMacro( NumberOfChannels, unsigned int );
  igstkGetMacro( NumberOfChannels, unsigned int );

  /** Constructor and destructor */
  Frame();
  Frame(unsigned int width, unsigned int height, unsigned int channels);
  Frame( const Frame & t );
  virtual ~Frame();

  void * GetImagePtr();

  /** Returns the time at which the validity of this information starts.
   * The data in this frame should not be used for scenes to be rendered
   * before that validity time. The time is returned in milliseconds.
   *
   * \sa TimeStamp
   *
   * */
  TimePeriodType GetStartTime() const;


  /** Returns the time at which the validity of this information expires. The
   * data in this frame should not be used for scenes to be rendered
   * after that validity time. The time is returned in milliseconds.
   *
   * \sa TimeStamp
   *
   * */
  TimePeriodType GetExpirationTime() const;

  void SetTimeToExpiration( TimePeriodType millisecondsToExpiration );

  /** Returns the validity status of the frame at the time passed as
   * argument. The frame values should not be used in a scene if the time
   * when the scene is to be rendered returned 'false' when passed to this
   * IsValid() function. The time is passed in milliseconds.
   *
   * \sa TimeStamp
   *
   * */
  bool IsValidAtTime( TimePeriodType timeToTestInMilliseconds ) const;

  /** Returns the validity status of the frame when it is called
   *
   * \sa TimeStamp
   *
   * */
  bool IsValidNow() const;

  /** Method for printing the member variables of this class to an ostream */
  void Print(std::ostream& os, itk::Indent indent) const;

protected:

  void PrintHeader(std::ostream& os, itk::Indent indent) const;

  void PrintTrailer(std::ostream& itkNotUsed(os),
                                          itk::Indent itkNotUsed(indent)) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  void SetFrameDimensions( unsigned int, unsigned int, unsigned int);
  void SetImagePtr( void*, TimePeriodType millisecondsToExpiration);

  std::vector< unsigned char >* m_Image;
  TimeStamp                     m_TimeStamp;
  void*                         m_ImagePtr;
  unsigned int                  m_Width;
  unsigned int                  m_Height;
  unsigned int                  m_NumberOfChannels;

};

std::ostream& operator<<(std::ostream& os, const igstk::Frame& o);
}

#endif
