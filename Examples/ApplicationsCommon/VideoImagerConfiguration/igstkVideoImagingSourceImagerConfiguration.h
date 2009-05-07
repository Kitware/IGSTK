/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkVideoImagingSourceImagerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImagingSourceVideoImagerConfiguration_h
#define __igstkImagingSourceVideoImagerConfiguration_h

#include "igstkVideoImagerConfiguration.h"

namespace igstk
{


/**
 * \class ImagingSourceToolConfiguration
 * A ImagingSource VideoImager tool.
 */
class ImagingSourceToolConfiguration : public VideoImagerToolConfiguration
{
public:
  ImagingSourceToolConfiguration();
  ImagingSourceToolConfiguration(const ImagingSourceToolConfiguration &other);
  virtual ~ImagingSourceToolConfiguration();

  virtual std::string GetToolTypeAsString();

protected:

};


/**
 * \class ImagingSourceVideoImagerConfiguration
 * Configuration for the ImagingSource VideoImager.
 */
class ImagingSourceVideoImagerConfiguration : public VideoImagerConfiguration
{
public:
  ImagingSourceVideoImagerConfiguration();
  virtual ~ImagingSourceVideoImagerConfiguration();

  igstkSetMacro( BufferSize, unsigned int );
  igstkGetMacro( BufferSize, unsigned int );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:
  virtual void InternalAddTool( const VideoImagerToolConfiguration *tool );

private:

  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;

  unsigned int m_BufferSize;


};


} // end of name space
#endif
