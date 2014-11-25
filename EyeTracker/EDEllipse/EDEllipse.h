#ifndef _EDELLIPSE_H_
#define _EDELLIPSE_H_

//OpenCV Headers
#include <cv.h>
#include <highgui.h>
#include <math.h>

#include "EDSegment.h"
#include <cvblob.h>

#define ELLIPSE_THRESHOLD 40

using namespace cvb;

CvBox2D getEllipseFromLine(Line *line);
CvPoint centerOfLargestBlob(IplImage* src);
int isTooFar(CvPoint centroid, CvBox2D ellipse);

CvPoint centerOfLargestBlob(IplImage* src) {
	CvPoint centroid;
	IplImage *gray = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	
	cvSmooth(src, gray, CV_GAUSSIAN, 5, 5, 5, 5);
	cvThreshold(gray, gray, 65, 255, CV_THRESH_BINARY_INV);
	IplImage *labelImg=cvCreateImage(cvGetSize(gray), IPL_DEPTH_LABEL, 1);

	CvBlobs blobs;
	unsigned int result = cvLabel(gray, labelImg, blobs);
	unsigned int maxArea = 0;

	for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
	{
		CvBlob *blob = (*it).second;

		if (blob->area > maxArea)
		{
			centroid.x = blob->centroid.x;
			centroid.y = blob->centroid.y;
			maxArea = blob->area;
		}
	}

	cvReleaseImage(&labelImg);
	cvReleaseImage(&gray);

	return centroid;
}

CvBox2D getEllipseFromLine(Line *line) {
	CvMemStorage* storage = cvCreateMemStorage(0); 
        CvSeq* seq = cvCreateSeq(CV_32FC2, sizeof(CvSeq), sizeof(CvPoint2D32f), storage);  

	for (int i=0; i<line->length; i++) {       
		CvPoint2D32f p;
		p.x = (float)line->pixels[i].c;
		p.y = (float)line->pixels[i].r;
		cvSeqPush(seq, &p);
	}

        CvBox2D box = cvFitEllipse2(seq); 
	
        cvClearSeq(seq);
        cvReleaseMemStorage(&storage);

	return box;
}

int isTooFar(CvPoint centroid, CvBox2D ellipse) {
	double threshold = (ellipse.size.width+ellipse.size.height)/2;
	int dx, dy;

	dx = centroid.x - ellipse.center.x;
	dy = centroid.y - ellipse.center.y;

	return sqrt(dx*dx + dy*dy) > threshold;
}

#endif
