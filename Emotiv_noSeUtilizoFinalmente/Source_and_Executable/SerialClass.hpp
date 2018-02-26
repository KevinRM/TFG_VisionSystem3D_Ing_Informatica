#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#define ARDUINO_WAIT_TIME 2000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string>
#include <conio.h>

class Serial
{
    private:
        //Serial comm handler
        HANDLE hSerial;
        //Connection status
        bool connected;
        //Get various information about the connection
        COMSTAT status;
        //Keep track of last error
        DWORD errors;

    public:
        //Initialize Serial communication with the given COM port
        Serial(const char *portName);
        //Close the connection
        ~Serial();
        //Writes data from a buffer through the Serial connection
        //return true on success.
        bool WriteData(const char *buffer, unsigned int nbChar);
        //Check if we are actually connected
        bool IsConnected();


};

#endif // SERIALCLASS_H_INCLUDED