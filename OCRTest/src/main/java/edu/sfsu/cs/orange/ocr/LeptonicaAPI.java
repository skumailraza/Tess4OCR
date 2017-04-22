package edu.sfsu.cs.orange.ocr;

/**
 * Created by skrk on 4/22/17.
 */
import android.graphics.PixelFormat;

import com.googlecode.leptonica.android.Skew;
import com.googlecode.leptonica.android.*;
import com.googlecode.leptonica.android.Pix;

import org.opencv.core.Mat;

import static org.opencv.core.CvType.CV_8UC1;

public class LeptonicaAPI {

//    Pix MAT2PIXBinary(Mat imgMAT){
//        assert(imgMAT.depth() == CV_8UC1);
//        Pix imgPIX;
//        int w = imgMAT.cols();
//        int h = imgMAT.rows();
//        int d = 1;
//
//        if ((imgPIX = pixCreate(w, h, d)) == null){
//            return imgPIX;
//        }
//
//        // Get a pointer to target image data
//        int data = pixGetData(imgPIX);
//        int wpl = pixGetWpl(imgPIX);
//        int line;
//
//        // Set target image data bits individually to the values of the source image.
//        // Note the inversion due to 0 assigned to background in 1bpp image.
//        for(int y=0; y<h; y++) {
//            line = data + y * wpl;
//            for(int x=0; x<w; x++) {
//                int pixval = imgMAT.put(y,x);
//                if(pixval == 1)
//                    SET_DATA_BIT(line, x);
//            }
//        }
//        return imgPIX;
//    }

}
