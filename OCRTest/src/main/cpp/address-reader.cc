/** 
 * @file address-reader.cc 
 * This is the main implementation file of the class CAddressReader. 
 * 
 * @author Faisal Shafait (faisalshafait@gmail.com)  
 * 
 * @version 0.1  
 * 
 */ 
 
#include <iostream>
#include <stdexcept>
#include "address-reader.h" 
#include "opencv-tess-utils.h" 
#include "bg-estimate.h"
#include "../../../../../../../../../../usr/local/include/opencv2/core/types_c.h"
//#include "../../../../../../../../../../usr/local/include/tesseract/apitypes.h"
//#include "../../../../../../../../../../usr/local/include/opencv2/calib3d/calib3d.hpp"
//#include "../../../../../../../../../../usr/local/include/opencv2/core/core.hpp"
//#include "../../../../../../../../../../usr/local/include/opencv2/highgui/highgui.hpp"
//#include "../../../../../../../../../../usr/local/include/leptonica/allheaders.h"
//#include "../../../../../../../../../../usr/local/include/opencv2/imgproc/imgproc.hpp"
//#include "../../../../../../../../../../usr/local/include/tesseract/baseapi.h"
//#include "../../../../../../../../../../usr/local/include/opencv2/core/types_c.h"
//#include "../../../../../../../../../../usr/local/include/opencv2/gpu/gpu.hpp"
//#include "../../../../../../../../../../usr/local/include/c++/6.3.0/bits/regex_constants.h"
//#include "../../../../../../../../../../usr/local/include/c++/6.3.0/bits/regex.h"
//#include "../../../../../../../../../../usr/local/include/opencv2/core/Mat.hpp"
//#include "../../../../../../../../../../usr/include/opencv2/core/types.hpp"

#include <tesseract/strngs.h>
#include <regex>
#include <sstream>
 
using namespace std; 
using namespace tesseract; 
using namespace cv;

/** 
 * This function is responsible for reading an address image, processing it to extract 
 * all of its regions, and store them in the CAddress object passed to it as an argument. 
 *  
 * @param[in]     fname File name of the address image captured with the phone camera. 
 * @param[out]    address A reference to a address object that contains all extracted information from the input address image. 
 * @return True if successful, False if some fatal error occurs during address processing.
 * 
 */ 
 
bool CAddressReader::processAddress(string fname, CAddress &address){
    double tcStartPreProc = (double)getTickCount();
    address.m_regions.clear();
    address.m_ReadSuccess = false;
    string::size_type pAtPath = fname.find_last_of('/');  
    string filePath = fname.substr(0,pAtPath+1);
    string fileFullName = fname.substr(pAtPath+1); 
    string::size_type pAtExt = fileFullName.find_last_of('.');   // Find extension point 
    string fileBaseName = fileFullName.substr(0,pAtExt); 
    ofstream logFile; 
    // Set the debug images prefix to the full file name including the path
    m_debugImgPrefix = filePath + fileBaseName;
    // Read image 
    Mat imgMATColor = imread(fname.c_str()); 
    if(imgMATColor.empty()){
        fprintf(stderr, "Could not open image with OPENCV. Skipping image %s\n", fname.c_str()); 
        return false; 
    }
    
    // Rescale image to have the larger dimension (width) to m_LargerDim
    // to improve Tesseract's result
    int maxDim = max(imgMATColor.cols, imgMATColor.rows); 
    Mat rescaledImg; 
	double scale = m_LargerDim / maxDim;
	resize(imgMATColor, imgMATColor, Size(), scale, scale, INTER_LANCZOS4);

    // Convert image to greyscale
    Mat imgMATGray;
    cvtColor(imgMATColor, imgMATGray, COLOR_BGR2GRAY);
    
    // Convert image to binary
    Mat imgShafait;
    binarizeShafait(imgMATGray, imgShafait, 50, 0.1);
    if(m_debug){
        string imgFileName = m_debugImgPrefix + "-00a-binary-shafait.png";
        imwrite(imgFileName, imgShafait);
    }



    Mat imgBGgrey, imgBGbin;
    binarizeBG(imgMATGray, imgBGgrey, imgBGbin);
    if(m_debug){
        string binBGImgFileName = m_debugImgPrefix + "-00b-bin-BG.png";
        imwrite(binBGImgFileName, imgBGbin);
        string grayBGImgFileName = m_debugImgPrefix + "-00c-BG.png";
        imwrite(grayBGImgFileName, imgBGgrey);
    }
    
//    // Convert binarized image from OpenCV to Leptonica format
//    Pix *imgPIXfMAT = MAT2PIXBinary(imgShafait);
//
//    Pix *imgPIXBin = binarizeLeptonica(imgMATGray);
//    if(m_debug){
//        string binLeptImgFileName = m_debugImgPrefix + "-00d-bin-Lept.png";
//        pixWrite(binLeptImgFileName.c_str(), imgPIXBin, IFF_PNG);
//    }
    

    // Identify rotation angle and fix the rotation
    float angle, conf;
    pixFindSkew(imgPIXfMAT, &angle, &conf);
    float deg2rad = 3.14159265 / 180.;
    Pix *pixRotated = pixRotate(imgPIXfMAT, deg2rad * angle, L_ROTATE_SAMPLING,
                                L_BRING_IN_WHITE, 0, 0);

    // to inspect the rotated image for debugging
    if(m_debug){
        string imgFileName = m_debugImgPrefix + "-01-rotated.png";
        pixWrite(imgFileName.c_str(), pixRotated, IFF_PNG);
    }

    
    double tcEndPreProc = (double)getTickCount();

    // Initialize tesseract-ocr with English, without specifying tessdata path 
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if ( api->Init(NULL, "eng") ){
        fprintf(stderr, "Could not initialize Tesseract OCR.\n"); 
        return false; 
    } 

    // Tesseract First Run on Deskewed Image to get address layout
    api->SetPageSegMode(tesseract::PSM_AUTO);
    api->SetImage(pixRotated);
    api->SetVariable("textord_words_default_minspace","0.8");
    api->SetVariable("textord_words_min_minspace","0.8");
    api->SetVariable("textord_words_default_nonspace","0.4");
    api->SetVariable("tessedit_char_whitelist", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-:(),/#*");
    Boxa* wordBoxes = api->GetComponentImages(tesseract::RIL_WORD, true, NULL, NULL);
    
    vector<Rect> cvWordBoxes; 
 
    int nRects = (wordBoxes == NULL) ? 0 : wordBoxes->n;
    for (int i = 0; i < nRects; i++) {
        BOX* box = boxaGetBox(wordBoxes, i, L_CLONE);
        cvWordBoxes.push_back( Rect(box->x, box->y, box->w, box->h) );
        boxDestroy(&box);
    }
    
    double tcStartReadaddress = (double)getTickCount(); 

    // Read text corresponding to each detected region using the specified variation
    // of the reading algorithm
    wholeImageRecognition(api, address);
    //wordLevelRecognition(api, cvWordBoxes, address);
    //textLineRecognition(api, cvWordBoxes, address);
    
    double tcEnd = (double)getTickCount();
    if(m_debug){ // Paint word and line boxes as well as print timing information
        Mat wordImg = imgMATColor.clone();
        Mat rWordImg;
        rotate(wordImg, -angle, rWordImg);
        Mat lineImg = wordImg.clone();

        for(int i=0; i<nRects; i++){
            rectangle(rWordImg, cvWordBoxes[i], Scalar(200,128,0), 3*m_lineWidth);
        }
        
        vector<Rect> concomps;
        Mat imgMATrotated;
        PIX2MATGray(pixRotated, imgMATrotated);
        findConComp(imgMATrotated, concomps, 1.0, 1.0, 10.0, 8);
        for(int i=0; i<concomps.size(); i++){
            rectangle(rWordImg, concomps[i], Scalar(0,0,255), m_lineWidth);
        }
        string wordImgFileName = m_debugImgPrefix + "-02-words.png";
        imwrite(wordImgFileName.c_str(), rWordImg);

        printf("\n============================================\n");
        printf("Timing Profile\n");
        printf("============================================\n");
        printf("Total address Reading Time: %.3f\n", (tcEnd - tcStartPreProc) / getTickFrequency() );
        printf("OpenCV Preprocessing: %.3f\n", (tcEndPreProc - tcStartPreProc) / getTickFrequency() );
        printf("Address segmentation: %.3f\n", (tcStartReadaddress - tcEndPreProc) / getTickFrequency() );
        printf("Tesseract OCR: %.3f\n", (tcEnd - tcStartReadaddress) / getTickFrequency() );
        printf("============================================\n");
    }
    address.m_ReadSuccess = true;
    
    // Release memory
    api->End();
    delete api;
    pixDestroy(&imgPIXfMAT);
    pixDestroy(&pixRotated);
    boxaDestroy(&wordBoxes);
    return true; 
} 

/** 
 * This function implements word level recognition using Tesseract. 
 * First, Tesseract is run on the supplied word bounding boxes to get
 * the text corresponding to each word.
 * The words extracted from Tesseract are merged to obtain text-lines. 
 * The logic used for obtaining text-lines is to merge two consecutive 
 * words if their bounding boxes overlap along the y-axis, and the gap between them is 
 * smaller than a threshold based on the heights of the two words.
 *  
 * @param[in]     api Pointer to Tesseract's API that is initialized with the input image. 
 * @param[in]     wordBoxes A vector of word level bounding boxes. 
 * @param[out]    address Recognition result by running Tesseract on individual words
 *                and merging the resulting word strings to get text-lines. 
 */ 
bool CAddressReader::wordLevelRecognition(TessBaseAPI *api, vector<Rect> &wordBoxes,
                                          CAddress &address){
    int offset = 1; // Make an offset around the image so that text is not clipped from corners
    int nWords = wordBoxes.size();
    // Feed individual word boxes to Tesseract for recognition
    vector<string> wordStrings;
    for(int i=0; i<nWords; i++){
        Rect regionBox = wordBoxes[i];
        // Set Tesseract to read a single word
        api->SetRectangle(regionBox.x - offset, regionBox.y - offset, regionBox.width + 2*offset, regionBox.height + 2*offset); 
        //api->SetPageSegMode(PSM_SINGLE_WORD);
        char *outText = api->GetUTF8Text(); 
        string str(outText);
        str.erase(remove(str.begin(), str.end(), '\n'), str.end());
        wordStrings.push_back(str);
        delete[] outText;
    }
    
    if(wordBoxes.size() != wordStrings.size()){
        fprintf(stderr, "Error: cvWordBoxes.size() != wordStrings.size()\n");
        return false;
    }
    // Merge words to construct text-lines
    bool newTextLine = true; 
    Rect prevWord;
    CRegion region;
    string txt;
    for(int i=0; i<nWords; i++){
        Rect currWord = wordBoxes[i];
        string currTxt = wordStrings[i];
        if(!newTextLine){ 
            int hGap = currWord.x - prevWord.x - prevWord.width; 
            int hGapThresh = m_wordGapFactor * max(currWord.height, prevWord.height); 
            bool vOverlap = false; 
            if( ( (currWord.y <= prevWord.y) && 
                  (currWord.y + currWord.height > prevWord.y) ) ||
                ( (prevWord.y <= currWord.y) && 
                  (prevWord.y + prevWord.height > currWord.y) ) )
                vOverlap = true; 
            if( vOverlap && (hGap > 0) && (hGap < hGapThresh) ){ 
                region.m_bbox = region.m_bbox | currWord;
                region.m_text = region.m_text + " " + currTxt;
                prevWord = currWord;
             } else{
                   region.m_text = region.m_text + "\n";
                   address.m_regions.push_back(region);
                   newTextLine = true;
            } 
        } 
        if(newTextLine){ 
            region.m_bbox = currWord;
            region.m_text = currTxt;
            newTextLine = false; 
            prevWord = currWord; 
            continue; 
        } 
    } 
    region.m_text = region.m_text + "\n";
    address.m_regions.push_back(region);
    return true;
}
                    
/** 
 * This function implements text-line level recognition using Tesseract.
 * The main idea is to first merge consective words based on the same algorithms
 * as used in wordLevelRecognition to obtain text lines, and then use Tesseract
 * to directly recognize the text-lines. Using this approach, Tesseract is 
 * better able to estimate geometric text-line properties and hence disambiguate
 * certain characters. The most notable effect is on extraction of commas
 * separating the first name and the last name - which are usually lost in
 * word level recognition.
 *  
 * @param[in]     api Pointer to Tesseract's API that is initialized with the input image. 
 * @param[in]     wordBoxes A vector of word level bounding boxes. 
 * @param[out]    address Recognition result by first merging individual words to
 *                to get text-lines and then running Tesseract on the text-lines.
 */ 
bool CAddressReader::textLineRecognition(TessBaseAPI *api, vector<Rect> &wordBoxes,
                                          CAddress &address){
    // Merge words to construct text-lines
    bool newTextLine = true; 
    Rect prevWord, textline;
    vector<Rect> textLines;
    string txt;
    int nWords = wordBoxes.size();
    for(int i=0; i<nWords; i++){
        Rect currWord = wordBoxes[i];
        if(!newTextLine){ 
            int hGap = currWord.x - prevWord.x - prevWord.width; 
            int hGapThresh = m_wordGapFactor * max(currWord.height, prevWord.height); 
            bool vOverlap = false; 
            if( ( (currWord.y <= prevWord.y) && 
                  (currWord.y + currWord.height > prevWord.y) ) ||
                ( (prevWord.y <= currWord.y) && 
                  (prevWord.y + prevWord.height > currWord.y) ) )
                vOverlap = true; 
            if( vOverlap && (hGap > 0) && (hGap < hGapThresh) ){ 
                textline |= currWord;
                prevWord = currWord;
             } else{
                   textLines.push_back(textline);
                   newTextLine = true;
            } 
        } 
        if(newTextLine){ 
            textline = currWord;
            newTextLine = false; 
            prevWord = currWord; 
            continue; 
        } 
    } 
    textLines.push_back(textline);
    
    int nTLines = textLines.size();
    int offset = 2; // Make an offset around the image so that text is not clipped from corners
    // Feed individual word boxes to Tesseract for recognition
    vector<string> wordStrings;
    for(int i=0; i<nTLines; i++){
        CRegion region;
        Rect regionBox = textLines[i];
        region.m_bbox = regionBox;
        // Set Tesseract to read a single text-line
        api->SetPageSegMode(tesseract::PSM_AUTO);
        api->SetRectangle(regionBox.x - offset, regionBox.y - offset, regionBox.width + 2*offset, regionBox.height + 2*offset); 
        char *outText = api->GetUTF8Text(); 
        string str(outText);
        //str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        region.m_text = str;
        address.m_regions.push_back(region);
        delete[] outText;
    }
    
    return true;
}

/** 
 * This function implements whole image recognition using Tesseract
 *  
 * @param[in]     api Pointer to Tesseract's API that is initialized with the input image. 
 * @param[out]    address Recognition result. Since layout analysis is done internally,
 *                the whole image is considered one region.
 */ 
bool CAddressReader::wholeImageRecognition(TessBaseAPI *api, CAddress &address){
    // Get OCR result
    char *outText = api->GetUTF8Text();
    string imageText(outText);

    // Push OCR result to a single region
    CRegion oneR;
    oneR.m_bbox = Rect();
    oneR.m_text = imageText;

    // Insert recognition result into CAddress object    
    address.m_regions.push_back(oneR);
    address.m_ReadSuccess = true;

    // Destroy used object and release memory
    delete[] outText;
    return true;
}
                    
/** 
 * This function implements global image thresholding using Otsu's algorithm 
 *  
 * @param[in]     gray Gray scale input image as an OpenCV Mat object. 
 * @param[out]    binary Output image binarized using Otsu's method. 
 */ 
void CAddressReader::binarizeOtsu(Mat &gray, Mat &binary){ 
    threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU); 
} 
 
/** 
 * This function implements local adaptive thresholding algorithms from: 
 * Faisal Shafait, Daniel Keysers, Thomas M. Breuel. "Efficient Implementation  
 * of Local Adaptive Thresholding Techniques Using Integral Images",  
 * SPIE Document Recognition and Retrieval XV, DRR'08, San Jose, CA, USA. Jan. 2008 
 *  
 * @param[in]     gray Gray scale input image as an OpenCV Mat object. 
 * @param[out]    binary Output image binarized using Shafait's method. 
 * @param[in]     w local square window side length to compute adaptive threshold. 
 * @param[in]     k Gray level sensititivity parameter. Lower values of k result in whiter images (fewer black pixels) and vice versa. 
 */ 
void CAddressReader::binarizeShafait(Mat &gray, Mat &binary, int w, double k){ 
    Mat sum, sumsq; 
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
            if (gray.at<uchar>(i,j) > threshold) 
                binary.at<uchar>(i,j) = 255; 
            else 
                binary.at<uchar>(i,j) = 0; 
        } 
    } 
} 
 
/** 
 * This function calls Leptonica's adaptive thresholding algorithm that participated
 * in ICDAR 2009 document image binarization contest
 *  
 * @param[in]     gray Gray scale input image as an OpenCV Mat object. 
 * @return        binary Output image (Pix *) binarized using Leptonica. 
 *
 */ 
Pix * CAddressReader::binarizeLeptonica(cv::Mat &gray){
    Pix *imgPIXGray = MAT2PIXGray(gray);
    int threshval;
    Pix *imgPIX = pixMaskedThreshOnBackgroundNorm(imgPIXGray, 
                        NULL, 10, 15, 100, 50, 2, 2, 0.10, &threshval);
    if(!imgPIX)
        imgPIX = pixCopy(NULL, imgPIXGray);
    pixDestroy(&imgPIXGray);
    return imgPIX;
}

/** 
 * This function implements Percentile based adaptive thresholding 
 * algorithm published in CBDAR 2015 paper:
 * 
 *  
 * @param[in]     gray Gray scale input image as an OpenCV Mat object. 
 * @param[out]    grayBG Extracted page background as a greyscale image. 
 * @param[out]    binary Output image binarized using Percentile method. 
 *
 */ 
void CAddressReader::binarizeBG(cv::Mat &gray, cv::Mat &grayBG, cv::Mat &binary){
    grayBG = gray.clone();

    // Estimate image background using Percentile filter
    CBgEstimate bg;
    bg.bgEstimatePercentile(gray, grayBG);
    bg.binarizeByBackgroundOtsu(gray, grayBG, binary);
}

/** 
 * This function computes connected components from an input binary image 
 *  
 * @param[in]     binary Input binary image with background labeled as 0 and foreground as 1. 
 * @param[out]    blobs Output vector of connected components containing all pixels belonging to each component. 
 * @param[out]    blob_rects Output vector of connected component bounding boxes. 
 */ 
void CAddressReader::conComps(const Mat &binary, vector <vector<Point2i> > &blobs, vector < Rect > &blob_rects){ 
    blobs.clear(); 
    blob_rects.clear(); 
    // Fill the label_image with the blobs 
    // 0  - background 
    // 1  - unlabelled foreground 
    // 2+ - labelled foreground 
 
    cv::Mat label_image; 
    binary.convertTo(label_image, CV_32FC1); // weird it doesn't support CV_32S! 
 
    int label_count = 2; // starts at 2 because 0,1 are used already 
 
    for(int y=0; y < binary.rows; y++) { 
        for(int x=0; x < binary.cols; x++) { 
            if((int)label_image.at<float>(y,x) != 1) { 
                continue; 
            } 
 
            cv::Rect rect; 
            cv::floodFill(label_image, cv::Point(x,y), cv::Scalar(label_count), &rect, cv::Scalar(0), cv::Scalar(0), 4); 
 
            std::vector <cv::Point2i> blob; 
 
            for(int i=rect.y; i < (rect.y+rect.height); i++) { 
                for(int j=rect.x; j < (rect.x+rect.width); j++) { 
                    if((int)label_image.at<float>(i,j) != label_count) { 
                        continue; 
                    } 
 
                    blob.push_back(cv::Point2i(j,i)); 
                } 
            } 
            blobs.push_back(blob); 
 
            label_count++; 
        } 
    } 
 
    for(size_t i=0; i < blobs.size(); i++) { 
        int top = 10000, bottom = -1, left = 10000, right = -1; 
        for(size_t j=0; j < blobs[i].size(); j++) { 
            int x = blobs[i][j].x; 
            int y = blobs[i][j].y; 
            if (x < left) 
                left = x; 
            if (x > right) 
                right = x; 
            if (y < top) 
                top = y; 
            if (y > bottom) 
                bottom = y; 
        } 
        int w = right - left; 
        int h = bottom - top; 
        blob_rects.push_back(Rect(left, top, w, h)); 
    } 
}

/** 
 * This function deskews a document image with the specified rotation angle. 
 *  
 * @param[in]     inImg Input image. 
 * @param[in]     angle Rotation angle in degrees. 
 * @param[out]    outImg Output deskewed image. 
 */ 
void CAddressReader::rotate(Mat &inImg, double angle, Mat &outImg){
    //int len = std::max(inImg.cols, inImg.rows);
    Point2f pt(inImg.cols/2., inImg.rows/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(inImg, outImg, r, inImg.size());
}

bool CAddressReader::parseAddress(CAddress &raw, Address &parsed){
    bool result = false;
    if(raw.m_ReadSuccess){
        string rawOcrOutput = "";
        for(int i=0; i<raw.m_regions.size(); i++){
            string addressLine = raw.m_regions[i].m_text;
            if(addressLine.length() >= 3)
               rawOcrOutput += addressLine;
        }
        result = parseAddress(rawOcrOutput, parsed);
    } 
    if (!result) {
        cout << "Address parsing failed";
    }
    return result;
}

bool CAddressReader::parseAddress(std::string rawOcrOutput, Address &parsed){
    std::vector<std::string>lines = regexSplit(rawOcrOutput,"\n");
    int matched;
    std::string location="";
    //assign name field
    parsed.name=lines[0];
    //find the line containing city, state, zip
    matched=findLine(rawOcrOutput);
    if(matched>6){
        return 0;
    }
    //parse the cityStateZip line to assign City, State, Zip code
    assignValues(lines[matched],parsed);
    if(!(isNumber(parsed.zip))){
        return 0;
    }
    //the lines between cityZipState line and Name line carry location
    for (int i=1; i<matched; i++){
        location=location+lines[i]+" ";
    }
    //assign location feild
    parsed.location=location;
    //check if there is a line after cityStateZip line
    if(lines.size()-1>matched && lines[lines.size()-1].length()<=10){
            parsed.country = lines[lines.size() - 1];
        }
    else{
        parsed.country="USA";
    }
    //display the struct
    parsed.println(cout);
    return true;
}
std::vector<std::string> CAddressReader::regexSplit(const std::string &s, std::string rgx_str) {
    std::vector<std::string> elems;
    std::regex rgx (rgx_str);
    std::sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
    std::sregex_token_iterator end;
    while (iter != end)  {
        elems.push_back(*iter);
        ++iter;
    }
    return elems;
}
int CAddressReader::findLine(std::string ocrAddress){
    std::vector<std::string>lines = regexSplit(ocrAddress,"\n");
    std::regex rgx("(.*) ([A-Z][A-Z]) ([0-9-]+)", regex_constants::ECMAScript | regex_constants::icase );
    std::smatch pieces_match;
    int matched=0;
    int j=0;
    for (int i=0; i<lines.size(); i++) {
        if (std::regex_search(lines[i], pieces_match, rgx)) {
        return i;
        }
    }
}
void CAddressReader::assignValues(std::string line, Address& myAddress){
    std::string arr[4];
    int i = 0;
    std::stringstream ssin(line);
    while (ssin.good() && i < 4){
        ssin >> arr[i];
        ++i;
    }
    myAddress.city=arr[0];
    myAddress.state=arr[1];
    myAddress.zip=arr[2];
}
bool CAddressReader::isNumber(std::string input){
    std::regex rgx("([^\\d]|^)\\d{4,5}([^\\d]|$)");
    std::smatch pieces_match;
    int matched=0;
    int j=0;
        if (std::regex_search(input, pieces_match, rgx)) {
            return 1;
        }
    return 0;
}
