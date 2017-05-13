/*
 * Copyright 2011 Robert Theis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package edu.sfsu.cs.orange.ocr;

import java.io.File;
import java.util.ArrayList;

import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;

import com.googlecode.leptonica.android.ReadFile;
import com.googlecode.tesseract.android.ResultIterator;
import com.googlecode.tesseract.android.TessBaseAPI;
import com.googlecode.tesseract.android.TessBaseAPI.PageIteratorLevel;

import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.utils.*;
import com.googlecode.leptonica.android.*;

import static java.lang.Math.max;
import static org.opencv.imgproc.Imgproc.INTER_LANCZOS4;

/**
 * Class to send OCR requests to the OCR engine in a separate thread, send a success/failure message,
 * and dismiss the indeterminate progress dialog box. Used for non-continuous mode OCR only.
 */
final class OcrRecognizeAsyncTask extends AsyncTask<Void, Void, Boolean> {

  //  private static final boolean PERFORM_FISHER_THRESHOLDING = false;
  //  private static final boolean PERFORM_OTSU_THRESHOLDING = false;
  //  private static final boolean PERFORM_SOBEL_THRESHOLDING = false;

  public static final boolean DEFAULT_TOGGLE_PREPROCESS = false;

  private CaptureActivity activity;
  private TessBaseAPI baseApi;
  private byte[] data;
  private int width;
  private int height;
  private OcrResult ocrResult;
  private long timeRequired;
  public boolean isPreprocessingActive;

  OcrRecognizeAsyncTask(CaptureActivity activity, TessBaseAPI baseApi, byte[] data, int width, int height) {
    this.activity = activity;
    this.baseApi = baseApi;
    this.data = data;
    this.width = width;
    this.height = height;
  }

  @Override
  protected Boolean doInBackground(Void... arg0) {
    long start = System.currentTimeMillis();
    Bitmap bitmap = activity.getCameraManager().buildLuminanceSource(data, width, height).renderCroppedGreyscaleBitmap();

    String textResult;

    try {
      //Imgcodecs.imwrite("/storage/emulated/0/DCIM/orig.jpg",image);
      //SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences();
      //isPreprocessingActive = prefs.getBoolean(PreferencesActivity.KEY_PREPROCESSING, OcrRecognizeAsyncTask.DEFAULT_TOGGLE_PREPROCESS);
      isPreprocessingActive = activity.getPreprocessingFlag();
      if(isPreprocessingActive) {
        //Mat image = new Mat();
        //Utils.bitmapToMat(bitmap,image);
        Mat gray = new Mat();
        Utils.bitmapToMat(bitmap, gray);
        Imgproc.cvtColor(gray, gray, Imgproc.COLOR_BGR2GRAY);
        //Utils.matToBitmap(gray,bitmap);

        /** Code to Resize **/
//
//    int m_LargerDim = 2000;
//
//    int maxDim = max(gray.cols(), gray.rows());
//    Mat rescaledImg;
//    int orows = gray.rows();
//    int ocols = gray.cols();
//    double scale = m_LargerDim / maxDim;
//    Imgproc.resize(gray, gray, Size, scale, scale, INTER_LANCZOS4);
//    Imgproc.resize();

        /********************/

        Mat background = new Mat();
        //Utils.bitmapToMat(bitmap,background);   //to test with BinarizeBG
        Mat finalimage = new Mat();
        //Utils.bitmapToMat(bitmap,finalimage);
//
//    finalimage.convertTo(finalimage,CvType.CV_8UC1);
//    background.convertTo(background, CvType.CV_8UC1);
        //OpencvNativeClass.BinarizeShafait(gray.getNativeObjAddr(),image.getNativeObjAddr());
        OpencvNativeClass.BinarizeBG(background.getNativeObjAddr(), gray.getNativeObjAddr(), finalimage.getNativeObjAddr());
//        Imgcodecs.imwrite("/storage/emulated/0/DCIM/original.jpg", gray);
//        Imgcodecs.imwrite("/storage/emulated/0/DCIM/binarized.jpg", finalimage);
//        Imgcodecs.imwrite("/storage/emulated/0/DCIM/background.jpg", background);

        Utils.matToBitmap(finalimage, bitmap);

        //Pix fimage = ReadFile.readBitmap(bitmap);
        //fimage = Binarize.otsuAdaptiveThreshold(fimage);


        //float angle = Skew.findSkew(fimage);
        //Log.i("Skew: ", Float.toString(angle));
        //double deg2rad = 3.14159265 / 180.;

        //fimage = Rotate.rotate(fimage, angle);

        //bitmap = WriteFile.writeBitmap(fimage);

        //Mat skewed = new Mat();

        //Utils.bitmapToMat(bitmap,skewed);
        //Imgcodecs.imwrite("/storage/emulated/0/DCIM/deskewed.jpg", skewed);
        //
        //double deg2rad = 3.14159265 / 180.0;
        //float fdeg2rad = (float)deg2rad;
        //angle=angle*fdeg2rad;
        //Log.i("Skew:(in degrees) ", Float.toString(angle));
        //fimage = Rotate.rotate(fimage, angle, true);
        //bitmap = WriteFile.writeBitmap(fimage);

      }

      baseApi.setImage(ReadFile.readBitmap(bitmap));
      textResult = baseApi.getUTF8Text();


      timeRequired = System.currentTimeMillis() - start;

      // Check for failure to recognize text
      if (textResult == null || textResult.equals("")) {
        return false;
      }

      ocrResult = new OcrResult();
      ocrResult.setWordConfidences(baseApi.wordConfidences());
      ocrResult.setMeanConfidence( baseApi.meanConfidence());
      ocrResult.setRegionBoundingBoxes(baseApi.getRegions().getBoxRects());
      ocrResult.setTextlineBoundingBoxes(baseApi.getTextlines().getBoxRects());
      ocrResult.setWordBoundingBoxes(baseApi.getWords().getBoxRects());
      ocrResult.setStripBoundingBoxes(baseApi.getStrips().getBoxRects());

      // Iterate through the results.
      final ResultIterator iterator = baseApi.getResultIterator();
      int[] lastBoundingBox;
      ArrayList<Rect> charBoxes = new ArrayList<Rect>();
      iterator.begin();
      do {
        lastBoundingBox = iterator.getBoundingBox(PageIteratorLevel.RIL_SYMBOL);
        Rect lastRectBox = new Rect(lastBoundingBox[0], lastBoundingBox[1],
                lastBoundingBox[2], lastBoundingBox[3]);
        charBoxes.add(lastRectBox);
      } while (iterator.next(PageIteratorLevel.RIL_SYMBOL));
      iterator.delete();
      ocrResult.setCharacterBoundingBoxes(charBoxes);

    } catch (RuntimeException e) {
      Log.e("OcrRecognizeAsyncTask", "Caught RuntimeException in request to Tesseract. Setting state to CONTINUOUS_STOPPED.");
      e.printStackTrace();
      try {
        baseApi.clear();
        activity.stopHandler();
      } catch (NullPointerException e1) {
        // Continue
      }
      return false;
    }
    timeRequired = System.currentTimeMillis() - start;
    ocrResult.setBitmap(bitmap);
    String[] temp = textResult.split("\n");
    if(temp.length!=0)
      textResult="";
    for(int i=0;i<temp.length;i++)
    {
      if(temp[i].length()!=0){
        if(i<temp.length-1){
          textResult = textResult + temp[i] + "\n";
        }
        else
          textResult = textResult + temp[i];
      }
    }
//    String textResult2=ParsingNativeClass.ParseAddress(textResult);
//    Log.d("Return parsing",textResult2);
    ocrResult.setViewtext(textResult);
    ocrResult.setText(textResult);
    ocrResult.setRecognitionTimeRequired(timeRequired);
    return true;
  }

  @Override
  protected void onPostExecute(Boolean result) {
    super.onPostExecute(result);

    Handler handler = activity.getHandler();
    if (handler != null) {
      // Send results for single-shot mode recognition.
      if (result) {
        Message message = Message.obtain(handler, R.id.ocr_decode_succeeded, ocrResult);
        message.sendToTarget();
      } else {
        Message message = Message.obtain(handler, R.id.ocr_decode_failed, ocrResult);
        message.sendToTarget();
      }
      activity.getProgressDialog().dismiss();
    }
    if (baseApi != null) {
      baseApi.clear();
    }
  }
}