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

void drawSubEdges(Line line, int numOfCorner, int *cornerIndexes, TGAImage *img, Colour *color);

int main(int argc, char *argv[]) {
	// Here is the test code
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

	for(int x=0; x<width; x++)
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

		if (isTooShort(*lines[i])) continue;
		if (!isClosedLine(*lines[i])) continue;
		if (getEntropy(*lines[i]) < SEGMENT_MIN_ENTROPY) continue;

		cornerIndexes = getCorners(*lines[i], &numOfCorner);
		
		if (numOfCorner) {
			drawSubEdges(*lines[i], numOfCorner, cornerIndexes, img, &color);
			numOfNearCircular++;
		}
		free (cornerIndexes);

	} //end-for

	if (numOfNearCircular < CORNER_MIN_ARC) {
		printf("There is no Near-Circular\n");

		for (i=0; i<map->noSegments; i++){
			int numOfCorner;
			int *cornerIndexes;

			cornerIndexes = getCorners(*lines[i], &numOfCorner);
			
			if (numOfCorner) {
				drawSubEdges(*lines[i], numOfCorner, cornerIndexes, img, &color);
			}
		
			free (cornerIndexes);
		} //end-for
	} // end-if

	for (i=0; i<map->noSegments; i++) {
		free(lines[i]->pixels);
		free(lines[i]);
	}
	free(lines);

	timer.Stop();

	printf("EDCircle detects <%d> edge segments in <%4.2lf> ms.\n\n", map->noSegments, timer.ElapsedTime());
	img->WriteImage("image.tga");

	delete map;
	delete srcImg;

	return 0;
}

void drawSubEdges(Line line, int numOfCorner, int *cornerIndexes, TGAImage *img, Colour *color) {
	int c;

	Line **sub_lines = (Line **) malloc (sizeof(Line *) * (numOfCorner-1));

	for (c=0; c<(numOfCorner-1); c++) {
		int j;
		sub_lines[c] = makeLine(line, cornerIndexes[c], cornerIndexes[c+1]);
		
		if (isTooShort(*sub_lines[c])) continue;
		if (getEntropy(*sub_lines[c]) < CORNER_MIN_ENTROPY) continue;

		color->r = (color->r * 13) % 186 + 70;
		color->g = (color->g * 83) % 186 + 70;
		color->b = (color->b * 91) % 186 + 70;

		for (j=0; j<sub_lines[c]->length; j++) {
			int x = sub_lines[c]->pixels[j].x;
			int y = sub_lines[c]->pixels[j].y;

			img->setPixel(*color, height-x-1,y);
		}
	}

	for (c=0; c<(numOfCorner-1); c++) {
		free(sub_lines[c]->pixels);
		free(sub_lines[c]);
	}
	free(sub_lines);
}
