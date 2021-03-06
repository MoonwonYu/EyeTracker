/**************************************************************************************************************
 * Disclaimer for Edge Drawing Library
 * 
 * This software library is provided "as is" and "with all faults." Authors of this library make 
 * no warranties of any kind concerning the safety, suitability, lack of viruses, inaccuracies, 
 * typographical errors, or other harmful components of this software product. There are inherent 
 * dangers in the use of any software, and you are solely responsible for determining whether this 
 * software product is compatible with your equipment and other software installed on your equipment. 
 * You are also solely responsible for the protection of your equipment and backup of your data, 
 * and the authors of this software product will not be liable for any damages you may suffer in 
 * connection with using, modifying, or distributing this software product. 
 *
 * You are allowed to use this library for scientific research only. 
 *
 * By using this library you are implicitly assumed to have accepted all of the above statements, 
 * and accept to cite the following papers:
 *
 * [1] C. Topal and C. Akinlar, �Edge Drawing: A Combined Real-Time Edge and Segment Detector,� 
 *     Journal of Visual Communication and Image Representation, 23(6), 862-872, DOI: 10.1016/j.jvcir.2012.05.004 (2012).
 *
 * [2] C. Akinlar and C. Topal, �EDPF: A Real-time Parameter-free Edge Segment Detector with a False Detection Control,� 
 *     International Journal of Pattern Recognition and Artificial Intelligence, 26(1), DOI: 10.1142/S0218001412550026 (2012).
 **************************************************************************************************************/

/***************************************************
 * A simple test program for ED & EDPF
 ***************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "../Headers/Image.h"

#include "../Headers/Timer.h"
#include "../Headers/EdgeMap.h"
#include "../Headers/EDLib.h"

int main(int argc, char *argv[]){
  // Here is the test code
  int width, height;
  unsigned char *srcImg; 

  if (argc != 2) return 1;

  char *str = (char *)argv[1];

  if (ReadImagePGM(str, (char **)&srcImg, &width, &height) == 0){
    printf("Failed opening <%s>\n", str);
    return 1;
  } //end-if

  printf("Working on %dx%d image\n", width, height);

  //-------------------------------- ED Test ------------------------------------
  Timer timer;

  timer.Start();

  EdgeMap *map = DetectEdgesByED(srcImg, width, height, SOBEL_OPERATOR, 36, 8, 1.0);

  timer.Stop();

  printf("ED detects <%d> edge segments in <%4.2lf> ms\n\n", map->noSegments, timer.ElapsedTime());

  // This is how you access the pixels of the edge segments returned by ED
  memset(map->edgeImg, 0, width*height);
  for (int i=0; i<map->noSegments; i++){
    for (int j=0; j<map->segments[i].noPixels; j++){
      int r = map->segments[i].pixels[j].r;
      int c = map->segments[i].pixels[j].c;
      
      map->edgeImg[r*width+c] = 255;
    } //end-for

  } //end-for

  SaveImagePGM((char *)"ED-EdgeMap.pgm", (char *)map->edgeImg, width, height);

  delete map;

  ///---------------------------- EDPF Test ----------------------------------------
  timer.Start();

  TGAImage *img = new TGAImage((short)width, (short)height);
  Colour color = {0,0,0,255};

  for(int x=0; x<height; x++)
    for(int y=0; y<width; y++)
      img->setPixel(color,x,y);

  map = DetectEdgesByEDPF(srcImg, width, height, 1.0);

  timer.Stop();

  printf("EDPF detects <%d> edge segments in <%4.2lf> ms.\n\n", map->noSegments, timer.ElapsedTime());


  // This is how you access the pixels of the edge segments returned by EDPF
  memset(map->edgeImg, 0, width*height);
  for (int i=0; i<map->noSegments; i++){
    color.r = (color.r * 13) % 186 + 70;
    color.g = (color.r * 17) % 186 + 70;
    color.b = (color.r * 41) % 186 + 70;

    for (int j=0; j<map->segments[i].noPixels; j++){
      int r = map->segments[i].pixels[j].r;
      int c = map->segments[i].pixels[j].c;
      
      img->setPixel(color,height-r-1,c);
      map->edgeImg[r*width+c] = 255;
    } //end-for
  } //end-for

  img->WriteImage("image.tga");
  SaveImagePGM((char *)"EDPF-EdgeMap.pgm", (char *)map->edgeImg, width, height);
  delete map;

  //-------------------------------- DetectEdgesByCannySR Test ------------------------------------
  timer.Start();

  map = DetectEdgesByCannySR(srcImg, width, height, 40, 80, 3, 1.0);

  timer.Stop();
  printf("CannySR detects <%d> edge segments in <%4.2lf> ms\n\n", map->noSegments, timer.ElapsedTime());

  // This is how you access the pixels of the edge segments returned by EDPF
  memset(map->edgeImg, 0, width*height);
  for (int i=0; i<map->noSegments; i++){
    for (int j=0; j<map->segments[i].noPixels; j++){
      int r = map->segments[i].pixels[j].r;
      int c = map->segments[i].pixels[j].c;
      
      map->edgeImg[r*width+c] = 255;
    } //end-for
  } //end-for

  SaveImagePGM("CannySR-EdgeMap.pgm", (char *)map->edgeImg, width, height);
  delete map;

  //-------------------------------- DetectEdgesByCannySRPF Test ------------------------------------
  timer.Start();

  map = DetectEdgesByCannySRPF(srcImg, width, height, 3, 1.0);

  timer.Stop();
  printf("CannySRPF detects <%d> edge segments in <%4.2lf> ms\n\n", map->noSegments, timer.ElapsedTime());

  // This is how you access the pixels of the edge segments returned by EDPF
  memset(map->edgeImg, 0, width*height);
  for (int i=0; i<map->noSegments; i++){
    for (int j=0; j<map->segments[i].noPixels; j++){
      int r = map->segments[i].pixels[j].r;
      int c = map->segments[i].pixels[j].c;
      
      map->edgeImg[r*width+c] = 255;
    } //end-for
  } //end-for

  SaveImagePGM("CannySRPF-EdgeMap.pgm", (char *)map->edgeImg, width, height);
  delete map;

  delete srcImg;

  return 0;
} //end-main

