package edu.sfsu.cs.orange.ocr.language;

import android.media.ImageReader;
import android.util.Log;

import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;
//import org.opencv.highgui.*;

import static java.lang.Math.sqrt;
import static org.opencv.core.CvType.CV_64F;
import static org.opencv.core.CvType.CV_64FC1;


/**
 * Created by skrk on 4/16/17.
 */

public class TestShafait {



    public void binarizeShafait(Mat gray, Mat binary, int w, double k){
        Imgproc imgproc = new Imgproc();
        //gray.convertTo(gray,CV_64F);
        Mat sum = new Mat();
        Mat sumsq = new Mat();
        gray.copyTo(binary);
        int half_width = w >> 1;
        //imgproc.integral2(gray, sum, sumsq, CV_64F, CV_64F);
        System.out.println(gray.get(1,0));
//        for(int i=0; i<gray.rows(); i ++){
//            for(int j=0; j<gray.cols(); j++){
//                int x_0 = (i > half_width) ? i - half_width : 0;
//                int y_0 = (j > half_width) ? j - half_width : 0;
//                int x_1 = (i + half_width >= gray.rows()) ? gray.rows() - 1 : i + half_width;
//                int y_1 = (j + half_width >= gray.cols()) ? gray.cols() - 1 : j + half_width;
//                Log.i("get", sum.get(x_0, y_0).toString());
//                double area = (x_1-x_0) * (y_1-y_0);
//                double mean = (sum.get(x_0,y_0)[0] + sum.get(x_1,y_1)[0] - sum.get(x_0,y_1)[0] - sum.get(x_1,y_0)[0]) / area;
//                double sq_mean = (sumsq.get(x_0,y_0)[0] + sumsq.get(x_1,y_1)[0] - sumsq.get(x_0,y_1)[0] - sumsq.get(x_1,y_0)[0]) / area;
//                double stdev = sqrt(sq_mean - (mean * mean));
//                double threshold = mean * (1 + k * ((stdev / 128) -1) );
//                if (gray.get(i,j)[0] > threshold)
//                    binary.get(i,j)[0] = 255;
//                else
//                    binary.get(i,j)[0] = 0;
//            }
//        }
    }

    public static void main(String args[]){
        //Mat image = new Mat(2,2,CV_64F);


    }
}
