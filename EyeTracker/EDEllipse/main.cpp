#include <stdio.h>
#include <stdlib.h>

#include "Image.h"
#include "EDEllipse.h"
#include "EdgeMap.h"
#include "EDLib.h"
#include "EDCorner.h"
#include "Timer.h"

#define DEFAULT_IMAGE (char *)"2.pgm"

//NameSpaces
using namespace std;

int width, height;

void drawEllipse(Line *line, IplImage *img);

int main(int argc, char *argv[])
{
	int i;
	
	char *str;
	EdgeMap *map;
	Timer timer;
	int numOfNearCircular = 0;
	IplImage *cvImg;

	if (argc == 1) {
		char *temp = DEFAULT_IMAGE;
		str = temp;
	}
	else {
		str = argv[1];
	}

	timer.Start();

	if ((cvImg = cvLoadImage(str, CV_LOAD_IMAGE_GRAYSCALE)) == 0) {
		printf("Failed opening <%s>\n", str);
		return 1;
	}
	
	width = cvImg->width;
	height = cvImg->height;

	printf("Working on %dx%d image\n", width, height);
	
	timer.Start();
	Colour color = {0,0,0,255};

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

	timer.Stop();

	printf("EDEllipse detects <%d> edge segments in <%4.2lf> ms.\n\n", map->noSegments, timer.ElapsedTime());

	cvShowImage("my", cvImg);
	cvWaitKey(0);
	cvReleaseImage(&cvImg);

	delete map;

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
        	cvEllipse(img, center, size, -box.angle, 0, 360, CV_RGB(100,100,100), 1, CV_AA, 0);
	}
}
