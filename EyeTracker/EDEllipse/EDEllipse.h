#ifndef _EDELLIPSE_H_
#define _EDELLIPSE_H_

#include "EdgeMap.h"
#include "EDSegment.h"
#include <stdlib.h>
#include <math.h>


int isHaveCorner(Line line);
vector<RotatedRect> getEllipseFromLine(Line line);

int isHaveCorner(Line line) {
	return 0;
}

#endif
