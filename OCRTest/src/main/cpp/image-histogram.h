/** 
 * @file histogram.h 
 * This is the main header file containing convenience data structure for
 * computing and manipulating histograms over image regions
 * 
 * @author Faisal Shafait (faisalshafait@gmail.com)  
 * 
 * @version 0.1  
 * 
 */ 
 
#ifndef IMAGE_HISTOGRAM_H 
#define IMAGE_HISTOGRAM_H 
 
#include "stdio.h" 
#include <string> 
#include <fstream> 
#include <assert.h>
// Open CV headers 
#include "opencv2/core/core.hpp" 
 
 
/** 
 * @class CHistogram 
 * This class contains implementation of a convenience data structure for
 * computing and manipulating histograms over image regions 
*/ 
 
class CHistogram {
protected:
    int* buckets;
    int  range;

public:
    CHistogram(){
        buckets = 0;
        range = 0;
    }
    CHistogram(int r){
        buckets = new int[r];
        for (int i=0; i<r; i++) buckets[i]=0;
        range = r;
    }
    CHistogram(cv::Mat &img, int r){
        assert(img.depth() == CV_8U);
        int nRows = img.rows;
        int nCols = img.cols;
        range=r;
        int bucketNo;
        buckets = new int[r];
        for (int i=0; i<r; i++) buckets[i]=0;

        int max = 255;
        for (int i=0; i<nCols; i++){
            for (int j=0; j<nRows; j++){
                bucketNo = (int) ( (double)(img.at<uchar>(j,i)) / max * (range-1) );
                buckets[bucketNo]++;
            }
        }
    }
    CHistogram(cv::Mat &img, int x, int y, int w, int r){
        assert(img.depth() == CV_8U);
        range=r;
        int bucketNo;
        buckets = new int[r];
        for (int i=0; i<r; i++) buckets[i]=0;
        int max = 255;
        
        assert(x-w >= 0 && x+w < img.cols && y-w >= 0 && y+w < img.rows);
        for (int i=x-w; i<=x+w; i++){
            for (int j=y-w; j<=y+w; j++){
                bucketNo = (int) ( (double)(img.at<uchar>(j,i)) / max * (range-1) );
                buckets[bucketNo]++;
            }
        }
    }

    CHistogram(cv::Mat &img, int xLow, int xHigh, int yLow, int yHigh, int r) {
        assert(img.depth() == CV_8U);
        range=r;
        int bucketNo;
        buckets = new int[r];
        for (int i=0; i<r; i++) buckets[i]=0;
        int max = 255;

        assert(xLow < xHigh && yLow < yHigh);     
        assert(xLow >= 0 && xHigh < img.cols && yLow >= 0 && yHigh < img.rows);
        for (int i=xLow; i<=xHigh; i++){
            for (int j=yLow; j<=yHigh; j++){
                bucketNo = (int) ( (double)(img.at<uchar>(j,i)) / max * (range-1) );
                buckets[bucketNo]++;
            }
        }
    }

    ~CHistogram(){
        delete [] buckets;
        buckets = 0;
        range = -1;
    }

    int get(int i){
        assert(i>=0 && i<range);
        return buckets[i];
    }
    void out(const char* file = "histo.r"){
        FILE *outstream;
        if ((outstream = fopen(file, "w")) == NULL) {
                fprintf(stderr, "could not write histogram to histo.r");
                return;
                }
        fprintf(outstream, "histo <- c(");
        for (int i=0; i<range-1; i++)
                fprintf(outstream, "%d,", buckets[i]);
        fprintf(outstream, "%d)", buckets[range-1]);
        fclose(outstream);
    }

    int &operator[](int i) const{
        assert(i>=0 && i<range);
        return buckets[i];
    }
    void inc(int i){
        assert(i>=0 && i<range);
        buckets[i] ++ ;
    }
    void dec(int i){
        assert(i>=0 && i<range);
        buckets[i] -- ;
    }
    int getRange(){
        return range;
    }
    void getMinMax(int* maxVal, int* minVal, int* maxValIdx, int* minValIdx){
        *maxVal = -1;
        *minVal = 100000;
        *maxValIdx = -1;
        *minValIdx = -1;
        for(int n=0; n<range; n++){
            int val = buckets[n];
            if(val > *maxVal){
                *maxVal = val;
                *maxValIdx = n;
            }
            if(val < *minVal){
                *minVal = val;
                *minValIdx = n;
            }
        }
        return;
    }
    void plot(cv::Mat &outImg, int limit){
        assert( (range > 0) && (range <= 256) );
        int maxVal, minVal, maxValIdx, minValIdx;
        getMinMax(&maxVal, &minVal, &maxValIdx, &minValIdx);
        int nCols = range + 2;
        double scaleFactor = std::max(1.0, maxVal * 1.0 / double(limit));
        int nRows = std::min(limit, maxVal) + 10;
        cv::Mat histImg(nRows, nCols, CV_8UC1, 255);
        for(int x=0; x<range; x++){
            double histVal = buckets[x] / scaleFactor;
            int topVal = nRows - histVal;
            for(int y=nRows-1; y>topVal; y--){
                histImg.at<uchar>(y, x+1) = 0;
            }
        }
        outImg = histImg.clone();
    }
    
    double getMaxVarThresh(int mLow=0, int mHigh=255){
        assert(range == 256);
        assert(mLow >= 0);
        assert(mHigh < 256);
        assert(mHigh > mLow);
        int    histo[256]; // trimmed histogram
        double pdf[256];   // probability distribution
        double cdf[256];   // cumulative probability distribution
        double myu[256];   // mean value for separation 
        double max_sigma, sigma[256]; // inter-class variance 
        double tmaxvar;
        double histarea = 0.0;

      
        /* initializing histogram */
        for (int i=0; i<256; i++){
            histo[i] = 0;
        }
        
        /* loading histogram in the specified range*/
        for (int i=mLow; i<=mHigh; i++){
            histo[i] = buckets[i];
            histarea += buckets[i];
        }
        
        /* calculation of probability density */
        for (int i=0; i<256; i++){
            pdf[i] = (double)histo[i] / histarea;
        }
        
        /* cdf & myu generation */
        cdf[0] = pdf[0];
        myu[0] = 0.0;       /* 0.0 times prob[0] equals zero */
        for (int i=1; i<256; i++){
            cdf[i] = cdf[i-1] + pdf[i];
            myu[i] = myu[i-1] + i*pdf[i];
        }
      
        /* sigma maximization
           sigma stands for inter-class variance 
           and determines optimal threshold value */
        tmaxvar = 0.0;
        max_sigma = 0.0;
        for (int i=0; i<255; i++){
            if (cdf[i] != 0.0 && cdf[i] != 1.0)
                sigma[i] = pow(myu[255]*cdf[i] - myu[i], 2) / 
                    (cdf[i]*(1.0 - cdf[i]));
            else
                sigma[i] = 0.0;
            if (sigma[i] > max_sigma) {
                max_sigma = sigma[i];
                tmaxvar = i;
            }
        }
        return tmaxvar;
    }
};

  
#endif 
  
