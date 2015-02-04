#ifndef _EDGE_H_
#define _EDGE_H_

#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <math.h>

#include "../Headers/EdgeMap.h"

/// (1) Use DetectEdgesByCannySR(srcImg, width, height, 20, 20, sobelKernelApertureSize, smoothingSigma) to obtain ALL edge segments in the image
/// (2) Validate the edge segments using the Helmholtz principle, returning only the validated edge segments
/// Note: smoothingSigma must be >= 1.0
EdgeMap *DetectEdgesByCanny(unsigned char *srcImg, int width, int height, int sobelKernelApertureSize=3, double smoothingSigma=1.0);

EdgeMap *DetectEdgesByCanny(unsigned char *srcImg, int width, int height, int sobelKernelApertureSize, double smoothingSigma) {
	EdgeMap *map = new EdgeMap(width, height);

	return map;
}

#endif
