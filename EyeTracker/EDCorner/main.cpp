#include <stdio.h>
#include <stdlib.h>

#include "../Headers/Image.h"
#include "../Headers/EDSegment.h"
#include "../Headers/EDCorner.h"

#include "../Headers/Timer.h"
#include "../Headers/EdgeMap.h"
#include "../Headers/EDLib.h"

#include <cv.h>
#include <highgui.h>

using namespace cv;

int width, height;

void drawSubEdges(Line **sub_lines, int count, TGAImage *img, Colour *color);

int main(int argc, char *argv[]) {
	int i;
	unsigned char *srcImg;
	EdgeMap *map;
	Timer timer;
	int numOfNearCircular = 0;

	IplImage *cvImg;

	if (argc != 2) return 1;

	char *	str = (char *)argv[1];

	if (ReadImagePGM(str, (char **)&srcImg, &width, &height) == 0){
		printf("Failed opening <%s>\n", str);
		return 1;
	} //end-if

	timer.Start();

	if ((cvImg = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR)) == 0) {
			printf("Failed opening <%s>\n", argv[1]);
			return 1;
	}

	printf("Working on %dx%d image\n", width, height);
	
	timer.Start();
	TGAImage *img = new TGAImage((short)width, (short)height);
	Colour color = {0,0,0,255};

	for(int x=0; x<height; x++)
		for(int y=0; y<width; y++)
			img->setPixel(color,x,y);

	map = DetectEdgesByEDPF(srcImg, width, height, 1.0);

	Line **lines = NULL;
	lines = (Line **) malloc (sizeof(Line *) * map->noSegments);
	for (i=0; i<map->noSegments; i++){
		lines[i] = makeLine(map->segments[i]);
	}
	
	for (i=0; i<map->noSegments; i++){
		int numOfCorner;
		int *cornerIndexes;
		Line **sub_lines;
		int count;

		if (lines[i]->length < SEGMENT_MIN_CIRCULAR_LENGTH) continue;
		if (!isClosedLine(*lines[i])) continue;
		if (getEntropy(*lines[i]) < SEGMENT_MIN_ENTROPY) continue;

		cornerIndexes = getCorners(*lines[i], &numOfCorner);
		
		for (int c=0; c<numOfCorner; c++)
			cvCircle(cvImg, Point(lines[i]->pixels[cornerIndexes[c]].c, lines[i]->pixels[cornerIndexes[c]].r), 2, CV_RGB(200,200,0));

		sub_lines = getSubArcs(*lines[i], numOfCorner, cornerIndexes, &count);
		numOfNearCircular += count;

		drawSubEdges(sub_lines, count, img, &color);
		
		for (int c=0; c<count; c++) {
			free(sub_lines[c]->pixels);
			free(sub_lines[c]);
		}
		free(sub_lines);

		free (cornerIndexes);

	} //end-for

	if (1) {
//	if (!numOfNearCircular) {
		printf("There is no Near-Circular\n");

		for (i=0; i<map->noSegments; i++){
			int numOfCorner;
			int *cornerIndexes;
			Line **sub_lines;
			int count;

			cornerIndexes = getCorners(*lines[i], &numOfCorner);
			
			for (int c=0; c<numOfCorner; c++)
				cvCircle(cvImg, Point(lines[i]->pixels[cornerIndexes[c]].c, lines[i]->pixels[cornerIndexes[c]].r), 2, CV_RGB(0,200,200));

			sub_lines = getSubArcs(*lines[i], numOfCorner, cornerIndexes, &count);

			drawSubEdges(sub_lines, count, img, &color);
		
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

	printf("EDCorner detects <%d> edge segments in <%4.2lf> ms.\n\n", map->noSegments, timer.ElapsedTime());
	img->WriteImage("image.tga");

	cvSaveImage("image.jpg", cvImg);

	delete map;
	delete srcImg;

	return 0;
}

void drawSubEdges(Line **sub_lines, int count, TGAImage *img, Colour *color) {
	int i;

	for (i=0; i<count; i++) {
		int j;

		color->r = (color->r * 13) % 186 + 70;
		color->g = (color->g * 83) % 186 + 70;
		color->b = (color->b * 91) % 186 + 70;

		for (j=0; j<sub_lines[i]->length; j++) {
			int r = height-sub_lines[i]->pixels[j].r-1;
			int c = sub_lines[i]->pixels[j].c;

			img->setPixel(*color, r, c);
		}
	}
}
