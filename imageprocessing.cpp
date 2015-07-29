#include "imageprocessing.h"

ImageProcessing::ImageProcessing(QObject *parent) : QObject(parent)
{

}

ImageProcessing::~ImageProcessing()
{

}

void ImageProcessing::erodeImage(QImage &sourceImage)
{
    unsigned char *resData = sourceImage.bits();

    int end = SCREEN_WIDTH*SCREEN_HEIGHT*4;
    bool check[end/4];

    for(int i=0; i<end; i+=4)    {
        if( !(resData[i]==255 && resData[i+1]==0 && resData[i+2]==0) ) {
            for(int k=0; k<8; k++)  {   //
                if( (i + (dir[k][0]*SCREEN_WIDTH) + (dir[k][1]*4)) >= 0 &&
                    (i + (dir[k][0]*SCREEN_WIDTH) + (dir[k][1]*4)) < end &&
                    resData[(i + (dir[k][0]*SCREEN_WIDTH) + (dir[k][1]*4))] == 255 )
                    resData[(i + (dir[k][0]*SCREEN_WIDTH) + (dir[k][1]*4))] = 1;
            }
        }
    }

    for(int i=0; i<end; i+=4)   {
        if( resData[i] == 1 )   {
            resData[i] = 0;
            resData[i+1] = 0;
            resData[i+2] = 0;
        }
    }
}
/*
void ImageProcessing::Dilate()
{
    unsigned char *resData = resultImage.bits();
//    int end = WIDTH*HEIGHT*4;

    for(int i=0; i<HEIGHT; i++) {
        for(int j=0; j<WIDTH; j++)  {
            if( resData[(i*WIDTH + j)*4] == 255 ) {
                for(int k=0; k<8; k++)  {
                    if( ((i+dir[k][0])*WIDTH + j+dir[k][1])*4 >= 0 &&
                        ((i+dir[k][0])*WIDTH + j+dir[k][1])*4 < end &&
                        resData[((i+dir[k][0])*WIDTH + j+dir[k][1])*4 ] == 0 )
                        resData[ ((i+dir[k][0])*WIDTH + j+dir[k][1])*4 ] = 1;
                }
            }
        }
    }
    for(int i=0; i<end; i+=4)   {
        if( resData[i] == 1 )   {
            resData[i] = 255;
            resData[i+1] = 255;
            resData[i+2] = 255;
        }
    }
}
*/
