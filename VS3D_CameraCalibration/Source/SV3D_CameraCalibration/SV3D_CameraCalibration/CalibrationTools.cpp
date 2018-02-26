#include "CalibrationTools.h"

CalibrationTools::CalibrationTools () {
	// Assign constant positions to all the corners and assume that camera is moving
	/*for (int j = 0; j<board_n; j++) {
		obj.push_back (Point3f (j / board_w, j%board_w, 0.0f));
	}*/
	object_points.resize(numBoards);
	
	for (int n = 0; n < numBoards; n++) {
		for (int i = 0; i < board_h; i++) {
			for (int j = 0; j < board_w; j++) {
				object_points[n].push_back (Point3f ((float) j * 1.0, (float) i * 1.0, 0));
				//obj.push_back (Point3f ((float) j * 1.0, (float) i * 1.0, 0));
			}
		}
	}
}

void CalibrationTools::Menu () {
	char option;
	bool finishProgram = false;

	do {
		cout << "=====================================" << endl;
		cout << "========= Calibration Tools =========" << endl;
		cout << "=====================================" << endl;
		cout << "1. Start the stereo calibration process." << endl;
		cout << "2. Play live video." << endl;
		cout << "3. Save parameters to files." << endl;
		cout << "4. Exit." << endl;
		cout << "--> ";

		cin >> option;
		if (option >= '1' && option <= '4') {
			switch (option) {
				case '1':
				{
					ClearScreen ();
					CaptureAndFindChessBoardCorners ();
					ClearScreen ();
					Calibrate ();
					ClearScreen ();
					Rectify ();
					ClearScreen ();
					UndistortRectify ();
					ClearScreen ();
					calibrationComplete = true;
				} break;
				case '2':
				{
					if (calibrationComplete) {
						ClearScreen ();
						PlayLiveVideo ();
						ClearScreen ();
					} else {
						cout << endl << "-> ERROR: First execute the calibration process (1)." << endl;
						PressKeyToContinue ();
						ClearScreen ();
					}
				} break;
				case '3':
				{
					if (calibrationComplete) {
						ClearScreen ();
						WriteFiles ();
						ClearScreen ();
					} else {
						cout << endl << "-> ERROR: First execute the calibration process (1)." << endl;
						PressKeyToContinue ();
						ClearScreen ();
					}
				} break;
				case '4':
				{
					finishProgram = true;
				} break;
			}
		} else {
			cout << "Please insert a number (1-4) to select an option." << endl;
			PressKeyToContinue ();
			ClearScreen ();
		}
	} while (!finishProgram);
}

void CalibrationTools::ClearScreen () {
	if (_WIN32 || _WIN64) {
		system ("cls");
	} else {
		system ("clear");
	}
}

void CalibrationTools::PressKeyToContinue () {
	cout << endl << "Press a key to continue." << endl;
	_getch ();
}

void CalibrationTools::CaptureAndFindChessBoardCorners () {
	cout << "-> Capturing images and finding corners" << endl;
	cout << endl << "-> Press SPACE to capture image" << endl << endl;
	Mat img1, img2, gray1, gray2;
	VideoCapture cap1 = VideoCapture (0);
	VideoCapture cap2 = VideoCapture (1);

	int success = 0, k = 0;
	bool found1 = false, found2 = false;

	while (success < numBoards) {
		cap1 >> img1;
		cap2 >> img2;
		cvtColor (img1, gray1, CV_BGR2GRAY);
		cvtColor (img2, gray2, CV_BGR2GRAY);

		found1 = findChessboardCorners (img1, board_sz, corners1, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		found2 = findChessboardCorners (img2, board_sz, corners2, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

		if (found1) {
			cornerSubPix (gray1, corners1, Size (11, 11), Size (-1, -1), TermCriteria (CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			drawChessboardCorners (gray1, board_sz, corners1, found1);
		}

		if (found2) {
			cornerSubPix (gray2, corners2, Size (11, 11), Size (-1, -1), TermCriteria (CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			drawChessboardCorners (gray2, board_sz, corners2, found2);
		}

		imshow ("image1", gray1);
		imshow ("image2", gray2);

		k = waitKey (10);
		if (k == ' ' && found1 != 0 && found2 != 0) {
			imagePoints1.push_back (corners1);
			imagePoints2.push_back (corners2);
			//object_points.push_back (obj);
			success++;
			cout << "Image " << success << " of " << numBoards << " and corners stored" << endl;

			if (success >= numBoards) {
				break;
			}
		}
	}
	imageSize = img1.size ();
	destroyAllWindows ();
	cap1.release ();
	cap2.release ();
	cout << "-> Done!" << endl;
	PressKeyToContinue ();
}

void CalibrationTools::Calibrate () {
	cout << "-> Starting calibration" << endl;

	stereoCalibrate (object_points, imagePoints1, imagePoints2,
		CM1, D1, CM2, D2, imageSize, R, T, E, F,
		CV_CALIB_SAME_FOCAL_LENGTH | CV_CALIB_ZERO_TANGENT_DIST,
		cvTermCriteria (CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5));

	cout << "-> Done!" << endl;
	PressKeyToContinue ();
}

void CalibrationTools::Rectify () {
	cout << "-> Starting rectification" << endl;

	stereoRectify (CM1, D1, CM2, D2, imageSize, R, T, R1, R2, P1, P2, Q);

	cout << "-> Done!" << endl;
	PressKeyToContinue ();
}

void CalibrationTools::UndistortRectify () {
	cout << "-> Applying undistort" << endl;
	
	initUndistortRectifyMap (CM1, D1, R1, P1, imageSize, CV_32FC1, map1x, map1y);
	initUndistortRectifyMap (CM2, D2, R2, P2, imageSize, CV_32FC1, map2x, map2y);
	
	cout << "-> Done!" << endl;
	PressKeyToContinue ();
}

void CalibrationTools::PlayLiveVideo () {
	cout << "-> Playing live video" << endl;
	cout << "-> Press ESC to finish" << endl;
	VideoCapture cap1 = VideoCapture (0);
	VideoCapture cap2 = VideoCapture (1);
	Mat img1, img2, imgU1, imgU2;
	int key = 0;

	while (1) {
		cap1 >> img1;
		cap2 >> img2;

		remap (img1, imgU1, map1x, map1y, INTER_LINEAR, BORDER_CONSTANT, Scalar ());
		remap (img2, imgU2, map2x, map2y, INTER_LINEAR, BORDER_CONSTANT, Scalar ());

		imshow ("image1", imgU1);
		imshow ("image2", imgU2);

		key = waitKey (5);

		if (key == 27) {
			break;
		}
	}
	destroyAllWindows ();
	cap1.release ();
	cap2.release ();
}

void CalibrationTools::WriteFiles () {
	cout << "-> Saving parameters" << endl;
	FileStorage fs1 ("mystereocalib.yml", FileStorage::WRITE);
	if (fs1.isOpened ()) {
		fs1 << "CM1" << CM1;
		fs1 << "CM2" << CM2;
		fs1 << "D1" << D1;
		fs1 << "D2" << D2;
		fs1 << "R" << R;
		fs1 << "T" << T;
		fs1 << "E" << E;
		fs1 << "F" << F;
		fs1 << "R1" << R1;
		fs1 << "R2" << R2;
		fs1 << "P1" << P1;
		fs1 << "P2" << P2;
		fs1 << "Q" << Q;
		fs1.release ();
	} else {
		printf ("Error: Could not open mystereocalib file\n");
	}

	fs1.open ("rmaps.yml", FileStorage::WRITE);
	if (fs1.isOpened ()) {
		fs1 << "M00" << map1x << "M01" << map1y << "M10" << map2x << "M11" << map2y;
		fs1.release ();
	} else {
		cout << "Error: Could not open rmaps file" << endl;
	}
	cout << "-> Done!" << endl;
	PressKeyToContinue ();
}