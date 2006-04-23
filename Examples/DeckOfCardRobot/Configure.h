/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    Configure.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/** Computer specific settings */

/** Serial port number the polaris connected to */
#define IGSTK_POLARIS_PORT_NUMBER  igstk::SerialCommunication::PortNumber2

/** Tracker tool's port number and its srom file. */
#define TRACKER_TOOL_PORT 3
#define TRACKER_TOOL_SROM_FILE "C:/Patrick/Vicra/Tookit/Tool Definition Files/8700340.rom"

/** Reference tool's port number and its srom file. */
#define USE_REFERENCE_TOOL 1
#define REFERENCE_TOOL_PORT 4
#define REFERENCE_TOOL_SROM_FILE "C:/Patrick/Vicra/Tookit/Tool Definition Files/8700339.rom"
