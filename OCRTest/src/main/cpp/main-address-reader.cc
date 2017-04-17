/** 
 * @file main-address-reader.cc 
 * This is the the top level executable file for the class CAddressReader. 
 * As input, it takes a text file containing file names of all address images to be processed.
 * The OCR output is saved in the file [input]-ocr.txt
 * 
 * @author Faisal Shafait (faisalshafait@gmail.com)  
 * 
 * @version 0.1  
 * 
 */ 

#include "address-reader.h"
//#include "../../../../../../../../../../usr/local/include/tesseract/baseapi.h"

using namespace std; 
using namespace tesseract;
using namespace cv;

int main(int argc, char **argv)
{
    ifstream batchFile;
    string batchFileName;
    if(argc == 2)
        batchFileName = argv[1];
    else{
        batchFileName = "../samples.txt";
    }
    batchFile.open(batchFileName.c_str());

    if (batchFile.is_open())
    {
        ofstream ocrBatchFile;
        string::size_type pAtExtB = batchFileName.find_last_of('.');   // Find extension point
        string fileBaseName = batchFileName.substr(0,pAtExtB);
        string ocrBatchFileName = fileBaseName + "-ocr.txt";
        ocrBatchFile.open(ocrBatchFileName.c_str(), std::ios::trunc);
        while ( batchFile.good() )
        {
            string fname;
            getline (batchFile,fname);
            if(fname.length() < 2) continue;
            fprintf(stderr,"Processing Image: %s\n", fname.c_str());
            CAddressReader addressReader;
	        CAddress address;
	        Address parsed;
            addressReader.processAddress(fname, address);
            if(ocrBatchFile.is_open()){
                ocrBatchFile << "\n=========================================================\n";
                ocrBatchFile << fname << endl;
                ocrBatchFile << "=========================================================\n";
                if(address.m_ReadSuccess){
                    for(int i=0; i<address.m_regions.size(); i++){
                        string addressLine = address.m_regions[i].m_text;
                        if(addressLine.length() >= 3)
                            ocrBatchFile << addressLine.c_str();
                    }
                    addressReader.parseAddress(address, parsed);
                    parsed.println(ocrBatchFile);
                } else
                    ocrBatchFile << "address Reading Failed!\n";
                ocrBatchFile << "\n*********************************************************\n\n";
            }
            else
                fprintf(stderr, "Could not open file: %s \n", ocrBatchFileName.c_str());
        }
    } else {
        fprintf(stderr, "Could not open batch file: %s\n", batchFileName.c_str());
    }
    batchFile.close();
    return 0;
}
 
