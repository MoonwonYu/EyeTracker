#ifndef _EDSEGMENT_H_
#define _EDSEGMENT_H_

#include "EdgeMap.h"
#include <stdlib.h>
#include <math.h>

#define SEGMENT_NUM_DIRECTION 8
#define SEGMENT_MIN_ENTROPY 2.5
#define SEGMENT_MAX_DISTANCE 15
#define SEGMENT_MIN_LENGTH 25

typedef struct GPixel {
	int x;
	int y;
	double gradient;
} GPixel;

typedef struct Line {
	int length;
	GPixel *pixels;
} Line;

int isTooShort(Line line);
Line *makeLine(EdgeSegment segment);
Line *makeLine(Line line, int start, int end);
double getGradient(Line line, int index);
int isClosedLine(Line line);
int isClosedLine(Line line, int start, int end);
double getMaxEntropy(Line line, int *start, int *end);
double getEntropy(Line line);
double getEntropy(Line line, int start, int end);
int getDirection(double gradient);

Line *makeLine(EdgeSegment segment) {
	int i;
	Line *line = (Line *) malloc (sizeof(Line));
	memset(line, 0, sizeof(Line));

	line->length = segment.noPixels;	
	line->pixels = (GPixel *) malloc (sizeof(GPixel) * segment.noPixels);
	memset(line->pixels, 0, sizeof(GPixel) * segment.noPixels);

	for (i=0; i<segment.noPixels; i++) {
		line->pixels[i].x = segment.pixels[i].r;
		line->pixels[i].y = segment.pixels[i].c;
	}
	for (i=0; i<segment.noPixels; i++) {
		line->pixels[i].gradient = getGradient(*line, i);
	}

	return line;
}

Line *makeLine(Line line, int start, int end) {
	int i;
	int length = end-start+1;

	Line *sub_line = (Line *) malloc (sizeof(Line));
	memset(sub_line, 0, sizeof(Line));

	sub_line->length = length;
	sub_line->pixels = (GPixel *) malloc (sizeof(GPixel)*length);
	memset(sub_line->pixels, 0, sizeof(GPixel)*length);

	for (i=0; i<length; i++) {
		sub_line->pixels[i].x = line.pixels[start+i].x;
		sub_line->pixels[i].y = line.pixels[start+i].y;
		sub_line->pixels[i].gradient = line.pixels[start+i].gradient;
	}

	return sub_line;
}

double getGradient(Line line, int index) {
	double dx, dy;
	double theta180;	// +-90
	double theta360;	// +-180
	
	if (index > 0 && index < line.length-1) {
		dx = line.pixels[index-1].x - line.pixels[index+1].x;
		dy = line.pixels[index-1].y - line.pixels[index+1].y;
	}
	else if (index == 0) {
		dx = line.pixels[index].x - line.pixels[index+1].x;
		dy = line.pixels[index].y - line.pixels[index+1].y;
	}
	else if (index == line.length-1) {
		dx = line.pixels[index-1].x - line.pixels[index].x;
		dy = line.pixels[index-1].y - line.pixels[index].y;
	}
	else {
		printf("out of index (getGradient) : %d\n", index);
		return 0.0;
	}

	theta180 = atan(dy/dx)*180.0/M_PI;
	if (dx > 0) theta360 = theta180;
	else {
		if (dy > 0) {
			theta360 = theta180 + 180.0;
		}
		else {
			theta360 = theta180 - 180.0;
		}
	}

	return theta360;
}

int isClosedLine(Line line) {
	int dx, dy;

	dx = abs(line.pixels[0].x - line.pixels[line.length-1].x);
	dy = abs(line.pixels[0].y - line.pixels[line.length-1].y);
//	printf("%d, %d\n", dx, dy);
	return dx <= SEGMENT_MAX_DISTANCE && dy <= SEGMENT_MAX_DISTANCE;
}

int isClosedLine(Line line, int start, int end) {
	int dx, dy;

	dx = abs(line.pixels[start].x - line.pixels[end].x);
	dy = abs(line.pixels[start].y - line.pixels[end].y);
//	printf("%d, %d\n", dx, dy);
	return dx <= SEGMENT_MAX_DISTANCE && dy <= SEGMENT_MAX_DISTANCE;
}

double getMaxEntropy(Line line, int *start, int *end) {
	double max = 0.0;
	int s, e;
	
	for (s=0; s<line.length-1; s++) {
		for (e=s+1; e<line.length; e++) {
			double tempEp;

			tempEp = getEntropy(line, s, e);
			if (tempEp > max) {
				max = tempEp;
				*start = s;
				*end = e;
			}
		}
	}

	return max;
}

double getEntropy(Line line) {
	double entropy = 0.0;
	int fgradient[SEGMENT_NUM_DIRECTION] = {0, 0, 0, 0, 0, 0, 0, 0};
	int i;

	for (i=0; i<line.length; i++) {
		fgradient[getDirection(line.pixels[i].gradient)]++;
	}
	
	for (i=0; i<SEGMENT_NUM_DIRECTION; i++) {
		double fg = (double)fgradient[i]/(double)(line.length);
		if (!fgradient[i]) continue;
		entropy -= fg*log(fg)/log(2);
	}

	return entropy;
}

double getEntropy(Line line, int start, int end) {
	double entropy = 0.0;
	int fgradient[SEGMENT_NUM_DIRECTION] = {0, };
	int i;

	for (i=start; i<=end; i++) {
//		printf("%f\n", line.pixels[i].gradient);
		fgradient[getDirection(line.pixels[i].gradient)]++;
	}
	
	for (i=0; i<SEGMENT_NUM_DIRECTION; i++) {
		double fg = (double)fgradient[i]/(double)(end-start+1);
		if (!fgradient[i]) continue;
		entropy -= fg*log(fg)/log(2);
	}

	return entropy;
}

int getDirection(double gradient) {
	double unit = 180.0 / SEGMENT_NUM_DIRECTION;
	int i;

	for (; gradient > 90.0; gradient -= 180.0);
	for (; gradient < -90.0; gradient += 180.0);

	for (i=1; i<=SEGMENT_NUM_DIRECTION; i++) {
		if (gradient >= 90.0-unit*i) {
			return i-1;
		}
	}
	
	printf("unKnown direction : %f\n", gradient);
	return 0;
}

int isTooShort(Line line) {
	return line.length < SEGMENT_MIN_LENGTH;
}

#endif
