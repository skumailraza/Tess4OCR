APP_STL := gnustl_static
	APP_CPPFLAGS := -frtti -fexceptions -fno-stack-protector
	APP_ABI := armeabi-v7a
	APP_PLATFORM := android-16
	APP_CPPFLAGS += -std=c++11
	LOCAL_C_INCLUDES += ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/include
