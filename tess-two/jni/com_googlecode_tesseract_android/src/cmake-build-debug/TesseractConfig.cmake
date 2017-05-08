# ===================================================================================
#  The Tesseract CMake configuration file
#
#             ** File generated automatically, do not modify **
#
#  Usage from an external project:
#    In your CMakeLists.txt, add these lines:
#
#    find_package(Tesseract REQUIRED)
#    include_directories(${Tesseract_INCLUDE_DIRS})
#    target_link_libraries(MY_TARGET_NAME ${Tesseract_LIBRARIES})
#
#    This file will define the following variables:
#      - Tesseract_LIBRARIES             : The list of all imported targets for OpenCV modules.
#      - Tesseract_INCLUDE_DIRS          : The Tesseract include directories.
#      - Tesseract_VERSION               : The version of this Tesseract build: "3.05"
#      - Tesseract_VERSION_MAJOR         : Major version part of Tesseract_VERSION: "3"
#      - Tesseract_VERSION_MINOR         : Minor version part of Tesseract_VERSION: "05"
#
# ===================================================================================

include(${CMAKE_CURRENT_LIST_DIR}/TesseractTargets.cmake)

# ======================================================
#  Version variables:
# ======================================================

SET(Tesseract_VERSION           3.05)
SET(Tesseract_VERSION_MAJOR     3)
SET(Tesseract_VERSION_MINOR     05)

# ======================================================
# Include directories to add to the user project:
# ======================================================

# Provide the include directories to the caller
set(Tesseract_INCLUDE_DIRS /home/srq/Installations/tesseract/api;/home/srq/Installations/tesseract/ccmain;/home/srq/Installations/tesseract/ccstruct;/home/srq/Installations/tesseract/ccutil)

# ====================================================================
# Link libraries:
# ====================================================================

set(Tesseract_LIBRARIES tesseract)
