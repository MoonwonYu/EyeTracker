#ifndef _EDCANDIDATE_H_
#define _EDCANDIDATE_H_

#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <math.h>

#define CANDIDATE_MAX_COMBINATION 7

CvBox2D findPuppil(IplImage* cvImg);
double scoreOfEllipse(Line *line, CvBox2D ellipse);
double getEpsilon(Line *line, CvBox2D ellipse);
double getTau(CvBox2D ellipse);
int getPhi(Line *line);

CvBox2D findPuppil(IplImage* cvImg) {
	double minScore = 1.0;
	CvBox2D pupil;
	int numOfEllipses = 0;
	CvBox2D ellipses[ELLIPSE_MAX_NUM];
	Line *ellipseLines[ELLIPSE_MAX_NUM];

	findEllipsesFromImage(cvImg, &numOfEllipses, ellipses, ellipseLines);

	// detect faileure
	pupil.size.width = -1;

//	drawEllipses(cvImg, numOfEllipses, ellipses);

	int i, n;

	int numOfCombinations = pow(2,numOfEllipses);
	for (n=1; n<numOfCombinations; n++) {
		int cnt = 0;
		int li, ln, len;
		Line *line = (Line *) malloc (sizeof(Line));
		line->length = 0;

		for (ln=0, li=n; li>0; ln++, li/=2) {
			line->length += ellipseLines[ln]->length;
		}

		line->pixels = (GPixel *) malloc (sizeof(GPixel)*line->length);
		for (ln=0, li=n; li>0; ln++, li/=2) {
			for (len=0; len<ellipseLines[ln]->length; len++) {
				line->pixels[cnt++] = ellipseLines[ln]->pixels[len];
			}
		}

		CvBox2D ellipse = getEllipseFromLine(line);

		double score = scoreOfEllipse(line, ellipse);
		if (score < minScore) {
			minScore = score;
			pupil = ellipse;
		}

		free(line->pixels);
		free(line);
	}

	for (i=0; i<numOfEllipses; i++) {
		free(ellipseLines[i]->pixels);
		free(ellipseLines[i]);
	}

	return pupil;
}

double scoreOfEllipse(Line *line, CvBox2D ellipse) {
//	printf("%f, %f, %d\n", getEpsilon(line, ellipse), getTau(ellipse), getPhi(line));
	return pow(getEpsilon(line, ellipse), 2) * exp(abs(1-getTau(ellipse))) / pow(getPhi(line), 2);
}

double getEpsilon(Line *line, CvBox2D ellipse) {
	int i;
	double result = 0;

	for(int i = 0; i < line->length; i++){
		double squareError;
		double r;
		double diff;
		double ox, oy;
		double oGradient;

		ox = line->pixels[i].c - ellipse.center.x;
		oy = line->pixels[i].r - ellipse.center.y;

		oGradient = atan(oy/ox) + ellipse.angle/180.0*M_PI;

		r = ellipse.size.width*ellipse.size.height / sqrt(pow(ellipse.size.height*cos(oGradient), 2) + pow(ellipse.size.width*sin(oGradient), 2));
		diff = r - sqrt(ox*ox + oy*oy);
		squareError = pow(diff , 2);

		result += squareError;
	}

	return sqrt(result)/line->length;
}

double getTau(CvBox2D ellipse) {
	return abs(ellipse.size.width - ellipse.size.height) / (ellipse.size.width + ellipse.size.height);
}

int getPhi(Line *line) {
	return line->length;
}

#endif
