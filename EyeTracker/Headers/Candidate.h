#ifndef _CANDIDATE_H_
#define _CANDIDATE_H_

#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <math.h>

//#define CANDIDATE_MAX_COMBINATION 10
#define CANDIDATE_MIN_RADIUS 30
#define CANDIDATE_MAX_RADIUS 100
#define CANDIDATE_TEST 0

CvBox2D findPuppil(IplImage* cvImg);
double averageRadius(CvBox2D ellipse);
double scoreOfEllipse(Line *line, CvBox2D ellipse);
double getEpsilon(Line *line, CvBox2D ellipse);
double getTau(CvBox2D ellipse);
double getPhi(Line *line, CvBox2D ellipse);
double getCircumference(CvBox2D ellipse);

CvBox2D findPuppil(IplImage* cvImg) {
	double minScore = -1;
	CvBox2D pupil;
	int numOfEllipses = 0;
	CvBox2D ellipses[ELLIPSE_MAX_NUM];
	Line *ellipseLines[ELLIPSE_MAX_NUM];
	CvPoint centroid;

	findEllipsesFromImage(cvImg, &numOfEllipses, ellipses, ellipseLines, &centroid);

	// detect faileure
	pupil.size.width = -1;

	if (CANDIDATE_TEST == 1) {
		printf("num of ellipse : %d\n", numOfEllipses);
		drawEllipses(cvImg, numOfEllipses, ellipses);
	}

	int i, n;

	int numOfCombinations = pow(2,numOfEllipses);
	for (n=1; n<numOfCombinations; n++) {
		int cnt = 0;
		int li, ln, len;
		Line *line = (Line *) malloc (sizeof(Line));
		line->length = 0;

		for (ln=0, li=n; li>0; ln++, li/=2) {
			if (li%2) {
				line->length += ellipseLines[ln]->length;
			}
		}

		line->pixels = (GPixel *) malloc (sizeof(GPixel)*line->length);
		for (ln=0, li=n; li>0; ln++, li/=2) {
			if (li%2) {
				for (len=0; len<ellipseLines[ln]->length; len++) {
					line->pixels[cnt++] = ellipseLines[ln]->pixels[len];
				}
			}
		}

		CvBox2D ellipse = getEllipseFromLine(line);

		if (contains(centroid, ellipse)) {
			double score = scoreOfEllipse(line, ellipse);

			if (CANDIDATE_TEST == 2) {
				drawEllipse(ellipse, cvImg);
			}
			if (minScore < 0 || score < minScore) {
				if (averageRadius(ellipse) > CANDIDATE_MIN_RADIUS &&
					averageRadius(ellipse) < CANDIDATE_MAX_RADIUS) {
					minScore = score;
					pupil = ellipse;
				}
			}
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

double averageRadius(CvBox2D ellipse) {
	return (ellipse.size.width + ellipse.size.height)/4;
}

double scoreOfEllipse(Line *line, CvBox2D ellipse) {
//	printf("%f, %f, %d\n", getEpsilon(line, ellipse), getTau(ellipse), getPhi(line, ellipse));
	double score = pow(getEpsilon(line, ellipse), 2) * exp(abs(1-getTau(ellipse))) / pow(getPhi(line, ellipse), 2);
//	printf("score : %f\n", score);
	return score;
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

double getPhi(Line *line, CvBox2D ellipse) {
	return line->length / getCircumference(ellipse);
}

double getCircumference(CvBox2D ellipse) {
	return M_PI * (5 * (ellipse.size.width+ellipse.size.height)/4 - (ellipse.size.width*ellipse.size.height)/(ellipse.size.width+ellipse.size.height));
}

#endif
