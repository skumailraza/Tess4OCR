/** 
 * @file bg-estimate.h 
 * This is the main header file of document image background estimation algorithm
 * 
 * @author Faisal Shafait (faisalshafait@gmail.com)  
 * 
 * @version 0.1  
 * 
 */ 
 
#ifndef BG_ESTIMATE_H 
#define BG_ESTIMATE_H 
 
#include "stdio.h" 
#include <string> 
#include <fstream> 
#include <algorithm>
#include "image-histogram.h"
// Tesseract headers 
#include <tesseract/baseapi.h> 
#include <leptonica/allheaders.h> 
// Open CV headers 
#include "opencv2/core/core.hpp" 
#include "opencv2/highgui/highgui.hpp" 
#include "opencv2/imgproc/imgproc.hpp" 
 
 
 
/** 
 * @class CBgEstimate 
 * This class contains implementation of document image background estimation algorithm. 
*/ 
 
class CBgEstimate{ 
public:
    /// Binarize image using a threshold on the supplied estimate of the image background
    void binarizeByBackground(cv::Mat &gray, cv::Mat &imgBg, cv::Mat &binary); 
    /// Binarize image with Otsu after image normalization using the supplied estimate of the image background
    void binarizeByBackgroundOtsu(cv::Mat &gray, cv::Mat &imgBg, cv::Mat &binary); 
    void bgEstimatePercentile(cv::Mat &gray, cv::Mat &background); /// Background estimation using the percentile filter 

    CBgEstimate(){
        m_perc_index = 0.5;
        m_perc_w = 80;
        m_bg_thresh = 0.7;
        m_ignore_border = true;
    }
    ~CBgEstimate(){}

private:
    /// BgThresh binarization sensitivity parameter (default value = 0.7)
    double m_bg_thresh;
    /// BgThresh Percentile index to decide per pixel background value (default value = 0.5)
    double m_perc_index;
    /// Window size for local background estimation
    int m_perc_w;
    /// Ignore border pixels along top and bottom to speed up processing
    bool m_ignore_border;
    
    /// Global thresholding using Otsu algorithm 
    double binarizeOtsu(cv::Mat &gray, cv::Mat &binary); 
    int backgroundFromHisto(CHistogram &hist, int pixelCount);    
    
}; 
  
#endif 
  
