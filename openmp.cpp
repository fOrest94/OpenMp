#include "stdafx.h"
#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

VideoCapture prepareVideo() {

	VideoCapture recordVideo(0);

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
	return recordVideo;
}

Mat getFrameFromVideo(Mat frame, double contrastAlpha, int beta_Brightness) {

	int i, j;

	Mat modifiedFrame = Mat::zeros(frame.size(), frame.type());

	//#pragma omp parallel num_threads(2)

	#pragma omp parallel for lastprivate(i)
	for (i = 0; i < frame.rows; i++) {
		#pragma omp parallel for lastprivate(j)
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
		movieFile = prepareVideo();

	int frameCount = movieFile.get(CAP_PROP_FRAME_COUNT);

	cout << "Liczba klatek w filmie: " << frameCount
		<< "\nSzerokosc: " << movieFile.get(CV_CAP_PROP_FRAME_WIDTH)
		<< "\nWysokosc: " << movieFile.get(CV_CAP_PROP_FRAME_HEIGHT)
		<< "\nFormat: " << movieFile.get(CAP_PROP_FORMAT);

	int frame_width = movieFile.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = movieFile.get(CV_CAP_PROP_FRAME_HEIGHT);
	double contrastAlpha = 1.2;
	int beta_Brightness = 45;
	cout << "\n Wybierz kontrast: ( 0.0 - 3.0 )";
	cin >> contrastAlpha;
	cout << "\n Wybierz jasność: ( 0 - 100 )";
	cin >> beta_Brightness;


	VideoWriter video("outcpp2.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height));

	clock_t begin = clock();

	while(1) {

		Mat frame;
		movieFile >> frame;
		if (frame.empty()) 
			break;
		video.write(getFrameFromVideo(frame, contrastAlpha, beta_Brightness));

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
	addEffectsToVideo();
	destroyAllWindows();
	system("pause");
	return 0;
}



