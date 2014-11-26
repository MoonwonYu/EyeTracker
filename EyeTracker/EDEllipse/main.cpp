#include <stdio.h>
#include <stdlib.h>

#include "../Headers/Image.h"
#include "../Headers/EDEllipse.h"
#include "../Headers/EdgeMap.h"
#include "../Headers/EDLib.h"
#include "../Headers/EDCorner.h"
#include "../Headers/Timer.h"

//NameSpaces
using namespace std;

int width, height;

void drawEllipse(Line *line, IplImage *img);
void findEllipsesFromImage(IplImage* cvImg);

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

		width = cvImg->width;
		height = cvImg->height;

		printf("Working on %dx%d image\n", width, height);

		findEllipsesFromImage(cvImg);

		timer.Stop();
		printf("EDEllipse detects edge segments in <%4.2lf> ms.\n\n", timer.ElapsedTime());

		while (1) {
			cvShowImage("Photo", cvImg);
			if (cvWaitKey(10) >= 0) break;
		}

		return 0;
	}

//////////CAM
	CvCapture *capture = cvCaptureFromCAM(CV_CAP_ANY);
	IplImage *frame;

	while(1){
		frame = cvQueryFrame(capture);
		
		if(!frame) {
			printf("Cannot open CAM\n");
			break;
		}

		findEllipsesFromImage(frame);

		cvShowImage("Cam", frame);

		if (cvWaitKey(10) == 27)
			break;
	}

	cvReleaseCapture(&capture);
	cvReleaseImage(&frame);
	cvDestroyWindow("Cam");

	return 0;
}

void drawEllipse(Line *line, IplImage *img) {
	CvPoint centroid = centerOfLargestBlob(img);

	CvBox2D box = getEllipseFromLine(line);

	CvPoint center;
        CvSize size;
        center = cvPointFrom32f(box.center);
        size.width = cvRound(box.size.width*0.5);
        size.height = cvRound(box.size.height*0.5);

	if (!isTooFar(centroid, box)) {
        	cvEllipse(img, center, size, -box.angle, 0, 360, CV_RGB(255,255,255), 1, CV_AA, 0);
	}
}

void findEllipsesFromImage(IplImage* cvImg) {
	int i;

	char *str;
	EdgeMap *map;
	int numOfNearCircular = 0;

	width = cvImg->width;
	height = cvImg->height;

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
		if (!isClosedLine(*lines[i])) continue;
		if (getEntropy(*lines[i]) < SEGMENT_MIN_ENTROPY) continue;

		cornerIndexes = getCorners(*lines[i], &numOfCorner);

		sub_lines = getSubArcs(*lines[i], numOfCorner, cornerIndexes, &count);
		numOfNearCircular += count;

		for (j=0; j<count; j++) {
			drawEllipse(sub_lines[j], cvImg);
		}

		for (int c=0; c<count; c++) {
			free(sub_lines[c]->pixels);
			free(sub_lines[c]);
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
				drawEllipse(sub_lines[j], cvImg);
			}

			for (int c=0; c<count; c++) {
				free(sub_lines[c]->pixels);
				free(sub_lines[c]);
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
