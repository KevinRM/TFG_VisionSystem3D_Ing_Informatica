#include <iostream>
#include <signal.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <winsock.h>
#include "include/Iedk.h"
#include "SerialClass.hpp"

class Detection {
	private:
		EmoEngineEventHandle eventHandle;
		unsigned int userID;
		Serial* SP;
		int limitGoRight;
		int limitGoLeft;
		int limitGoUp;
		int limitGoDown;
	public:
		Detection();
		void startProgram();
		bool connectEmotiv();
		bool calibrateEmotiv();
		bool connectArduino();
		void defineMovementLimitsCameras();
		void moveCameras (const int x, const int y);
		bool exitProgram();
		void pauseProgram();
};