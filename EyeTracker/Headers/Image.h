#ifndef __IMAGE_SAVER__
#define __IMAGE_SAVER__

//includes
#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>

using namespace std;

//data structures
typedef struct Colour {
	unsigned char r,g,b,a;
} Colour;

class TGAImage {

public:

	//Default Constructor
TGAImage() {

}

//Overridden Constructor
TGAImage(short width, short height) {
	m_width = width;
	m_height = height;
	m_pixels = new Colour[m_width*m_height];
}

//Set all pixels at once
void setAllPixels(Colour *pixels) {
	m_pixels = pixels;
}

//Set indivdual pixels
void setPixel(Colour inputcolor, int x, int y) {
	m_pixels[convert2dto1d(x,y)] = inputcolor;
}

//Convert 2d array indexing to 1d indexing
int convert2dto1d(int x, int y) {
	return m_width * x + y;
}

void WriteImage(string filename) {

	//Error checking
	if (m_width <= 0 || m_height <= 0)
	{
		printf("Image size is not set properly\n");
		return;
	}

	ofstream o(filename.c_str(), ios::out | ios::binary);

	//Write the header
	o.put(0);
   	o.put(0);
   	o.put(2);                         /* uncompressed RGB */
   	o.put(0); 		o.put(0);
   	o.put(0); 	o.put(0);
   	o.put(0);
   	o.put(0); 	o.put(0);           /* X origin */
   	o.put(0); 	o.put(0);           /* y origin */
   	o.put((m_width & 0x00FF));
   	o.put((m_width & 0xFF00) / 256);
   	o.put((m_height & 0x00FF));
   	o.put((m_height & 0xFF00) / 256);
   	o.put(32);                        /* 24 bit bitmap */
   	o.put(0);
   	
	//Write the pixel data
	for (int i=0;i<m_height*m_width;i++) {
		o.put(m_pixels[i].b);
		o.put(m_pixels[i].g);
		o.put(m_pixels[i].r);
		o.put(m_pixels[i].a);
	}   
	
	//close the file
	o.close();
	
}

private:

	//store the pixels
	Colour *m_pixels;

	short m_height;
	short m_width;

	

};

//////////////////////////////////////////////////////////////////////////////
//	PGM
//////////////////////////////////////////////////////////////////////////////

/// Two functions to read/save PGM files
int ReadImagePGM(char *filename, char **pBuffer, int *pWidth, int *pHeight);
void SaveImagePGM(char *filename, char *buffer, int width, int height);

/******************************************************************************
* Function: ReadImagePGM
* Purpose: This function reads in an image in PGM format. The image can be
* read in from either a file or from standard input. The image is only read
* from standard input when infilename = NULL. Because the PGM format includes
* the number of columns and the number of rows in the image, these are read
* from the file. Memory to store the image is allocated in this function.
* All comments in the header are discarded in the process of reading the
* image. Upon failure, this function returns 0, upon sucess it returns 1.
******************************************************************************/
int ReadImagePGM(char *filename, char **pBuffer, int *pWidth, int *pHeight){
   FILE *fp;
   char buf[71];
   int width, height;

   if ((fp = fopen(filename, "rb")) == NULL){
     fprintf(stderr, "Error reading the file %s in ReadImagePGM().\n", filename);
     return(0);
   } //end-if

   /***************************************************************************
   * Verify that the image is in PGM format, read in the number of columns
   * and rows in the image and scan past all of the header information.
   ***************************************************************************/
   fgets(buf, 70, fp);
   bool P2 = false;
   bool P5 = false;

   if      (strncmp(buf, "P2", 2) == 0) P2 = true;
   else if (strncmp(buf, "P5", 2) == 0) P5 = true;

   if (P2 == false && P5 == false){
      fprintf(stderr, "The file %s is not in PGM format in ", filename);
      fprintf(stderr, "ReadImagePGM().\n");
      fclose(fp);
      return 0;
   } //end-if

   do {fgets(buf, 70, fp);} while (buf[0] == '#');  /* skip all comment lines */
   sscanf(buf, "%d %d", &width, &height);
   fgets(buf, 70, fp);  // Skip max value (255)

   *pWidth = width;
   *pHeight = height;

   /***************************************************************************
   * Allocate memory to store the image then read the image from the file.
   ***************************************************************************/
   if (((*pBuffer) = (char *) malloc((*pWidth)*(*pHeight))) == NULL){
      fprintf(stderr, "Memory allocation failure in ReadImagePGM().\n");
      fclose(fp);
      return(0);
   } //end-if  

   if (P2){
      int index=0;
      char *p = *pBuffer;
      int col = 0;
      int read = 0;

      while (1){
        int c;
        if (fscanf(fp, "%d", &c) < 1) break;
        read++;

        if (col < *pWidth) p[index++] = (unsigned char)c;

        col++;
        if (col == width) col = 0;
      } //end-while

      if (read != width*height){
        fprintf(stderr, "Error reading the image data in ReadImagePGM().\n");
        fclose(fp);
        free((*pBuffer));
        return(0);
      } //end-if

   } else if (P5){
      int index=0;
      char *p = *pBuffer;
      int col = 0;
      int read = 0;

      while (1){
        unsigned char c;
        if (fread(&c, 1, 1, fp) < 1) break;
        read++;

        if (col < *pWidth) p[index++] = c;

        col++;
        if (col == width) col = 0;
      } //end-while

     if (read != width*height){
        fprintf(stderr, "Error reading the image data in ReadImagePGM().\n");
        fclose(fp);
        free((*pBuffer));
        return(0);
     } //end-if
   } //end-else

   fclose(fp);
   return 1;
} //end-ReadPGMImage

///---------------------------------------------------------------------------------
/// Save a buffer as a .pgm image
///
void SaveImagePGM(char *filename, char *buffer, int width, int height){
  FILE *fp = fopen(filename, "wb");

  // .PGM header
  fprintf(fp, "P5\n");
  fprintf(fp, "# Some comment here!\n");
  fprintf(fp, "%d %d\n", width, height);
  fprintf(fp, "255\n");

  // Grayscale image
  fwrite(buffer, 1, width*height, fp);

  fclose( fp );
} //end-SaveImagePGM
#endif
