#include "Detection.hpp"

//using namespace std;

Detection::Detection() {
	port = 1234;
	maxang = 500;
	angle = maxang/4;
	eventHandle = IEE_EmoEngineEventCreate();
	userID = -1;
}

void Detection::startProgram() {
	bool correct = connectDevice();
	if (correct) {
		Sleep(2000);
		system("CLS");

		correct = calibrateDevice();
		if (correct) {
			Sleep(2000);
			system("CLS");

			printf("======= CORRECTO =======\n\n");
			printf("Ya puedes mover la cabeza \n");
			int x = 0, y = 0, dx, dy;

			while (true) {
				if (IEE_HeadsetGetGyroDelta(userID, &dx, &dy) == EDK_OK) {
					x += dx;
					if (x >  maxang) x =  maxang;
					if (x < -maxang) x = -maxang;

					mouse(x*0.5/maxang);
				}
				Sleep(50);
			}
		}
	}
}

bool Detection::connectDevice() {
	printf("======= CONECTANDO =======\n");
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
			printf("Buscando dispositivo... \n");
		}

		Sleep(200);
	}
	return true;
}

bool Detection::calibrateDevice() {
	printf("======= CALIBRANDO =======\n");
	printf("\nPor favor, asegura que tu dispositivo esta encendido y no muevas la cabeza \n\n");

	bool calibrated = false;
	while (!calibrated) {
		int gyroX = 0;
		int gyroY = 0;
		int err = IEE_HeadsetGetGyroDelta(userID, &gyroX, &gyroY);

		//printf("Gyro X: %d 	Gyro Y: %d 	Err: %d 		", gyroX, gyroY, err);
		if (err == EDK_OK){
			printf("\nDispositivo calibrado \n");
			calibrated = true;
		}else if (err == EDK_GYRO_NOT_CALIBRATED){
			printf("El giroscopio no esta calibrado. Por favor, no te muevas \n");
		}else if (err == EDK_CANNOT_ACQUIRE_DATA){
			printf("No se pueden obtener datos \n");
		}else{
			printf("No hay un dispositivo conectado \n");
		}
		Sleep(200);
	}
	return true;
}

void Detection::mouse (const float x/*, const float y*/) {  
	double fScreenWidth  = ::GetSystemMetrics(SM_CXSCREEN)-1; 
	//double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN)-1; 
	double fx = (0.5 + x) * 65535;
	//double fy = (0.5 - y) * 65535;
	INPUT Input = {0};
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
	Input.mi.dx = fx;
	//Input.mi.dy = fy;
	::SendInput(1,&Input,sizeof(INPUT));
}

bool Detection::exitProgram() {
	printf("Pulse una tecla para salir \n");
	_getch();
	return false;
}

int main(void) {
	Detection obj_detection;
	obj_detection.startProgram();

	/*for (int i = 0; i < 10; i++) {
		if (!IEE_EngineDisconnect()) {
			cout << "Desconexion correcta" << "\n";
			return obj_detection.exitProgram();
		} else {
			cout << "No se pudo desconectar (intento " << i << " de 10)\n";
		}
	}
	cout << "Fallo en la desconexion del dispositivo.\n";
	return obj_detection.exitProgram();*/
}

