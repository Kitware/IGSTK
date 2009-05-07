/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkCompressedDVVideoImagerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCompressedDVVideoImagerConfiguration_h
#define __igstkCompressedDVVideoImagerConfiguration_h

#include "igstkVideoImagerConfiguration.h"

namespace igstk
{


/**
 * \class CompressedDVToolConfiguration
 * A CompressedDV VideoImager tool.
 */
class CompressedDVToolConfiguration : public VideoImagerToolConfiguration
{
public:
  CompressedDVToolConfiguration();
  CompressedDVToolConfiguration(const CompressedDVToolConfiguration &other);
  virtual ~CompressedDVToolConfiguration();

  virtual std::string GetToolTypeAsString();

protected:

};


/**
 * \class CompressedDVVideoImagerConfiguration
 * Configuration for the CompressedDV VideoImager.
 */
class CompressedDVVideoImagerConfiguration : public VideoImagerConfiguration
{
public:
  CompressedDVVideoImagerConfiguration();
  virtual ~CompressedDVVideoImagerConfiguration();

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
  unsigned int        m_BufferSize;
};


} // end of name space
#endif
