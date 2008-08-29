/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerConfigurationComposer.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkAuroraTrackerConfigurationComposer.h"
#include "igstkAuroraTrackerConfiguration.h"
 

igstk::igstkAuroraTrackerConfigurationComposer::igstkAuroraTrackerConfigurationComposer()
{
}



igstk::igstkAuroraTrackerConfigurationComposer::~igstkAuroraTrackerConfigurationComposer()
{
    if (this->TrackerConfig)
    {
        delete this->TrackerConfig;
    }
}



bool igstk::igstkAuroraTrackerConfigurationComposer::BuildConfiguration()
{
    if (! this->Parser)
    {
        cerr << "XML parser is NOT available.\n";
        return false;
    }

    vtkXMLDataElement *root = this->Parser->GetRootElement();
    int numOfNestedEle = root->GetNumberOfNestedElements();
    if (numOfNestedEle <= 0)
    {
        cerr << "XML tree is empty.\n";
        return false;
    }
 
    int i = 0;
    while (i < numOfNestedEle)
    { 
        vtkXMLDataElement *next = root->GetNestedElement(i); 
        std::string name(next->GetName());
        if (name.compare("send_to_ip") == 0)
        {
            this->ProcessSendToIp(next);
        }
        else if (name.compare("communication") == 0)
        {
            this->ProcessCommunication(next);
        }
        else if (name.compare("tool") == 0)
        {
            this->ProcessTool(next);
        }
        i++;
    }

    return true;
}



void igstk::igstkAuroraTrackerConfigurationComposer::ProcessSendToIp(vtkXMLDataElement *next)
{

}



void igstk::igstkAuroraTrackerConfigurationComposer::ProcessCommunication(vtkXMLDataElement *next)
{
    int numOfNestedEle = next->GetNumberOfNestedElements();
    if (numOfNestedEle <= 0)
    {
        cerr << "Communication element is empty.\n";
        return;
    }

    AuroraTrackerConfiguration *auroraConfig = new AuroraTrackerConfiguration;
    int i = 0;
    while (i < numOfNestedEle)
    { 
        vtkXMLDataElement *n = next->GetNestedElement(i); 
        std::string name(n->GetName());
        char *value = this->ReadElementValue(n);
 
        // cout << "name =  " << name << endl;
        // cout << "value =  " << value << endl;
        int v = atoi(value);
        if (name.compare("com_port") == 0)
        {
            switch (v)
            {
                case 0: 
                    auroraConfig->SetCOMPort(SerialCommunication::PortNumber0); break; 
                case 1: 
                    auroraConfig->SetCOMPort(SerialCommunication::PortNumber1); break; 
                case 2: 
                    auroraConfig->SetCOMPort(SerialCommunication::PortNumber2); break; 
                case 3: 
                    auroraConfig->SetCOMPort(SerialCommunication::PortNumber3); break; 
                case 4: 
                    auroraConfig->SetCOMPort(SerialCommunication::PortNumber4); break; 
                case 5: 
                    auroraConfig->SetCOMPort(SerialCommunication::PortNumber5); break; 
                case 6: 
                    auroraConfig->SetCOMPort(SerialCommunication::PortNumber6); break; 
                case 7: 
                    auroraConfig->SetCOMPort(SerialCommunication::PortNumber7); break; 
            }
        }
        else if (name.compare("baud_rate") == 0)
        {
            switch (v)
            {
                case 9600: 
                    auroraConfig->SetBaudRate(SerialCommunication::BaudRate9600); break; 
                case 19200: 
                    auroraConfig->SetBaudRate(SerialCommunication::BaudRate19200); break; 
                case 38400: 
                    auroraConfig->SetBaudRate(SerialCommunication::BaudRate38400); break; 
                case 57600: 
                    auroraConfig->SetBaudRate(SerialCommunication::BaudRate57600); break; 
                case 115200: 
                    auroraConfig->SetBaudRate(SerialCommunication::BaudRate115200); break; 
            }
        }
        else if (name.compare("data_bits") == 0)
        {
            switch (v)
            {
                case 7: 
                    auroraConfig->SetDataBits(SerialCommunication::DataBits7); break; 
                case 8: 
                    auroraConfig->SetDataBits(SerialCommunication::DataBits8); break; 
            }
        }
        else if (name.compare("parity") == 0)
        {
            std::string p(value);
            if (p.compare("N") == 0)
            {
                auroraConfig->SetParity(SerialCommunication::NoParity); break; 
            }
            else if (p.compare("O") == 0)
            {
                auroraConfig->SetParity(SerialCommunication::OddParity); break; 
            }
            else if (p.compare("E") == 0)
            {
                auroraConfig->SetParity(SerialCommunication::EvenParity); break; 
            }

        }
        else if (name.compare("stop_bits") == 0)
        {
            switch (v)
            {
                case 1: 
                    auroraConfig->SetStopBits(SerialCommunication::StopBits1); break; 
                case 2: 
                    auroraConfig->SetStopBits(SerialCommunication::StopBits2); break; 
            }
        }
        else if (name.compare("hand_shake") == 0)
        {
            switch (v)
            {
                case 0: 
                    auroraConfig->SetHandshake(SerialCommunication::HandshakeOff); break; 
                case 1: 
                    auroraConfig->SetHandshake(SerialCommunication::HandshakeOn); break; 
            }
        }

        i++;
    }

    this->TrackerConfig = auroraConfig; 
}


void igstk::igstkAuroraTrackerConfigurationComposer::ProcessTool(vtkXMLDataElement *next)
{
    int numOfNestedEle = next->GetNumberOfNestedElements();
    if (numOfNestedEle <= 0)
    {
        cerr << "Tool element is empty.\n";
        return;
    }

    AuroraTrackerConfiguration *auroraConfig =static_cast<AuroraTrackerConfiguration*>(this->TrackerConfig); 
    igstk::AuroraToolConfiguration auroraToolConfig;
    int i = 0;
    while (i < numOfNestedEle)
    { 
        vtkXMLDataElement *n = next->GetNestedElement(i); 
        std::string name(n->GetName());
        char *value = this->ReadElementValue(n);
 
        // cout << "name =  " << name << endl;
        // cout << "value =  " << value << endl;
        int v = atoi(value);
        if (name.compare("name") == 0)
        {
        }
        else if (name.compare("control_box_port") == 0)
        {
            auroraToolConfig.SetPortNumber(v);
        }
        else if (name.compare("control_box_channel") == 0)
        {
            auroraToolConfig.SetChannelNumber(v);
        }
        else if (name.compare("srom_file") == 0)
        {
            auroraToolConfig.SetSROMFile(value);
        }
        else if (name.compare("calibration_file") == 0)
        {
        }
        else if (name.compare("ip_port") == 0)
        {
        }
        i++;
    }
    auroraConfig->RequestAddTool(&auroraToolConfig);   
}

