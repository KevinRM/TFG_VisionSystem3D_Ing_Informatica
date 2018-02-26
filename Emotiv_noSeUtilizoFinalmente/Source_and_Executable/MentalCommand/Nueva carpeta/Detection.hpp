#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <winsock.h>
#include "include/Iedk.h"
#include "include/IEmoStateDLL.h"
#include "include/EmotivCloudClient.h"
//#include "include/MentalCommandDetection.h"
//#pragma comment(lib, "Ws2_32.lib")

//const std::
//const std::
//const std::

class Detection {
	private:
		int port;
		int maxang;
		int angle;
		EmoEngineEventHandle eventHandle;
		EmoStateHandle eventState;
		unsigned int userID;
		int userCloudID;
		bool enableLoger;
		std::string userName;
		std::string password;
		std::string profileName;
	public:
		Detection();
		void startProgram();
		bool connect();
		bool calibrateDevice();
		void mouse (const float x/*, const float y*/);
		bool exitProgram();
};