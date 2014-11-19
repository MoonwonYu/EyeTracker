#ifndef _EDSEGMENT_H_
#define _EDSEGMENT_H_

#include "EdgeMap.h"
#include <stdlib.h>
#include <math.h>

#define NUM_DIRECTION 8
#define MIN_ENTROPY 2.0
#define MAX_DISTANCE 15
#define MIN_LENGTH 25

typedef struct GPixel {
	int x;
	int y;
	float gradient;
} GPixel;

typedef struct Line {
	int length;
	GPixel *pixels;
} Line;

int isTooShort(Line line);
Line *makeLine(EdgeSegment segment);
GPixel makePixel(Line line, int x, int y, int index);
float getGradient(Line line, int index);
int isClosedLine(Line line);
int isClosedLine(Line line, int start, int end);
float getMaxEntropy(Line line, int *start, int *end);
float getMaxEntropy(Line line);
float getEntropy(Line line, int start, int end);
int getDirection(float gradient);

Line *makeLine(EdgeSegment segment) {
	int i;
	Line *line = (Line *) malloc (sizeof(Line));

	line->length = segment.noPixels;	
	line->pixels = (GPixel *) malloc (sizeof(GPixel)*segment.noPixels);

	for (i=0; i<segment.noPixels; i++) {
		line->pixels[i] = makePixel(*line, segment.pixels[i].r, segment.pixels[i].c, i);
	}

	return line;
}

GPixel makePixel(Line line, int x, int y, int index) {
	GPixel pixel = {x, y, };
	pixel.gradient = getGradient(line, index);

	return pixel;
}

float getGradient(Line line, int index) {
	float dx, dy;
	
	if (index != 0) {
		dx = line.pixels[index-1].x - line.pixels[index].x;
		dy = line.pixels[index-1].y - line.pixels[index].y;
	}
	else {
		return 0.0;
	}

	return atan(dy/dx)*180.0/M_PI;
}

int isClosedLine(Line line) {
	int dx, dy;

	dx = abs(line.pixels[0].x - line.pixels[line.length-1].x);
	dy = abs(line.pixels[0].y - line.pixels[line.length-1].y);
//	printf("%d, %d\n", dx, dy);
	return dx <= MAX_DISTANCE && dy <= MAX_DISTANCE;
}

int isClosedLine(Line line, int start, int end) {
	int dx, dy;

	dx = abs(line.pixels[start].x - line.pixels[end].x);
	dy = abs(line.pixels[start].y - line.pixels[end].y);
//	printf("%d, %d\n", dx, dy);
	return dx <= MAX_DISTANCE && dy <= MAX_DISTANCE;
}

float getMaxEntropy(Line line, int *start, int *end) {
	float max = 0.0;
	int s, e;
	for (s=0; s<line.length-1; s++) {
		for (e=s+1; e<line.length; e++) {
			float tempEp;
			if (!isClosedLine(line, s, e)) continue;
			if (isTooShort(line)) continue;

			tempEp = getEntropy(line, s, e);
			if (tempEp > MIN_ENTROPY && tempEp > max) {
				max = tempEp;
				*start = s;
				*end = e;
			}
		}
	}

	return max;
}

float getMaxEntropy(Line line) {
	float max = 0.0;
	int s, e;
	
	if (isClosedLine(line, s, e) && !isTooShort(line))
		max = getEntropy(line, 0, line.length-1);

	return max;
}

float getEntropy(Line line, int start, int end) {
	float entropy = 0.0;
	int fgradient[NUM_DIRECTION] = {0, };
	int i;

	for (i=start; i<=end; i++) {
//		printf("%f\n", line.pixels[i].gradient);
		fgradient[getDirection(line.pixels[i].gradient)]++;
	}
	
	for (i=0; i<NUM_DIRECTION; i++) {
		float fg = (float)fgradient[i]/(float)(end-start+1);
		if (!fgradient[i]) continue;
		entropy -= fg*log(fg)/log(2);
	}

	return entropy;
}

int getDirection(float gradient) {
	float unit = 180.0 / NUM_DIRECTION;
	int i;
	for (i=1; i<=NUM_DIRECTION; i++) {
		if (gradient >= 90.0-unit*i) return i-1;
	}
	
	printf("unKnown direction : %f\n", gradient);
	return 0;
}

int isTooShort(Line line) {
	return line.length < MIN_LENGTH;
}

#endif
