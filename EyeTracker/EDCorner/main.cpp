#include <stdio.h>
#include <stdlib.h>

#include "Image.h"
#include "EDSegment.h"
#include "EDCorner.h"

#include "Timer.h"
#include "EdgeMap.h"
#include "EDLib.h"

#define DEFAULT_IMAGE (char *)"2.pgm"

int width, height;

void drawSubEdges(Line **sub_lines, int count, TGAImage *img, Colour *color);

int main(int argc, char *argv[]) {
	int i;
	
	unsigned char *srcImg; 
	char *str;
	EdgeMap *map;
	Timer timer;
	int numOfNearCircular = 0;

	if (argc == 1) {
		char *temp = DEFAULT_IMAGE;
		str = temp;
	}
	else {
		str = argv[1];
	}

	timer.Start();

	if (ReadImagePGM(str, (char **)&srcImg, &width, &height) == 0){
	 	printf("Failed opening <%s>\n", str);
		return 1;
	} //end-if

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

	if (!numOfNearCircular) {
		printf("There is no Near-Circular\n");

		for (i=0; i<map->noSegments; i++){
			int numOfCorner;
			int *cornerIndexes;
			Line **sub_lines;
			int count;

			cornerIndexes = getCorners(*lines[i], &numOfCorner);
			
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
