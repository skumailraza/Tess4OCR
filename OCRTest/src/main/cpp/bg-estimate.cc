/** 
 * @file main-binarize.cc 
 * This is the the top level executable file for the class CBinarize. 
 * As input, it takes a greyscale / color document image and produces
 * a bi-level document image as output using the specified binarization
 * method.
 * 
 * @author Faisal Shafait (faisalshafait@gmail.com)  
 * 
 * @version 0.1  
 * 
 */ 

#include "bg-estimate.h"
//#include "../../../../../../../../../../usr/local/include/tesseract/apitypes.h"
//#include "../../../../../../../../../../usr/local/include/opencv2/calib3d/calib3d.hpp"
//#include "../../../../../../../../../../usr/local/include/opencv2/ocl/ocl.hpp"
//#include "../../../../../../../../../../usr/local/include/opencv2/imgproc/imgproc.hpp"
//#include "../../../../../../../../../../usr/local/include/opencv2/core/types_c.h"
//#include "../../../../../../../../../../usr/local/include/opencv2/core/core_c.h"

using namespace std; 
using namespace tesseract; 
using namespace cv;

/** 
 * This function implements global image thresholding using Otsu's algorithm:
 * N. Otsu, “A threshold selection method from gray-level histograms,”
 * IEEE Transactions on Systems, Man and Cybernetics, vol. 9, no. 1,
 * pp. 62-66, Jan. 1979.
 * @param[in]     gray Gray scale input image as an OpenCV Mat object. 
 * @param[out]    binary Output image binarized using Otsu's method. 
 */ 
double CBgEstimate::binarizeOtsu(Mat &gray, Mat &binary){ 
    double thresh =  threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU); 
    return thresh;
} 
 
/** 
 * This function binarizes the input image based on the supplied background image.
 *  
 * @param[in]     img Gray scale input image to be binarized 
 * @param[in]     imgBg Gray scale background estimated from the input image
 * @param[out]    outimg Output binarized image 
 */ 
void CBgEstimate::binarizeByBackground(Mat &gray, Mat &imgBg, Mat &binary){
    int nRows = gray.rows;
    int nCols = gray.cols;
    gray.copyTo(binary); 
    for (int i=0; i<nRows; i++){ 
        for (int j=0; j<nCols; j++) {
            if (gray.at<uchar>(i,j) < m_bg_thresh * imgBg.at<uchar>(i,j)) 
                binary.at<uchar>(i,j) = 0;
            else 
                binary.at<uchar>(i,j) = 255; 
        } 
    }
}

/** 
 * Binarize image with Otsu after image normalization using the supplied 
 * estimate of the image background.
 *  
 * @param[in]     img Gray scale input image to be binarized 
 * @param[in]     imgBg Gray scale background estimated from the input image
 * @param[out]    outimg Output binarized image 
 */ 
void CBgEstimate::binarizeByBackgroundOtsu(Mat &gray, Mat &imgBg, Mat &binary){
    int nRows = gray.rows;
    int nCols = gray.cols;
    gray.copyTo(binary);
    Mat imgNorm = gray.clone();
    for (int i=0; i<nRows; i++){ 
        for (int j=0; j<nCols; j++) {
            imgNorm.at<uchar>(i,j) = 255.0 * min(1.0,double(gray.at<uchar>(i,j)) / imgBg.at<uchar>(i,j) );
        } 
    }
    binarizeOtsu(imgNorm, binary);
}

 
int CBgEstimate::backgroundFromHisto(CHistogram &hist, int pixelCount) {
    int counter = 0;
    int limit = (int) (m_perc_index * pixelCount);
    int mean=0;
    int newOnes;
                
    for (int i=hist.getRange()-1; counter<limit; i--) {
        mean += hist[i] * i;
        counter += hist[i];
    }
    mean /= counter<1? 1:counter;
    return mean;
}


void CBgEstimate::bgEstimatePercentile(Mat &img, Mat &outimg) {

    int r = m_perc_w >> 1;
    int M;
    int pixelCount;
    int nRows = img.rows;
    int nCols = img.cols;
    if(!m_ignore_border){
        for (int x=0; x<nCols; x++) {
            for (int y=0;y<nRows; y++) {
                if(y<r || y>=nRows-r){
                    int xmin = max(0, x-r);
                    int xmax = min(nCols-1, x+r);
                    int ymin = max(0, y-r);
                    int ymax = min(nRows-1, y+r);
                    CHistogram hist(img, xmin, xmax, ymin, ymax, 256);
                    pixelCount = (xmax-xmin)*(ymax-ymin);
                    M = backgroundFromHisto(hist, pixelCount);
                    outimg.at<uchar>(y,x) = M;
                }
            }
        }
    }
    for (int y=r;y<nRows-r; y++) {
        CHistogram hist(img, 0, r, y-r, y+r, 256);
        pixelCount = (r+1)*(2*r+1);
        for (int x=0; x<nCols; x++) {
            M = backgroundFromHisto(hist, pixelCount);
            outimg.at<uchar>(y,x) = M;
            // remove old column from the histogram
            if (x>=r) {
                for (int dy=-r; dy<=r; dy++) {
                    hist[ img.at<uchar>(y+dy, x-r) ]--;
                    pixelCount--;
                }
            }
                        
            // add new column to the histogram
            if (x<nCols-r-1) {
                for (int dy=-r; dy<=r; dy++) {
                    hist[ img.at<uchar>(y+dy, x+r+1) ]++;
                    pixelCount++;
                }
            }
        }
    }
}


