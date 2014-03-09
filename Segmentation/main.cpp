//
//  main.cpp
//  Segmentation
//
//  Created by Eugene Chang on 14/3/8.
//  Copyright (c) 2014年 Eugene Chang. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;
using namespace cv;

char filename[100] = "/Users/yuyueugene84_macbook/Desktop/yuki_erode.jpg";
char rlsa_hor[100] = "/Users/yuyueugene84_macbook/Desktop/yuki_rlsa_h.jpg";
char rlsa_ver[100] = "/Users/yuyueugene84_macbook/Desktop/yuki_rlsa_v.jpg";
char rlsa_AND[100] = "/Users/yuyueugene84_macbook/Desktop/yuki_rlsa_final.jpg";
char rlsa_hor2[100] = "/Users/yuyueugene84_macbook/Desktop/yuki_rlsa_h2.jpg";
char rlsa_canny[100] = "/Users/yuyueugene84_macbook/Desktop/yuki_rlsa_canny.jpg";
char rlsa_contour[100] = "/Users/yuyueugene84_macbook/Desktop/yuki_rlsa_contour.jpg";
int counter = 0;
int tmp_x = 0, tmp_y = 0;
int rlsa_thresh = 30;
int rlsa_thresh_ver = 25;
int rlsa_thresh2 = 9;
uchar a,b,c,d;
int edgeThresh = 50;
int count_contour = 0;
double area = 0, area_largest = 0;

int main(int argc, const char * argv[])
{

    IplImage* img1 = cvLoadImage("/Users/yuyueugene84_macbook/Desktop/gotcha_test/tmp/comp2watch001.jpg");
    
    //declare image for grayscale image
    IplImage* gray = cvCreateImage(cvGetSize(img1),IPL_DEPTH_8U,1);
    //declare image for binary image
    IplImage* binary = cvCloneImage(gray);
    
    IplImage* result = cvCloneImage(gray);
    IplImage* rlsa_result = cvCloneImage(gray);
    IplImage* rlsa_result2 = cvCloneImage(gray);
    IplImage* rlsa_final = cvCloneImage(gray);
    IplImage* img_erode = cvCloneImage(gray);
    IplImage* img_canny = cvCloneImage(gray);
    IplImage* img_final2 = cvCloneImage(gray);
    IplImage* invert = cvCloneImage(gray);
    
    IplImage* img_contour = cvCreateImage(cvGetSize(img1),IPL_DEPTH_8U,3);
    
    
    CvMemStorage *storage = cvCreateMemStorage(0);
    
    CvSeq *first_contour = NULL;
    CvSeq *largest_contour = NULL;
    
    IplConvKernel* pKernel = NULL;
    
    //convert original image to grayscale image
    cvCvtColor(img1, gray, CV_RGB2GRAY);
    //convert grayscale image to binary image
    cvThreshold(gray, binary, 200.0, 255.0, cv::THRESH_BINARY);
    
    int pos = 3;
    
    pKernel = cvCreateStructuringElementEx(pos*2+1, pos*2+1, pos, pos, CV_SHAPE_ELLIPSE);
    //pKernel = cvCreateStructuringElementEx(pos*2+1, pos*2+1, pos, pos, CV_SHAPE_RECT);

    
    cvErode(binary, img_erode, pKernel, 1);
    
    cvSaveImage(filename, img_erode);
    
    for( int y = 0; y < img_erode->height; y++ )
    {
        counter = 0;
        tmp_x = -1;
        
        for ( int x = 0; x < img_erode->width; x++ )
        {
            
            //RLSA horizontal
            b = img_erode->imageData[y * img_erode->widthStep + x];
            
            if (b == 0) {
                //when b is a black pixel and counter is 0, record it
                if (counter == 0) {
                    tmp_x = x;
                    printf("tmp_x = %d\n", tmp_x);

                }
                //when counter is not 0 and its smaller than the threshold, fill the in btwn white pixels into black pixel
                if (tmp_x != -1 && counter != 0 && counter <= rlsa_thresh) {
                    
                    printf("counter = %d\n", counter);
                    
                    for (int i = x-counter; i < x; i++) {
                        rlsa_result->imageData[y * img_erode->widthStep + i] = 0;
                    }
                    
                    //reset counter and tmp_x
                    counter = 0;
                    tmp_x = -1;
                }
            }
            
//            if (b != 0 && tmp_x != -1 && counter > rlsa_thresh) {
//                //space is smaller than threshold
//                rlsa_result2->imageData[y * img_erode->widthStep + x] = 255;
//                tmp_x = -1;
//                counter = 0;
//                //counter++;
//            }
            
            //if pixel is not black and there is a black pixel b4 it
            if (b != 0 && tmp_x != -1) {
                counter++;
            }
            //if pixel is not black
            if (b != 0 && tmp_x == -1) {
                rlsa_result->imageData[y * img_erode->widthStep + x] = 255;
            }
            
        }//end for
    }
    
    cvSaveImage(rlsa_hor, rlsa_result);
    
    
    for (int x = 0; x < img_erode->width; x++) {
        
        counter = 0;
        tmp_y = -1;
        
        for (int y = 0; y < img_erode->height; y++) {
            
            a = img_erode->imageData[y * img_erode->widthStep + x];
            
            if (a == 0) {
                if (counter == 0) {
                    tmp_y = y;
                }
                
                if (tmp_y != -1 && counter != 0 && counter <= rlsa_thresh_ver) {
                    
                    for (int j = y-counter; j < y; j++) {
                        rlsa_result2->imageData[y * img_erode->widthStep + j] = 0;
                        //rlsa_result2->imageData[y * binary->height + j] = 0;
                    }
                    
                    tmp_y = -1;
                    counter = 0;
                }
                
            }
            
            //if pixel is not black and there is a black pixel b4 it
            if (a != 0 && tmp_y != -1) {
                //space is smaller than threshold
  
                counter++;
            }
            
            if (a != 0 && tmp_y != -1 && counter > rlsa_thresh_ver) {
                //space is smaller than threshold
                rlsa_result2->imageData[y * img_erode->widthStep + x] = 255;
                tmp_y = -1;
                counter = 0;
                //counter++;
            }
            
            if (a != 0 && tmp_y == -1) {
                rlsa_result2->imageData[y * img_erode->widthStep + x] = 255;
            }
            
        }
    }//end for
    

    
    
    cvSaveImage(rlsa_ver, rlsa_result2);
    
    for (int y = 0; y<img_erode->height; y++) {
        for (int x = 0; x<img_erode->width; x++) {
            c = rlsa_result->imageData[y * img_erode->widthStep + x];
            d = rlsa_result2->imageData[y * img_erode->widthStep + x];
            
            if (c == 0 && d == 0) {
                rlsa_final->imageData[y * img_erode->widthStep + x] = 0;
            }
            else
            {
                rlsa_final->imageData[y * img_erode->widthStep + x] = 255;
            }
        }
    }
    
    cvSaveImage(rlsa_AND, rlsa_final);
    
    
    for( int y = 0; y < rlsa_final->height; y++ )
    {
        counter = 0;
        tmp_x = -1;
        
        for ( int x = 0; x < rlsa_final->width; x++ )
        {
            
            //RLSA horizontal
            b = rlsa_final->imageData[y * rlsa_final->widthStep + x];
            
            if (b == 0) {
                //when b is a black pixel and counter is 0, record it
                if (counter == 0) {
                    tmp_x = x;
                    printf("tmp_x = %d\n", tmp_x);
                    
                }
                //when counter is not 0 and its smaller than the threshold, fill the in btwn white pixels into black pixel
                if (tmp_x != -1 && counter != 0 && counter <= rlsa_thresh2) {
                    
                    printf("counter = %d\n", counter);
                    
                    for (int i = x-counter; i < x; i++) {
                        img_final2->imageData[y * rlsa_final->widthStep + i] = 0;
                    }
                    
                    //reset counter and tmp_x
                    counter = 0;
                    tmp_x = -1;
                }
            }
            
            if (b != 0 && tmp_x != -1 && counter > rlsa_thresh2) {
                //space is smaller than threshold
                img_final2->imageData[y * rlsa_final->widthStep + x] = 255;
                tmp_x = -1;
                counter = 0;
                //counter++;
            }
            
            //if pixel is not black and there is a black pixel b4 it
            if (b != 0 && tmp_x != -1) {
                counter++;
            }
            //if pixel is not black
            if (b != 0 && tmp_x == -1) {
                img_final2->imageData[y * rlsa_final->widthStep + x] = 255;
            }
            
        }//end for
    }

    cvSaveImage(rlsa_hor2, img_final2);
   
    //canny edge detection of the segmented blocks
    cvCanny(img_final2, img_canny, edgeThresh, edgeThresh*3, 3);
    
    cvSaveImage(rlsa_canny, img_canny);
    
    //invert
//    for (int y=0; y<img_final2->height; y++) {
//        for (int x=0; x<img_final2->width; x++) {
//            a = img_final2->imageData[y * img_final2->widthStep + x];
//            if (a == 0) {
//                invert->imageData[y * invert->widthStep + x] = 255;
//            }
//            else{
//                invert->imageData[y * invert->widthStep + x] = 0;
//            }
//            
//        }
//    }
    
    //cvSaveImage(rlsa_contour, invert);
    
    cvFindContours(img_final2, storage, &first_contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
    //cvSaveImage(rlsa_contour, img_final2);
    
//    cvNamedWindow( "Invert", CV_WINDOW_AUTOSIZE);
//    cvShowImage( "Invert", invert);
    
//    //find contour of image
//    cvFindContours(img_final2, storage, &first_contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
//    
//    cvZero(img_contour);
//    
//    while (first_contour != NULL){
//        double area = fabs(cvContourArea(first_contour,CV_WHOLE_SEQ));
//        if(area > area_largest){
//            area_largest = area;
//            largest_contour = first_contour;
//        }
//        first_contour = first_contour->h_next;
//    }
//    
    for(; first_contour != 0; first_contour = first_contour->h_next)
    {
        count_contour++;
        CvScalar color = CV_RGB(rand()&255, rand()&255, rand()&255);
        cvDrawContours(img_contour, first_contour, color, color, 0, 2, CV_FILLED, cvPoint(0, 0));
        CvRect rect = cvBoundingRect(first_contour,0);
        cvRectangle(img_contour, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height),CV_RGB(255, 0, 0), 1, 8, 0);
    }
    
    printf("the num of contours : %d\n", count_contour);
    
    cvSaveImage(rlsa_contour, img_contour);
    
    //cvReleaseMemStorage(&storage);
    
    return 0;
}

