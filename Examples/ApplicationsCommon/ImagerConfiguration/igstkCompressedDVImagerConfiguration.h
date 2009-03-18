/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkCompressedDVImagerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCompressedDVImagerConfiguration_h
#define __igstkCompressedDVImagerConfiguration_h

#include "igstkImagerConfiguration.h"

namespace igstk
{


/**
 * A CompressedDV imager tool.
 */
class CompressedDVToolConfiguration : public ImagerToolConfiguration
{
public:
  CompressedDVToolConfiguration();
  CompressedDVToolConfiguration(const CompressedDVToolConfiguration &other);
  virtual ~CompressedDVToolConfiguration();

  virtual std::string GetToolTypeAsString();

protected:

};


/**
 * Configuration for the CompressedDV imager.
 */
class CompressedDVImagerConfiguration : public ImagerConfiguration//SocketCommunicatingImagerConfiguration
{
public:
  CompressedDVImagerConfiguration();
  virtual ~CompressedDVImagerConfiguration();

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
