/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkWebcamWinImagerConfiguration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkWebcamWinImagerConfiguration_h
#define __igstkWebcamWinImagerConfiguration_h

#include "igstkImagerConfiguration.h"

namespace igstk
{


/**
 * \class WebcamWinToolConfiguration
 * A WebcamWin imager tool.
 */
class WebcamWinToolConfiguration : public ImagerToolConfiguration
{
public:
  WebcamWinToolConfiguration();
  WebcamWinToolConfiguration(const WebcamWinToolConfiguration &other);
  virtual ~WebcamWinToolConfiguration();

  virtual std::string GetToolTypeAsString();

protected:

};


/**
 * \class WebcamWinImagerConfiguration
 * Configuration for the WebcamWin imager.
 */
class WebcamWinImagerConfiguration : public ImagerConfiguration
{
public:
  WebcamWinImagerConfiguration();
  virtual ~WebcamWinImagerConfiguration();

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
