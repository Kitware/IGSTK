/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerConfigurationComposer.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerConfigurationComposer.h"
#include <ctype.h>


igstk::igstkTrackerConfigurationComposer::igstkTrackerConfigurationComposer()
{
    this->ElementValue = NULL;    
    this->TrackerConfig = NULL;    
    this->Parser = NULL;    
}



igstk::igstkTrackerConfigurationComposer::~igstkTrackerConfigurationComposer()
{
    if (ElementValue)
    {
        delete [] this->ElementValue;
    }

    if (this->TrackerConfig)    
    {
        delete this->TrackerConfig;
    }
}



char *igstk::igstkTrackerConfigurationComposer::ReadElementValue(vtkXMLDataElement *element)
{
    if (! element)
    {
        cout << "igstkTrackerConfigurationComposer::ReadElementValue: element is NULL.\n";
        return NULL;
    }

    int num = element->GetNumberOfNestedElements();
    if (num > 0)
    {
        cout << "TigstkTrackerConfigurationComposer::ReadElementValue: element is NOT a leaf element.\n";
        return NULL;
    }
    else
    {
        istream *stream = this->Parser->GetStream();
        if (! stream)
        {
            ifstream *ifs = new ifstream;  
            ifs->open(this->Parser->GetFileName(), ios::in); 
            this->Parser->SetStream(ifs);
            stream = this->Parser->GetStream();
        } 

        // Scans for the start of the actual inline data.
        char c;
        stream->seekg(element->GetXMLByteIndex());
        stream->clear(stream->rdstate() & ~ios::eofbit);
        stream->clear(stream->rdstate() & ~ios::failbit);
        while(stream->get(c) && (c != '>')) ;
        while(stream->get(c) && isspace(c)) ;
        unsigned long pos = stream->tellg();

        // Value length in number of chars.
        stream->seekg(pos-1);
        int count = 0;
        while (stream->get(c) && (c != '<'))
        {
            count++;            
        }
        if (this->ElementValue != NULL)
        {
            delete [] this->ElementValue;
        }
        this->ElementValue = new char [count+2];
 
        // Reads value
        stream->seekg(pos-1);
        stream->get(this->ElementValue, count+1, '<');
        this->ElementValue[count+1] = '\0';
        return this->ElementValue;
    }
}
