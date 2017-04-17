/** 
 * @file opencv-tess-utils.h 
 * This file contains various utility functions for interfacing Tesseract 
 * and OpenCV and performing some basic image processing functions. 
 * 
 * @author Faisal Shafait (faisalshafait@gmail.com)  
 * 
 * @version 0.1  
 * 
 */ 
 
#ifndef OPENCV_TESS_UTILS_H 
#define OPENCV_TESS_UTILS_H 
 
#include "stdio.h" 
#include <string> 
#include <fstream> 
#include <assert.h>
#include "union-find.h"
// Tesseract headers 
#include <tesseract/baseapi.h> 
#include <leptonica/allheaders.h> 
// Open CV headers 
#include "opencv2/core/core.hpp" 
#include "opencv2/highgui/highgui.hpp" 
#include "opencv2/imgproc/imgproc.hpp" 
 
/** 
 * This function is responsible for converting an 8-bit (CV_8U) binary image
 * from OpenCV format (i.e. Mat data structure) to a 1-bit image in
 * Tesseract's format (Pix *). 
 * Note: Instead of simply copying the header, this method creates a deep
 * copy, meaning that the ownership of OpenCV Mat image remains with OpenCV
 * whereas Tesseract's Pix* points to new data. The returned image needs
 * to be explicitly deleted after usage to avoid memory leaks.
 *  
 * @param[in]     imgMAT Input CV_8U Mat data structure. 
 * @return        Pointer to the newly created 1bpp Pix data structure.
 * 
 */ 
 
Pix * MAT2PIXBinary(cv::Mat &imgMAT){
    assert(imgMAT.depth() == CV_8UC1);
    Pix *imgPIX;
    int w = imgMAT.cols;
    int h = imgMAT.rows;
    int d = 1;
    if ((imgPIX = pixCreate(w, h, d)) == NULL){
        fprintf(stderr,"PIX structure could not be made.\n");
        return imgPIX;
    }
    
    // Get a pointer to target image data
    unsigned int *data = pixGetData(imgPIX);
    int wpl = pixGetWpl(imgPIX);
    unsigned int *line;
    
    // Set target image data bits individually to the values of the source image.
    // Note the inversion due to 0 assigned to background in 1bpp image.
    for(int y=0; y<h; y++) {
        line = data + y * wpl;
        for(int x=0; x<w; x++) {
            bool pixval = !imgMAT.at<uchar>(y,x);
            if(pixval)
                SET_DATA_BIT(line, x);
        }
    }
    return imgPIX;
} 

/** 
 * This function is responsible for converting an 8-bit (CV_8U) image from
 * OpenCV format (i.e. Mat data structure) to an 8-bit image in Tesseract's
 * format (Pix *). 
 * Note: Instead of simply copying the header, this method creates a deep
 * copy, meaning that the ownership of OpenCV Mat image remains with OpenCV
 * whereas Tesseract's Pix* points to new data. The returned image needs
 * to be explicitly deleted after usage to avoid memory leaks.
 *  
 * @param[in]     imgMAT Input CV_8U Mat data structure. 
 * @return        Pointer to the newly created Pix data structure.
 * 
 */ 
 
Pix * MAT2PIXGray(cv::Mat &imgMAT){
    assert(imgMAT.depth() == CV_8U);
    Pix *imgPIX;
    int w = imgMAT.cols;
    int h = imgMAT.rows;
    int d = 8;
    if ((imgPIX = pixCreate(w, h, d)) == NULL){
        fprintf(stderr,"PIX structure could not be made.\n");
        return imgPIX;
    }
    
    // Get a pointer to target image data
    unsigned int *data = pixGetData(imgPIX);
    int wpl = pixGetWpl(imgPIX);
    int bpl = (d * w + 7) / 8;
    unsigned int *line;
    unsigned char val8;
    
    // Set target image data bytes individually to the values of the source image.
    for(int y=0; y<h; y++) {
        line = data + y * wpl;
        for(int x=0; x<bpl; x++) {
            val8 = imgMAT.at<uchar>(y,x);
            SET_DATA_BYTE(line,x,val8);
        }
    }
    return imgPIX;
} 

/** 
 * This function is responsible for converting a greyscale (8-bit) or binary
 * (1-bit) image in Tesseract's format (Pix *) to an 8-bit (CV_8U) image in
 * OpenCV format (i.e. Mat data structure). 
 * Note: Instead of simply copying the header, this method creates a deep
 * copy, meaning that the ownership of Tesseract's Pix* remains with Tesseract
 * whereas OpenCV Mat image points to new data. The input Pix* image needs
 * to be explicitly deleted after usage to avoid memory leaks.
 *  
 * @param[in]     imgMAT Input CV_8U Mat data structure. 
 * @return        Pointer to the newly created Pix data structure.
 * 
 */ 
 
bool PIX2MATGray(Pix *imgPIX, cv::Mat &imgMAT){
    int w, h, d;
    pixGetDimensions(imgPIX, &w, &h, &d);
    unsigned int *datad, *lined;
    datad = pixGetData(imgPIX);
    int wpld = pixGetWpl(imgPIX);
    //fprintf(stderr,"d = %d wpld = %d\n",d,wpld);
    imgMAT.create(h, w, CV_8U);
    
    // If bit depth of Tesseract image is 1 bit per pixel (d=1),
    // extract individual bits and convert them to bytes.
    // Inversion is needed here since Pix represents foreground as
    // 1 and background as 0 in the 1 bit per pixel format.
    if(d == 1){
        for(int y=0; y<h; y++) {
            lined = datad + y * wpld;
            for(int x=0; x<w; x++)
                imgMAT.at<uchar>(y,x) = 255*!GET_DATA_BIT(lined,x);
        }
    }
    
    // If bit depth of Tesseract image is 8 bits per pixel (d=8),
    // simply copy over the bytes one by one.
    else if(d == 8){
        for(int y=0; y<h; y++) {
            lined = datad + y * wpld;
            for(int x=0; x<w; x++)
                imgMAT.at<uchar>(y,x) = GET_DATA_BYTE(lined,x);
        }
    }
}

/** 
 * This function uses union find structures to build connected components
 * in a horizontal and a vertical sweep through the image
 *  
 * @param[in]     img Input CV_8U Mat data structure representing a binary image
 *                Foreground is assumed 0 and background is 255.
 * @param[in]     max_x Maximum allowed width of a connected component as 
 *                a fraction of image width
 * @param[in]     max_y Maximum allowed height of a connected component as 
 *                a fraction of image height
 * @param[in]     min_area Minimum allowed area (width x height) of a valid 
 *                connected component.
 * @param[in]     type Use 4 or 8 connected neighborhood.
 * @param[out]    rboxes An array of bounding boxes of valid connected components.
 * @return        Pointer to the newly created Pix data structure.
 * 
 */ 
void findConComp(cv::Mat &img, std::vector<cv::Rect> &rboxes,
                 float max_x, float max_y, float min_area, int type){	
    assert(img.depth() == CV_8U);
    cv::Mat labelImg = cv::Mat::zeros(img.rows, img.cols, CV_64F);
    int label = 0;
    CUnionFind *uf = new CUnionFind(img.rows*img.cols);
    int l1, l2;
    for (int y=0, Y=img.rows; y<Y; y++){	
        for (int x=0, X=img.cols ; x<X; x++){
            if (img.at<uchar>(y,x)==0){
				// adapt from left neighbor
                if (x>0 && img.at<uchar>(y,x-1)==0){
					labelImg.at<double>(y,x)=label;
                } else {
                    labelImg.at<double>(y,x)=++label;
                } // top neighbor
                if (y>0 && labelImg.at<double>(y-1,x)!=0){
                    l1 = uf->find(label);
                    l2 = uf->find(labelImg.at<double>(y-1,x));
                    if (l1 != l2) uf->set(l1,l2);
                }
                else if (y>0 && x>0 && labelImg.at<double>(y-1,x-1)!=0 && type==8){
                    l1 = uf->find(label);
                    l2 = uf->find(labelImg.at<double>(y-1,x-1));
                    if (l1 != l2) uf->set(l1,l2);
                }
                else if (y>0 && x<X-1 && labelImg.at<double>(y-1,x+1)!=0 && type==8){
                    l1 = uf->find(label);
                    l2 = uf->find(labelImg.at<double>(y-1,x+1));
                    if (l1 != l2) uf->set(l1,l2);
                }
            }
        }
    }
    //fprintf(stderr, "%d labels found!\n", label);
    for (int y=0; y<img.rows; y++) 
        for (int x=0; x<img.cols; x++) 
            labelImg.at<double>(y,x) = uf->find(labelImg.at<double>(y,x));
	
    // Init Bboxes and Seeds
    std::vector<cv::Rect> bb(label+1, cv::Rect());
	std::vector<bool> empty(label+1, true);
    int l;
    cv::Rect b;
    int l_old = 0 ;
    for (int y=0, Y=img.rows; y<Y; y++){
        for (int x=0, X=img.cols; x<X; x++){ 
            if (img.at<uchar>(y,x) == 0){
                l = uf->find(labelImg.at<double>(y,x));
                cv::Rect pt(x,y,1,1);
                if(empty[l]){
                    bb[l] = pt;
                    empty[l] = false;
                } else {
                    bb[l] |= pt;
                }
            }
        }
    }
    // Exclude boxes that are too big or too small
    float max_x_box = img.cols*max_x ;
    float max_y_box = img.rows*max_y ;
    cv::Rect imgDim(0,0,img.cols-1,img.rows-1);
    for (int i=1; i<=label; i++){
        if (uf->isRoot(i)){
            cv::Rect rc = bb[i];
            rc &= imgDim;
            if ( (rc.height > max_y_box) || (rc.width > max_x_box) ||
               (rc.height*rc.width < min_area)) continue;
            rboxes.push_back(rc);
//	        fprintf(stderr,"Root: %d %d %d %d\n", bb[i].x, bb[i].y, bb[i].width, bb[i].height);
        }
    }
}

#endif 
  
