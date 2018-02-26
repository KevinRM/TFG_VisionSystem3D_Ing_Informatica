#include <iostream>
#include <signal.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <winsock.h>
#include "include/Iedk.h"

//#include <sstream>
#include <thread>
#include <cstring>
#include <string>
#include "include/IEmoStateDLL.h"
#include "include/IedkErrorCode.h"
#include "include/EmotivCloudClient.h"
#include "include/MentalCommandDetection.h"
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class Detection {
	private:
		int port;
		int maxang;
		int angle;
		EmoEngineEventHandle eventHandle;
		unsigned int userID;
		// Mental
		int userCloudID;
		bool enableLoger;
		string userName;
		string password;
		string profileName;
	public:
		Detection();
		void startProgram();
		bool connectDevice();
		bool calibrateDevice();
		void mouse (const float x/*, const float y*/);
		bool exitProgram();
};