//------------------------------------------------------------------------------
// File : txtr.cpp
//------------------------------------------------------------------------------
// GLVU : Copyright 1997 - 2002 
//        The University of North Carolina at Chapel Hill
//------------------------------------------------------------------------------
// Permission to use, copy, modify, distribute and sell this software and its 
// documentation for any purpose is hereby granted without fee, provided that 
// the above copyright notice appear in all copies and that both that copyright 
// notice and this permission notice appear in supporting documentation. 
// Binaries may be compiled with this software without any royalties or 
// restrictions. 
//
// The University of North Carolina at Chapel Hill makes no representations 
// about the suitability of this software for any purpose. It is provided 
// "as is" without express or implied warranty.

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <textureobj.hpp>

void
bwtorgba(unsigned char *b,unsigned char *l,int n) {
    while(n--) {
	l[0] = *b;
	l[1] = *b;
	l[2] = *b;
	l[3] = 0xff;
	l += 4; b++;
    }
}

void
latorgba(unsigned char *b, unsigned char *a,unsigned char *l,int n) {
    while(n--) {
	l[0] = *b;
	l[1] = *b;
	l[2] = *b;
	l[3] = *a;
	l += 4; b++; a++;
    }
}

void
rgbtorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *l,int n) {
    while(n--) {
	l[0] = r[0];
	l[1] = g[0];
	l[2] = b[0];
	l[3] = 0xff;
	l += 4; r++; g++; b++;
    }
}

void
rgbatorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *a,unsigned char *l,int n) {
    while(n--) {
	l[0] = r[0];
	l[1] = g[0];
	l[2] = b[0];
	l[3] = a[0];
        l += 4; r++; g++; b++; a++;
    }
}

typedef struct _ImageRec {
    unsigned short imagic;
    unsigned short type;
    unsigned short dim;
    unsigned short xsize, ysize, zsize;
    unsigned int min, max;
    unsigned int wasteBytes;
    char name[80];
    unsigned long colorMap;
    FILE *file;
    unsigned char *tmp, *tmpR, *tmpG, *tmpB;
    unsigned long rleEnd;
    unsigned int *rowStart;
    int *rowSize;
} ImageRec;

static void
ConvertShort(unsigned short *array, long length) {
    unsigned b1, b2;
    unsigned char *ptr;

    ptr = (unsigned char *)array;
    while (length--) {
	b1 = *ptr++;
	b2 = *ptr++;
	*array++ = (b1 << 8) | (b2);
    }
}

static void
ConvertLong(unsigned *array, long length) {
    unsigned b1, b2, b3, b4;
    unsigned char *ptr;

    ptr = (unsigned char *)array;
    while (length--) {
	b1 = *ptr++;
	b2 = *ptr++;
	b3 = *ptr++;
	b4 = *ptr++;
	*array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
    }
}

static ImageRec *ImageOpen(const char *fileName)
{
    union {
	int testWord;
	char testByte[4];
    } endianTest;
    ImageRec *image;
    int swapFlag;
    int x;

    endianTest.testWord = 1;
    if (endianTest.testByte[0] == 1) {
	swapFlag = 1;
    } else {
	swapFlag = 0;
    }

    image = (ImageRec *)malloc(sizeof(ImageRec));
    if (image == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }
    if ((image->file = fopen(fileName, "rb")) == NULL) {
	perror(fileName);
	exit(1);
    }

    fread(image, 1, 12, image->file);

    if (swapFlag) {
	ConvertShort(&image->imagic, 6);
    }

    image->tmp = (unsigned char *)malloc(image->xsize*256);
    image->tmpR = (unsigned char *)malloc(image->xsize*256);
    image->tmpG = (unsigned char *)malloc(image->xsize*256);
    image->tmpB = (unsigned char *)malloc(image->xsize*256);
    if (image->tmp == NULL || image->tmpR == NULL || image->tmpG == NULL ||
	image->tmpB == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    if ((image->type & 0xFF00) == 0x0100) {
	x = image->ysize * image->zsize * sizeof(unsigned);
	image->rowStart = (unsigned *)malloc(x);
	image->rowSize = (int *)malloc(x);
	if (image->rowStart == NULL || image->rowSize == NULL) {
	    fprintf(stderr, "Out of memory!\n");
	    exit(1);
	}
	image->rleEnd = 512 + (2 * x);
	fseek(image->file, 512, SEEK_SET);
	fread(image->rowStart, 1, x, image->file);
	fread(image->rowSize, 1, x, image->file);
	if (swapFlag) {
	    ConvertLong(image->rowStart, x/sizeof(unsigned));
	    ConvertLong((unsigned *)image->rowSize, x/sizeof(int));
	}
    }
    return image;
}

static void
ImageClose(ImageRec *image) {
    fclose(image->file);
    free(image->tmp);
    free(image->tmpR);
    free(image->tmpG);
    free(image->tmpB);
    free(image);
}

static void
ImageGetRow(ImageRec *image, unsigned char *buf, int y, int z) {
    unsigned char *iPtr, *oPtr, pixel;
    int count;

    if ((image->type & 0xFF00) == 0x0100) {
	fseek(image->file, image->rowStart[y+z*image->ysize], SEEK_SET);
	fread(image->tmp, 1, (unsigned int)image->rowSize[y+z*image->ysize],
	      image->file);

	iPtr = image->tmp;
	oPtr = buf;
	while (1) {
	    pixel = *iPtr++;
	    count = (int)(pixel & 0x7F);
	    if (!count) {
		return;
	    }
	    if (pixel & 0x80) {
		while (count--) {
		    *oPtr++ = *iPtr++;
		}
	    } else {
		pixel = *iPtr++;
		while (count--) {
		    *oPtr++ = pixel;
		}
	    }
	}
    } else {
	fseek(image->file, 512+(y*image->xsize)+(z*image->xsize*image->ysize),
	      SEEK_SET);
	fread(buf, 1, image->xsize, image->file);
    }
}

void
readRGBfile(char *name, unsigned char **texData, int *width, int *height, 
            int *components) 
{
    unsigned *base,*lptr;
    unsigned char *rbuf, *gbuf, *bbuf, *abuf;
    ImageRec *image;
    int y;

    image = ImageOpen(name);
    
    if(!image)
	return;
    (*width)=image->xsize;
    (*height)=image->ysize;
    (*components)=image->zsize;
    base = (unsigned *)malloc(image->xsize*image->ysize*sizeof(unsigned));
    rbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    gbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    bbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    abuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    if(!base || !rbuf || !gbuf || !bbuf)
      return;
    lptr = base;
    for(y=0; y<image->ysize; y++) {
	if(image->zsize>=4) {
	    ImageGetRow(image,rbuf,y,0);
	    ImageGetRow(image,gbuf,y,1);
	    ImageGetRow(image,bbuf,y,2);
	    ImageGetRow(image,abuf,y,3);
	    rgbatorgba(rbuf,gbuf,bbuf,abuf,(unsigned char *)lptr,image->xsize);
	    lptr += image->xsize;
	} else if(image->zsize==3) {
	    ImageGetRow(image,rbuf,y,0);
	    ImageGetRow(image,gbuf,y,1);
	    ImageGetRow(image,bbuf,y,2);
	    rgbtorgba(rbuf,gbuf,bbuf,(unsigned char *)lptr,image->xsize);
	    lptr += image->xsize;
	} else if(image->zsize==2) {
	    ImageGetRow(image,rbuf,y,0);
	    ImageGetRow(image,abuf,y,1);
	    latorgba(rbuf,abuf,(unsigned char *)lptr,image->xsize);
	    lptr += image->xsize;
	} else {
	    ImageGetRow(image,rbuf,y,0);
	    bwtorgba(rbuf,(unsigned char *)lptr,image->xsize);
	    lptr += image->xsize;
	}
    }
    ImageClose(image);
    free(rbuf);
    free(gbuf);
    free(bbuf);
    free(abuf);

    *texData = (unsigned char *)base;
}

void
GLrescaleTextureToPowerOf2(unsigned char **texData,
                           int *texWidth, int *texHeight)
{
 // log b (a) = (log c (a)) / (log c (b))
 float rescaledPowW = log((double)*texWidth ) / log(2.0) + 0.5;
 float rescaledPowH = log((double)*texHeight) / log(2.0) + 0.5;

 int rescaledWidth  = (int)pow(2.0, (int)rescaledPowW);
 if (rescaledWidth < *texWidth)
     rescaledWidth  = (int)pow(2.0, (int)rescaledPowW+1.0);

 int rescaledHeight = (int)pow(2.0, (int)rescaledPowH);
 if (rescaledHeight < *texHeight)
     rescaledHeight = (int)pow(2.0, (int)rescaledPowH+1.0);

 if (*texWidth == (int)rescaledWidth && *texHeight == (int)rescaledHeight)
     return; // nothing to do - texture is already a power of 2

 unsigned int *new_texData = (unsigned int*)malloc(rescaledWidth*rescaledHeight*
                                                   sizeof(unsigned int));
 if (!new_texData) {
    fprintf(stderr,"GLrescaleTextureToPowerOf2(): OUT OF MEMORY\n");
    exit(-1);
 }

 glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
 glPixelStorei(GL_PACK_ALIGNMENT,   4);

 gluScaleImage(GL_RGBA,
               *texWidth, *texHeight, GL_UNSIGNED_INT, *texData,
               rescaledWidth, rescaledHeight, GL_UNSIGNED_INT, new_texData);

 free(*texData);

 *texData = (unsigned char *)new_texData;
 *texWidth  = rescaledWidth;
 *texHeight = rescaledHeight;
}

//----------------------------------------------------------------------------
// CREATES A TEXTURE OBJECT FROM A TIGHTLY-PACKED RGB COLOR ARRAY
// USER SPECIFIES WRAP (GL_CLAMP,GL_REPEAT) AND INTERP MODES (GL_NEAREST,GL_LINE AR)
//----------------------------------------------------------------------------
GLuint createTexture(GLubyte *Color, int Width, int Height,
                     GLfloat WrapMode, GLfloat InterpMode, int Ncomponents)
{
  GLuint TexID;

  // GET AN UNUSED TEXTURE ID
  glGenTextures(1,&TexID);

  // CREATE A NEW TEXTURE OBJECT NAMED TexID
  glBindTexture(GL_TEXTURE_2D,TexID);

  // SET TEXTURE WRAPPING AND FILTERING PARAMS
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, InterpMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, InterpMode);

  // UNPACKING FROM A PACKED COLOR ARRAY, SET BYTE ALIGNMENT TO 1
  int UnpackAlignment;
  glGetIntegerv(GL_UNPACK_ALIGNMENT,&UnpackAlignment);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);

  // DEFINE THE TEXTURE FROM THE COLOR ARRAY
  if (Ncomponents == 3)
   glTexImage2D(GL_TEXTURE_2D,0,3,Width,Height,0,GL_RGB,GL_UNSIGNED_BYTE,Color);
  else
   glTexImage2D(GL_TEXTURE_2D,0,4,Width,Height,0,GL_RGBA,GL_UNSIGNED_BYTE,Color);

  // RESTORE PREVIOUS UNPACK ALIGNMENT
  glPixelStorei(GL_UNPACK_ALIGNMENT,UnpackAlignment);

  return( TexID );
}


