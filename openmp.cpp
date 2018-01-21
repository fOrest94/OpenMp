#include "stdafx.h"
#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

void prepareVideo() {

	VideoCapture recordVideo(0);

	if (!recordVideo.isOpened())
		return;

	int frame_width = recordVideo.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = recordVideo.get(CV_CAP_PROP_FRAME_HEIGHT);

	VideoWriter video("outcpp.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, Size(frame_width, frame_height));

	while (1)
	{
		Mat frame;
		recordVideo >> frame;
		if (frame.empty())
			break;
		video.write(frame);
		imshow("Frame", frame);
		char c = (char)waitKey(1);
		if (c == 27)
			break;
	}

	recordVideo.release();
	video.release();
}

Mat getFrameFromVideo(Mat frame) {

	int i, j;
	double contrastAlpha = 1.2;
	int beta_Brightness = 45;

	Mat modifiedFrame = Mat::zeros(frame.size(), frame.type());

	//#pragma omp parallel num_threads(2)

	//#pragma omp parallel for lastprivate(i)
	#pragma omp parallel num_threads(2)
	for (i = 0; i < frame.rows; i++) {
		//#pragma omp parallel for lastprivate(j)
		#pragma omp parallel num_threads(2)
		for (j = 0; j < frame.cols; j++) {
			for (int c = 0; c < 3; c++)
			{
				modifiedFrame.at<Vec3b>(i, j)[c] =
					saturate_cast<uchar>(contrastAlpha*(frame.at<Vec3b>(i, j)[c]) + beta_Brightness);
			}
		}
	}
	return modifiedFrame;
}

void addEffectsToVideo() {

	VideoCapture movieFile("outcpp.avi");

	if (!movieFile.isOpened())
		return;

	int frameCount = movieFile.get(CAP_PROP_FRAME_COUNT);

	cout << "Liczba klatek w filmie: " << frameCount
		<< "\nSzerokosc: " << movieFile.get(CV_CAP_PROP_FRAME_WIDTH)
		<< "\nWysokosc: " << movieFile.get(CV_CAP_PROP_FRAME_HEIGHT)
		<< "\nFormat: " << movieFile.get(CAP_PROP_FORMAT);

	int frame_width = movieFile.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = movieFile.get(CV_CAP_PROP_FRAME_HEIGHT);

	VideoWriter video("outcpp2.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height));

	clock_t begin = clock();

	while(1) {

		Mat frame;
		movieFile >> frame;
		if (frame.empty()) 
			break;
		video.write(getFrameFromVideo(frame));

		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}
	clock_t end = clock();
	double time = double(end - begin) / CLOCKS_PER_SEC;
	cout << " Czas wyniosl: " << time;
	movieFile.release();
	video.release();
}

int main(int, char**)
{
	//prepareVideo();
	addEffectsToVideo();
	destroyAllWindows();
	system("pause");
	return 0;
}



