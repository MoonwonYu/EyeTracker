#ifndef _IMAGEPROCESS_H_
#define _IMAGEPROCESS_H_

#include <cv.h>
#include <highgui.h>
#include <math.h>

#include "../Headers/EDEllipse.h"

void drawEllipses(IplImage *img, int numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM]);
void drawEllipse(CvBox2D box, IplImage *img);

void drawEllipses(IplImage *img, int numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM]) {
	int i;

	//cvCircle(img, centroid, 1, CV_RGB(255,255,255));

	for (i=0; i<numOfEllipses; i++) {
		drawEllipse(ellipses[i], img);
	}
}

void drawEllipse(CvBox2D box, IplImage *img) {
	CvPoint center;
        CvSize size;
        center = cvPointFrom32f(box.center);
        size.width = cvRound(box.size.width*0.5);
        size.height = cvRound(box.size.height*0.5);

        cvEllipse(img, center, size, -box.angle, 0, 360, CV_RGB(255,255,255), 1, CV_AA, 0);
}

#endif
