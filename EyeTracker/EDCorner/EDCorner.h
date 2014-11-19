#ifndef _EDCORNER_H_
#define _EDCORNER_H_

#include <stdlib.h>
#include <math.h>
#include "EDSegment.h"

#define CORNER_SIGMA 2.0
#define CORNER_THRESHOLD 4.0

int *getCorners(Line line, int *numOfCorners);
double gaussian(double mean, double stddev, x);

int *getCorners(Line line, int *numOfCorners) {
	int i;
	float *scaledGradient = NULL;
	float *convolvedGradient = NULL;
	int *cornerIndexes = NULL;

	scaledGradient = (float *) malloc (sizeof(float) * line.length);
	convolvedGradient = (float *) malloc (sizeof(float) * line.length);
	cornerIndexes = (int *) malloc (sizeof(int) * line.length);

	scaledGradient[0] = 0;
	for (i=1; i<line.length; i++) {
		float theta = line.pixels[i].gradient - line.pixels[i-1].gradient;
		scaledGradient[i] = scaledGradient[i-1] + (theta < 90.0 ? theta : 180.0 - theta);
	}

	for (i=0; i<line.length; i++) convolvedGradient[0] = 0;	
	
	
	for (i=0; i<line.length; i++) {
		int j;
		for (j=0; j<line.length; j++)
			convolvedGradient[j] += gaussian(i, CORNER_SIGMA, j);
	}

	for (i=0, *numOfCorners = 0; i<line.length; i++) {
		if (convolvedGradient > CORNER_THRESHOLD) cornerIndexes[(*numOfCorners)++] = i;
	}

	return cornerIndexes;
}

double gaussian(double mean, double stddev, x)
{ 
	double variance2 = stddev*stddev*2.0;
	double term = x-mean; 
	return exp(-(term*term)/variance2)/sqrt(M_PI*variance2);
}

#endif
