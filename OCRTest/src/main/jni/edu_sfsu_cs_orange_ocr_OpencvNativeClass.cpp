#include <edu_sfsu_cs_orange_ocr_OpencvNativeClass.h>

JNIEXPORT void JNICALL Java_edu_sfsu_cs_orange_ocr_OpencvNativeClass_BinarizeShafait
(JNIEnv *, jclass, jlong input, jlong output)
{
    Mat &gray = *(Mat*) input;
    Mat &binary = *(Mat*) output;
    int w;
    double k;
    w = 50;
    k = 0.1;
    Mat sum, sumsq;

    int m_LargerDim = 2000;

    int maxDim = max(gray.cols, gray.rows);
    Mat rescaledImg;
    int orows = gray.rows;
    int ocols = gray.cols;
    double scale = m_LargerDim / maxDim;
    resize(gray, gray, Size(), scale, scale, INTER_LANCZOS4);

    cvtColor(gray, gray, COLOR_BGR2GRAY);

    //gray.convertTo(gray, CV_64F);
    gray.copyTo(binary);
    int half_width = w >> 1;
    integral(gray, sum, sumsq,  CV_64F);
        for(int i=0; i<gray.rows; i++){
            for(int j=0; j<gray.cols; j++){
                int x_0 = (i > half_width) ? i - half_width : 0;
                int y_0 = (j > half_width) ? j - half_width : 0;
                int x_1 = (i + half_width >= gray.rows) ? gray.rows - 1 : i + half_width;
                int y_1 = (j + half_width >= gray.cols) ? gray.cols - 1 : j + half_width;
                double area = (x_1-x_0) * (y_1-y_0);
                double mean = (sum.at<double>(x_0,y_0) + sum.at<double>(x_1,y_1) - sum.at<double>(x_0,y_1) - sum.at<double>(x_1,y_0)) / area;
                double sq_mean = (sumsq.at<double>(x_0,y_0) + sumsq.at<double>(x_1,y_1) - sumsq.at<double>(x_0,y_1) - sumsq.at<double>(x_1,y_0)) / area;
                double stdev = sqrt(sq_mean - (mean * mean));
                double threshold = mean * (1 + k * ((stdev / 128) -1) );
                //double thres = threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
                if (gray.at<uchar>(i,j) > threshold)
                    binary.at<uchar>(i,j) = 255;
                else
                    binary.at<uchar>(i,j) = 0;
            }
        }
//    int erosion_elem = 0;
//    int erosion_size = 0;
//    int dilation_elem = 0;
//    int dilation_size = 0;
//    int const max_kernel_size = 3;
//    int erosion_type = MORPH_ELLIPSE;
//    int dilation_type = MORPH_ELLIPSE;
//    Mat element = getStructuringElement( erosion_type,
//                                         Size( 2*erosion_size + 1, 2*erosion_size+1 ),
//                                         Point( erosion_size, erosion_size ) );
//
//    erode( binary, binary, element );
//
//    element = getStructuringElement( dilation_type,
//                                 Size( 2*dilation_size + 1, 2*dilation_size+1 ),
//                                 Point( dilation_size, dilation_size ) );
//    dilate( binary,binary, element );
//    int  morph_operator = 0;
//    int operation = morph_operator + 2;
//    int morph_size = 3; //kernal size
//    int morph_elem = 0;
//    Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
//
//    /// Apply the specified morphology operation
//    morphologyEx( binary, binary, operation, element );
    double iop = 0;
    resize(binary, binary,  cv::Size(ocols,orows), iop, iop, INTER_LANCZOS4);
}


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

double CBgEstimate::binarizeOtsu(Mat &gray, Mat &binary){
    double thresh =  threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
    return thresh;
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


JNIEXPORT void JNICALL Java_edu_sfsu_cs_orange_ocr_OpencvNativeClass_BinarizeBG
(JNIEnv *, jclass, jlong BG, jlong input, jlong output)
{

Mat &gray = *(Mat*) input;
Mat &binary = *(Mat*) output;
Mat &grayBG = *(Mat*) BG;
grayBG = gray.clone();
binary = gray.clone();
// Estimate image background using Percentile filter
CBgEstimate bg;
bg.bgEstimatePercentile(gray, grayBG);
bg.binarizeByBackgroundOtsu(gray, grayBG, binary);

}

