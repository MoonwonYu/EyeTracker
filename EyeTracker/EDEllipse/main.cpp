#include <cv.h>
#include <highgui.h>
#include <stdio.h>

#include "../Headers/ImageProcess.h"
#include "../Headers/EDCandidate.h"
#include "../Headers/Timer.h"

//NameSpaces
using namespace std;

int main(int argc, char *argv[])
{
//////////PHOTO
	if (argc == 2) {
		Timer timer;
		timer.Start();

		IplImage *cvImg;

		if ((cvImg = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE)) == 0) {
			printf("Failed opening <%s>\n", argv[1]);
			return 1;
		}

		CvBox2D pupil = findPuppil(cvImg);

		// exception
		if (pupil.size.width > 0) {
			drawEllipse(pupil, cvImg);
		}
		else {
			printf("No pupil detected!!\n");
		}

		timer.Stop();
		printf("EDEllipse worked in <%4.2lf> ms.\n\n", timer.ElapsedTime());
		
		while (1) {
			cvShowImage("Photo", cvImg);
			if (cvWaitKey(10) >= 0) break;
		}

		return 0;
	}
//////////CAM
	CvCapture *capture = cvCaptureFromCAM(-1);
	if (!capture) {
		printf("No camera\n");
		return -1;
	}
	IplImage *frame;

	while(1){
		frame = cvQueryFrame(capture);
		IplImage *cvImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		
		if(!frame) {
			printf("Cannot read cam\n");
			cvReleaseCapture(&capture);
			
			return -1;
		}
		cvCvtColor(frame, cvImg, CV_RGB2GRAY);

		CvBox2D pupil = findPuppil(cvImg);

		// exception
		if (pupil.size.width < 0) {
			printf("No pupil detected!!\n");
		}
		else {
			drawEllipse(pupil, cvImg);
		}

		cvShowImage("Cam", cvImg);

		cvReleaseImage(&cvImg);
		
		if (cvWaitKey(10) == 27) break;
	}

	cvReleaseImage(&frame);
	cvReleaseCapture(&capture);

	return 0;
}


