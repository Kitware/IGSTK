/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkImagingSourceImagerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImagingSourceImagerConfiguration_h
#define __igstkImagingSourceImagerConfiguration_h

#include "igstkImagerConfiguration.h"

namespace igstk
{


/**
 * \class ImagingSourceToolConfiguration
 * A ImagingSource imager tool.
 */
class ImagingSourceToolConfiguration : public ImagerToolConfiguration
{
public:
  ImagingSourceToolConfiguration();
  ImagingSourceToolConfiguration(const ImagingSourceToolConfiguration &other);
  virtual ~ImagingSourceToolConfiguration();

  virtual std::string GetToolTypeAsString();

protected:

};


/**
 * \class ImagingSourceImagerConfiguration
 * Configuration for the ImagingSource imager.
 */
class ImagingSourceImagerConfiguration : public ImagerConfiguration
{
public:
  ImagingSourceImagerConfiguration();
  virtual ~ImagingSourceImagerConfiguration();

  igstkSetMacro( BufferSize, unsigned int );
  igstkGetMacro( BufferSize, unsigned int );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

protected:
  virtual void InternalAddTool( const ImagerToolConfiguration *tool );

private:

  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;

  unsigned int m_BufferSize;


};


} // end of name space
#endif
