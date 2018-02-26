#include "Detection.hpp"

using namespace std;

Detection::Detection() {
	eventHandle = IEE_EmoEngineEventCreate();
	userID = -1;
	limitGoRight = 0;
	limitGoLeft = 0;
	limitGoUp = 0;
	limitGoDown = 0;
}

void Detection::startProgram() {
	system("CLS");
	bool correct = connectEmotiv();
	if (correct) {
		Sleep(2000);
		system("CLS");

		correct = calibrateEmotiv();
		if (correct) {
			Sleep(2000);
			system("CLS");

			correct = connectArduino();
			if (correct) {
				Sleep(2000);
				system("CLS");

				defineMovementLimitsCameras();
				system("CLS");

				printf("======= CORRECTO =======\n\n");
				printf("Ya puedes mover la cabeza \n");
				printf("Limites -> Derecho: %d 	Izquierdo: %d 	Superior: %d 	Inferior: %d \n", limitGoRight,
				 limitGoLeft, limitGoUp, limitGoDown);
				int x = 0, y = 0, dx, dy;

				while (true) {
					if (IEE_HeadsetGetGyroDelta(userID, &dx, &dy) == EDK_OK) {
						printf("x: %d   y: %d \n", dx, dy);
						x = dx;
						y = dy;
						moveCameras(x, y);
					}
					Sleep(20);
				}
			}
		}
	}
}

bool Detection::connectEmotiv() {
	printf("======= CONECTANDO EMOTIV =======\n");
	if (IEE_EngineConnect() != EDK_OK) {
		printf("Error en la conexion \n");
		return exitProgram();
	}

	int state = 0;
	bool ready = false;
	while (!ready) {
		state = IEE_EngineGetNextEvent(eventHandle);

		if (state == EDK_OK) {
			IEE_Event_t eventType = IEE_EmoEngineEventGetType(eventHandle);
			IEE_EmoEngineEventGetUserId(eventHandle, &userID);

			if (eventType == IEE_UserAdded) {
				printf("Dispositivo conectado \n\n");
				userID = 0;
				ready = true;
			}
		} else {
			printf("Buscando dispositivo.. \r");
		}

		Sleep(200);
	}
	return true;
}

bool Detection::calibrateEmotiv() {
	printf("======= CALIBRANDO EMOTIV =======\n");
	printf("\nPor favor, asegura que tu dispositivo esta encendido y no muevas la cabeza \n\n");

	bool calibrated = false;
	while (!calibrated) {
		int gyroX = 0;
		int gyroY = 0;
		int err = IEE_HeadsetGetGyroDelta(userID, &gyroX, &gyroY);

		if (err == EDK_OK){
			printf("\nDispositivo calibrado \n");
			calibrated = true;
		}else if (err == EDK_GYRO_NOT_CALIBRATED){
			printf("El giroscopio no esta calibrado. Por favor, no te muevas \r");
		}else if (err == EDK_CANNOT_ACQUIRE_DATA){
			printf("No se pueden obtener datos \r");
		}else{
			printf("No hay un dispositivo conectado \r");
		}
		Sleep(200);
	}
	return true;
}

bool Detection::connectArduino() {
	printf("======= CONECTANDO ARDUINO =======\n");
    SP = new Serial("COM5");
    if (SP->IsConnected()) {
    	return true;
    } else {
    	return false;
    }
}

void Detection::defineMovementLimitsCameras() {
	printf("======= DEFINIENDO LIMITES PARA ACTIVACION DEL MOVIMIENTO =======\n\n");
	bool exit = false;
	int movement = 0;
	while (!exit) {
		switch (movement) {
			case 0: {
				printf("-> Limite derecho: Pulsa una tecla para establecer el limite de activacion indicado \n");
				bool defined = false;
				int x = 0, dx, dy;
				while (!defined) {
					if (IEE_HeadsetGetGyroDelta(userID, &dx, &dy) == EDK_OK) {
						x += dx;
						printf("--> %d\r", x);
						
						if (_kbhit()) {
							_getch();
							limitGoRight = x;
							defined = true;
							movement = 1;
							printf("Limite establecido.\n\n");
						}
					}
					Sleep(200);
				}
				pauseProgram();
			} break;
			case 1: {
				printf("-> Limite izquierdo: Pulsa una tecla para establecer el limite de activacion indicado \n");
				bool defined = false;
				int x = 0, dx, dy;
				while (!defined) {
					if (IEE_HeadsetGetGyroDelta(userID, &dx, &dy) == EDK_OK) {
						x += dx;
						printf("--> %d\r", x);
						
						if (_kbhit()) {
							_getch();
							limitGoLeft = x;
							defined = true;
							movement = 2;
							printf("Limite establecido.\n\n");
						}
					}
					Sleep(200);
				}
				pauseProgram();
			} break;
			case 2: {
				printf("-> Limite superior: Pulsa una tecla para establecer el limite de activacion indicado \n");
				bool defined = false;
				int y = 0, dx, dy;
				while (!defined) {
					if (IEE_HeadsetGetGyroDelta(userID, &dx, &dy) == EDK_OK) {
						y += dy;
						printf("--> %d\r", y);
						
						if (_kbhit()) {
							_getch();
							limitGoUp = y;
							defined = true;
							movement = 3;
							printf("Limite establecido.\n\n");
						}
					}
					Sleep(200);
				}
				pauseProgram();
			} break;
			case 3: {
				printf("-> Limite inferior: Pulsa una tecla para establecer el limite de activacion indicado \n");
				bool defined = false;
				int y = 0, dx, dy;
				while (!defined) {
					if (IEE_HeadsetGetGyroDelta(userID, &dx, &dy) == EDK_OK) {
						y += dy;
						printf("--> %d\r", y);
						
						if (_kbhit()) {
							_getch();
							limitGoDown = y;
							defined = true;
							exit = true;
							printf("Limite establecido.\n\n");
						}
					}
					Sleep(200);
				}
				pauseProgram();
			} break;
		}
	}
}

void Detection::moveCameras (const int x, const int y) {
	printf("x: %d   y: %d\r", x, y);
	if (x >= limitGoRight) {
		SP->WriteData("d",1);
	} else if (x <= limitGoLeft) {
		SP->WriteData("a",1);
	}
	if (y >= limitGoUp) {
		SP->WriteData("s",1);
	} else if (y <= limitGoDown) {
		SP->WriteData("w",1);
	}
}

bool Detection::exitProgram() {
	printf("Pulse una tecla para salir \n");
	_getch();
	return false;
}

void Detection::pauseProgram() {
	printf("Pulse una tecla para continuar \n\n");
	_getch();
}

int main(void) {
	Detection obj_detection;
	obj_detection.startProgram();
}

