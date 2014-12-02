#ifndef _EDELLIPSE_H_
#define _EDELLIPSE_H_

//OpenCV Headers
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <cvblob.h>

#include "../Headers/EdgeMap.h"
#include "../Headers/EDLib.h"
#include "../Headers/EDSegment.h"
#include "../Headers/EDCorner.h"

#define ELLIPSE_BLOB_THRESHOLD 55
#define ELLIPSE_MAX_DISTANCE 50
#define ELLIPSE_MIN_DISTANCE 20
#define ELLIPSE_MAX_NUM 10

using namespace cvb;

void findEllipsesFromImage(IplImage* cvImg, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM]);
CvBox2D getEllipseFromLine(Line *line);
CvPoint centerOfLargestBlob(IplImage* src);
int contains(CvPoint centroid, CvBox2D ellipse);
int isTooFar(CvPoint centroid, CvBox2D ellipse);
int addEllipse(Line *line, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM], CvPoint centroid);

void findEllipsesFromImage(IplImage* cvImg, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM]) {
	int i;
	
	CvPoint centroid;

	int width, height;
	char *str;
	EdgeMap *map;
	int numOfNearCircular = 0;

	width = cvImg->width;
	height = cvImg->height;

	centroid = centerOfLargestBlob(cvImg);

	map = DetectEdgesByEDPF((unsigned char *)cvImg->imageData, width, height, 1.0);

	Line **lines = NULL;
	lines = (Line **) malloc (sizeof(Line *) * map->noSegments);
	for (i=0; i<map->noSegments; i++){
		lines[i] = makeLine(map->segments[i]);
	}

	for (i=0; i<map->noSegments; i++){
		int j;
		int numOfCorner;
		int *cornerIndexes;
		Line **sub_lines;
		int count;

		if (lines[i]->length < SEGMENT_MIN_CIRCULAR_LENGTH) continue;
		if (getEntropy(*lines[i]) < SEGMENT_MIN_ENTROPY) continue;

		cornerIndexes = getCorners(*lines[i], &numOfCorner);

		sub_lines = getSubArcs(*lines[i], numOfCorner, cornerIndexes, &count);
		numOfNearCircular += count;

		for (j=0; j<count; j++) {
			if (addEllipse(sub_lines[j], numOfEllipses, ellipses, ellipseLines, centroid) != 1) {
				free(sub_lines[j]->pixels);
				free(sub_lines[j]);
			}
		}

		free(sub_lines);

		free (cornerIndexes);

	} //end-for

	if (!numOfNearCircular) {
		printf("There is no Near-Circular\n");

		for (i=0; i<map->noSegments; i++){
			int j;
			int numOfCorner;
			int *cornerIndexes;
			Line **sub_lines;
			int count;

			cornerIndexes = getCorners(*lines[i], &numOfCorner);
	
			sub_lines = getSubArcs(*lines[i], numOfCorner, cornerIndexes, &count);

			for (j=0; j<count; j++) {
				if (addEllipse(sub_lines[j], numOfEllipses, ellipses, ellipseLines, centroid) != 1) {
					free(sub_lines[j]->pixels);
					free(sub_lines[j]);
				}
			}

			free(sub_lines);

			free (cornerIndexes);
		} //end-for
	} // end-if

	for (i=0; i<map->noSegments; i++) {
		free(lines[i]->pixels);
		free(lines[i]);
	}
	free(lines);

	delete map;
}
CvPoint centerOfLargestBlob(IplImage* src) {
	CvPoint centroid;
	IplImage *gray = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	
	cvSmooth(src, gray, CV_GAUSSIAN, 5, 5, 5, 5);
	cvThreshold(gray, gray, ELLIPSE_BLOB_THRESHOLD, 255, CV_THRESH_BINARY_INV);
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

int contains(CvPoint centroid, CvBox2D ellipse) {
	struct Point {
		double x;
		double y;
	} left, right, top, bottom;
	double dx, dy;
	double dleft, dright, dtop, dbottom;

	dx = centroid.x - ellipse.center.x;
	dy = centroid.y - ellipse.center.y;

	double radAngle = ellipse.angle > 90.0 ? (ellipse.angle-180.0)/180.0*M_PI : ellipse.angle/180.0*M_PI;

	left.x = ellipse.center.x - (ellipse.size.width*0.5)*cos(radAngle);
	left.y = ellipse.center.y - (ellipse.size.width*0.5)*sin(radAngle);

	right.x = ellipse.center.x + (ellipse.size.width*0.5)*cos(radAngle);
	right.y = ellipse.center.y + (ellipse.size.width*0.5)*sin(radAngle);

	top.x = ellipse.center.x - (ellipse.size.height*0.5)*sin(radAngle);
	top.y = ellipse.center.y - (ellipse.size.height*0.5)*cos(radAngle);

	bottom.x = ellipse.center.x + (ellipse.size.height*0.5)*sin(radAngle);
	bottom.y = ellipse.center.y + (ellipse.size.height*0.5)*cos(radAngle);

	double gradient = tan(radAngle);

	//printf("angle : %f, gradient : %f\n", ellipse.angle, gradient);
	if (gradient == 0) gradient = 0.00001;

	dleft = centroid.y - (left.y + (-1/gradient) * (centroid.x - left.x));
	dright = centroid.y - (right.y + (-1/gradient) * (centroid.x - right.x));
	dtop = centroid.y - (top.y + gradient * (centroid.x - top.x));
	dbottom = centroid.y - (bottom.y + gradient * (centroid.x - bottom.x));

	return (sqrt(dx*dx + dy*dy) < ELLIPSE_MIN_DISTANCE) || (sqrt(dx*dx + dy*dy) < ELLIPSE_MAX_DISTANCE && dleft*dright < 0 && dtop*dbottom < 0);
}

int isTooFar(CvPoint centroid, CvBox2D ellipse) {
	return !contains(centroid, ellipse);
}

int addEllipse(Line *line, int *numOfEllipses, CvBox2D ellipses[ELLIPSE_MAX_NUM], Line *ellipseLines[ELLIPSE_MAX_NUM], CvPoint centroid) {
	CvBox2D box = getEllipseFromLine(line);

	CvPoint center;
        CvSize size;
        center = cvPointFrom32f(box.center);
        size.width = cvRound(box.size.width*0.5);
        size.height = cvRound(box.size.height*0.5);

	if (!isTooFar(centroid, box)) {
		if (*numOfEllipses == ELLIPSE_MAX_NUM) {
			printf("Too many ellipses\n");
			return -1;
		}

//		printf("score : %f\n", scoreOfEllipse(line, box));
		ellipses[(*numOfEllipses)] = box;
		ellipseLines[(*numOfEllipses)++] = line;
		return 1;
	}

	return 0;
}

#endif
