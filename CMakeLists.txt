cmake_minimum_required(VERSION 2.8)
project( Reidentification)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FIND_PACKAGE( OpenCV REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)

include_directories(${GTK2_INCLUDE_DIRS})
link_directories(${GTK2_LIBRARY_DIRS})
# Add other flags to the compiler
add_definitions(${GTK2_CFLAGS_OTHER})

find_package (Threads REQUIRED)

add_executable( Reidentification main.cpp main.h models/Human.cpp models/Human.h descriptors/HistDescriptor.cpp
        descriptors/HistDescriptor.h descriptors/GaborDescriptor.cpp descriptors/GaborDescriptor.h
        descriptors/FilterBank.cpp descriptors/FilterBank.h descriptors/Kalman.cpp descriptors/Kalman.h
        descriptors/OpticalFlow.cpp descriptors/OpticalFlow.h UIFunctions.h UIFunctions.cpp ReidentificationAlg.cpp
        ReidentificationAlg.h ReidentificationThread.cpp ReidentificationThread.h HOGTrainer.cpp HOGTrainer.h
        haar/HaarDetector.cpp haar/HaarDetector.h ReidentificationData.h descriptors/GaborData.h descriptors/HistData.h
        descriptors/KalmanData.h descriptors/OpticalFlowData.h haar/HaarData.h models/HumanData.h MultiView.cpp MultiView.h View.cpp View.h ReidentificationData.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries( Reidentification ${OpenCV_LIBS} ${GTK2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})