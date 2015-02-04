#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <stdio.h>

#include "../Headers/ImageProcess.h"
#include "../Headers/Candidate.h"
#include "../Headers/Timer.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	VideoCapture video;
	video.set(CV_CAP_PROP_FOURCC, CV_FOURCC('D','I','V','4'));
	video.open(argv[1]);
	video.set(CV_CAP_PROP_FPS, 30);

	if (!video.isOpened())
	{
		cout<<"Failed to open file!!!"<<endl;
		return -1;
	}
	
	Mat frame;
	while(1)
	{
		if(!video.read(frame))
			break;
		IplImage oriImg = frame;

		IplImage *cvImg = cvCreateImage(cvGetSize(&oriImg), IPL_DEPTH_8U, 1);

		cvCvtColor(&oriImg, cvImg, CV_RGB2GRAY);

		CvBox2D pupil = findPuppil(cvImg);

		// exception check
		if (pupil.size.width > 0) {
			drawEllipse(pupil, cvImg);
		}
		else {
//			printf("No pupil detected!!\n");
		}

		cvShowImage("Cam", cvImg);

		if (cvWaitKey(10) >= 0) break;

		cvReleaseImage(&cvImg);
	}
	return 0;
} 	



