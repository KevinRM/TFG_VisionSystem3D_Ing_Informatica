#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <conio.h>
#include <iostream>

using namespace cv;
using namespace std;

class CalibrationTools {
	private:
	bool calibrationComplete = false;
	int numBoards = 10;		// Number of images to take
	int board_w = 7;		// Horizontal corners
	int board_h = 5;		// Vertical corners
	int board_n = board_w * board_h;	// Counter for loops
	Size board_sz = Size (board_w, board_h);	// Size of board
	vector<vector<Point3f> > object_points;		// Real location of corners in 3D
	vector<vector<Point2f> > imagePoints1, imagePoints2;	 // Location of the corners in the image
	vector<Point2f> corners1, corners2;
	vector<Point3f> obj;	// To insert elements in object_points
	Size imageSize;
	Mat CM1 = Mat (3, 3, CV_64FC1);	// Camera matrix 1
	Mat CM2 = Mat (3, 3, CV_64FC1);	// Camera matrix 2
	Mat D1, D2;						// Distotion coefficient
	Mat R, T, E, F;						// Rotation matrix and traslation matrix
	Mat R1, R2, P1, P2, Q;			// Parameters to stereoRectify
	Mat map1x, map1y, map2x, map2y;	// RMAPS parameters to undistort

	public:
	CalibrationTools ();
	void Menu ();
	void ClearScreen ();
	void PressKeyToContinue ();
	void CaptureAndFindChessBoardCorners ();
	void Calibrate ();
	void Rectify ();
	void WriteFiles ();
	void UndistortRectify ();
	void PlayLiveVideo ();
};