/** @mainpage CAddressReader: Read an address captured with a 
*   smart phone camera via Tesseract OCR and OpenCV
*   @par Description:
*   This poject implements the functionality to automatically extract 
*   text information from addresses using Tesseract OCR. Various 
*   document image processing algorithms are implemented to handle 
*   size variations and non-uniform illumination. 
*/

/** 
 * @file address-reader.h 
 * This is the main header file of the project containing class definitions. 
 * 
 * @author Faisal Shafait (faisalshafait@gmail.com)  
 * 
 * @version 0.1  
 * 
 */ 
 
#ifndef ADDRESS_READER_H 
#define ADDRESS_READER_H 
 
#include "stdio.h" 
#include <string> 
#include <fstream> 
// Tesseract headers 
#include <tesseract/baseapi.h> 
#include <leptonica/allheaders.h> 
// Open CV headers 
#include "opencv2/core/core.hpp" 
#include "opencv2/highgui/highgui.hpp" 
#include "opencv2/imgproc/imgproc.hpp"
//#include "../../../../../../../../../../usr/local/include/eigen3/Eigen/StdVector"
//#include "../../../../../../../../../../usr/local/include/tesseract/baseapi.h"
//#include "../../../../../../../../../../usr/local/include/opencv2/calib3d/calib3d.hpp"

/** 
 * @class CRegion 
 * This class captures information about an individual region of a address. 
*/ 
 
class CRegion{ 
public: 
    /// Image coordinates of the element's bounding box in pixels
    cv::Rect m_bbox;  
    /// ASCII transcription of the element as recognized by Tesseract 
    std::string m_text;   
}; 
 
/** 
 * @class CAddress
 * This class stores all regions of an address. 
*/ 
 
class CAddress{ 
public: 
    /// Array of address elements
    std::vector<CRegion> m_regions;  
    /// Boolean flag, set to False if processing address fails 
    bool m_ReadSuccess;           
}; 

struct Address {
    std::string name;
    std::string location;
    std::string city;
    std::string state;
    std::string zip;
    std::string country;
    void println(std::ostream &file){
        file<<"name: "<<name<<std::endl;
        file<<"location: "<<location<<std::endl;
        file<<"city: "<<city<<std::endl;
        file<<"state: "<<state<<std::endl;
        file<<"zip: "<<zip<<std::endl;
        file<<"country: "<<country<<std::endl;
    }
};

/** 
 * @class CAddressReader 
 * This class contains all image processing functionality for reading an address. 
*/ 
 
class CAddressReader{ 
public: 
    /// Main method to read an addresss 
    bool processAddress(std::string filename, CAddress &address); 
    bool parseAddress(CAddress &raw, Address &parsed);
    CAddressReader(){
        m_LargerDim = 2000;
        m_debugImgPrefix = "";
        m_lineWidth = 2;
        m_wordGapFactor = 2.0;
        m_debug = 1;
    }
    ~CAddressReader(){}
    
private:
    /// Minimum size (px) of the larger dimension for rescaling (default = 2000)
    double m_LargerDim;  
    /// Prefix file name of the debug image (default = <inputfilename>)
    std::string m_debugImgPrefix; 
    /// Line thickness (Width) of painted rectangles for debugging layout analysis (default = 2)
    int m_lineWidth;  
    /// Multiplier to obtain a horizontal gap threshold between two words based on the height of their bounding boxes
    double m_wordGapFactor; 
    /// Debug level
    int m_debug; 
    
    /// Global thresholding using Otsu algorithm 
    void binarizeOtsu(cv::Mat &gray, cv::Mat &binary); 
    /// Local adaptive thresholding using Shafait's algorithm 
    void binarizeShafait(cv::Mat &gray, cv::Mat &binary, int w, double k); 
    /// Local adaptive thresholding using Leptonica algorithm 
    Pix * binarizeLeptonica(cv::Mat &gray);
    /// Local adaptive thresholding using Percentile algorithm 
    void binarizeBG(cv::Mat &gray, cv::Mat &grayBG, cv::Mat &binary); 
    /// Connected component analysis
    void conComps(const cv::Mat &binary, std::vector <std::vector<cv::Point2i> > &blobs, std::vector < cv::Rect > &blob_rects); 
    /// Rotate the image using the specified rotation angle
    void rotate(cv::Mat &inImg, double angle, cv::Mat &outImg); 
    /// Word level recognition
    bool wordLevelRecognition(tesseract::TessBaseAPI *api, std::vector<cv::Rect> &wordBoxes,
                              CAddress &address); 
    /// Text-line level recognition
    bool textLineRecognition(tesseract::TessBaseAPI *api, std::vector<cv::Rect> &wordBoxes,
                              CAddress &address); 
    /// Image level recognition
    bool wholeImageRecognition(tesseract::TessBaseAPI *api, CAddress &address); 
    
    /// Address Parsing
    std::vector<std::string> regexSplit(const std::string &s, std::string rgx_str);
    int findLine(std::string);
    void assignValues(std::string, Address&);
    bool parseAddress(std::string rawOcrOutput, Address &parsed);
    bool isNumber(std::string);
    
}; 
  
#endif 
  
