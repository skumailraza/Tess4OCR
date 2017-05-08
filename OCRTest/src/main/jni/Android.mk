LOCAL_PATH := $(call my-dir)

	include $(CLEAR_VARS)

	#opencv
	OPENCVROOT:= ../OpenCV-android-sdk

	OPENCV_CAMERA_MODULES:=on
	OPENCV_INSTALL_MODULES:=on
	OPENCV_LIB_TYPE:=SHARED
	include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk


	LOCAL_SRC_FILES := edu_sfsu_cs_orange_ocr_OpencvNativeClass.cpp

	LOCAL_LDLIBS += -llog
	LOCAL_MODULE := MyOpencvLibs


	include $(BUILD_SHARED_LIBRARY)

    include $(CLEAR_VARS)

	LOCAL_SRC_FILES := edu_sfsu_cs_orange_ocr_ParsingNativeClass.cpp
	LOCAL_LDLIBS += -llog
    LOCAL_MODULE := MyParsingLibs


    include $(BUILD_SHARED_LIBRARY)


