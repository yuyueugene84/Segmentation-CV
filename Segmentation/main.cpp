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
char rlsa_contour1[100] = "/Users/yuyueugene84_macbook/Desktop/yuki_rlsa_contour1.jpg";
char rlsa_contour2[100] = "/Users/yuyueugene84_macbook/Desktop/yuki_rlsa_contour2.jpg";
int counter = 0;
int tmp_x = 0, tmp_y = 0, tmp_z = 0;
int rlsa_thresh = 30;
int rlsa_thresh_ver = 25;
int rlsa_thresh2 = 9;
uchar a,b,c,d;
int edgeThresh = 50;
int count_contour = 0, count_contour2 = 0;
double area = 0, area_largest = 0;

void FillInternalContours(IplImage *pBinary, double dAreaThre);

int main(int argc, const char * argv[])
{

    IplImage* img1 = cvLoadImage("/Users/yuyueugene84_macbook/Desktop/gotcha_test/tmp/siggraph2011RPBFA006.jpg");
    
    //declare image for grayscale image
    IplImage* gray = cvCreateImage(cvGetSize(img1),IPL_DEPTH_8U,1);
    //declare image for binary image
    IplImage* binary = cvCloneImage(gray);
    
    //IplImage* result = cvCloneImage(gray);
    IplImage* rlsa_result = cvCloneImage(gray);
    IplImage* rlsa_result2 = cvCloneImage(gray);
    IplImage* rlsa_final = cvCloneImage(gray);
    IplImage* img_erode = cvCloneImage(gray);
    //IplImage* img_canny = cvCloneImage(gray);
    IplImage* img_final2 = cvCloneImage(gray);
    //IplImage* invert = cvCloneImage(gray);
    
    IplImage* img_contour = cvCreateImage(cvGetSize(img1),IPL_DEPTH_8U,3);
    IplImage* img_contour2 = cvCreateImage(cvGetSize(img1),IPL_DEPTH_8U,3);
    
    
    CvMemStorage *storage = cvCreateMemStorage(0);
    //CvMemStorage *storage2 = cvCreateMemStorage(0);
    
    vector<CvRect> boxes;
    
    CvSeq *first_contour = NULL;
    //CvSeq *largest_contour = NULL;
    //CvSeq *second_contour = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvRect),storage2);
    //CvSeq *second_contour = NULL;
    

    
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
    
    //rlsa horizontal sweep
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
                    //printf("tmp_x = %d\n", tmp_x);

                }
                //when counter is not 0 and its smaller than the threshold, fill the in btwn white pixels into black pixel
                if (tmp_x != -1 && counter != 0 && counter <= rlsa_thresh) {
                    
                    //printf("counter = %d\n", counter);
                    
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
    
    //rlsa vertical sweep
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
    
    //rlsa vertical and horizontal AND operation
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
    
    //second rlsa horizontal sweep
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
                    //printf("tmp_x = %d\n", tmp_x);
                    
                }
                //when counter is not 0 and its smaller than the threshold, fill the in btwn white pixels into black pixel
                if (tmp_x != -1 && counter != 0 && counter <= rlsa_thresh2) {
                    
                    //printf("counter = %d\n", counter);
                    
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
    //cvCanny(img_final2, img_canny, edgeThresh, edgeThresh*3, 3);
    

    
    //find contour of image
    cvFindContours(img_final2, storage, &first_contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
    cvZero(img_contour2);
    
    CvRect boundbox ;
    
    
    //filter all the contour with area less than 12000
    for(; first_contour; first_contour = first_contour->h_next) {
        boundbox = cvBoundingRect(first_contour);
        area = boundbox.width * boundbox.height;
        //printf("area = %f\n", area);
        if (area > 12000) {
            printf("area = %f\n", area);
            boxes.push_back(boundbox);
        }

    }//end for
    
    area_largest = 0;
    tmp_z = 0;
    //cvSaveImage(rlsa_contour2, img_contour2);
    
    //find largest contour
    for (int z=0; z<boxes.size(); z++) {
        cout << boxes[z].x << endl;
        cout << boxes[z].y << endl;
        cout << boxes[z].width << endl;
        cout << boxes[z].height << endl;
        area = boxes[z].width * boxes[z].height;
        printf("area at %d = %f\n", z, area);
        if (area > area_largest) {
            area_largest = area;
            tmp_z = z;
            printf("area largest at %d = %f\n", z, area);
        }
    }
    
    //delete largest contour, which is the last element of cvseq that stored contour info
    boxes.erase(boxes.begin() + tmp_z);
    
    
    //output contour image
    for (int z=0; z<boxes.size(); z++) {
        cout << boxes[z].x << endl;
        cout << boxes[z].y << endl;
        cout << boxes[z].width << endl;
        cout << boxes[z].height << endl;
        area = boxes[z].width * boxes[z].height;
        printf("area at %d = %f\n", z, area);
        //CvRect rect = cvBoundingRect(first_contour,0);
        cvRectangle(img_contour2, cvPoint(boxes[z].x, boxes[z].y), cvPoint(boxes[z].x + boxes[z].width, boxes[z].y + boxes[z].height),CV_RGB(255, 0, 0), 1, 8, 0);
    }
    
    cvSaveImage(rlsa_contour2, img_contour2);
    
    //Draw contour on the original image
    for (int z=0; z<boxes.size(); z++) {
        cout << boxes[z].x << endl;
        cout << boxes[z].y << endl;
        cout << boxes[z].width << endl;
        cout << boxes[z].height << endl;
        area = boxes[z].width * boxes[z].height;
        printf("area at %d = %f\n", z, area);
        //CvRect rect = cvBoundingRect(first_contour,0);
        cvRectangle(img1, cvPoint(boxes[z].x, boxes[z].y), cvPoint(boxes[z].x + boxes[z].width, boxes[z].y + boxes[z].height),CV_RGB(255, 0, 0), 1, 8, 0);
    }
    
    cvSaveImage(rlsa_contour1, img1);
    
    
    return 0;
}


//void FillInternalContours(IplImage *pBinary, double dAreaThre)
//{
//    double dConArea;
//    CvSeq *pContour = NULL;
//    CvSeq *pConInner = NULL;
//    CvMemStorage *pStorage = NULL;
//    // 执行条件
//    if (pBinary)
//    {
//        // 查找所有轮廓
//        pStorage = cvCreateMemStorage(0);
//        cvFindContours(pBinary, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
//        // 填充所有轮廓
//        cvDrawContours(pBinary, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
//        // 外轮廓循环
//        int wai = 0;
//        int nei = 0;
//        for (; pContour != NULL; pContour = pContour->h_next)
//        {
//            wai++;
//            // 内轮廓循环
//            for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next)
//            {
//                nei++;
//                // 内轮廓面积
//                dConArea = fabs(cvContourArea(pConInner, CV_WHOLE_SEQ));
//                printf("%f\n", dConArea);
//                if (dConArea <= dAreaThre)
//                {
//                    cvDrawContours(pBinary, pConInner, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, CV_FILLED, 8, cvPoint(0, 0));
//                }
//            }
//        }
//        printf("wai = %d, nei = %d\n", wai, nei);
//        cvReleaseMemStorage(&pStorage);
//        pStorage = NULL;
//    }
//}
