cmake_minimum_required(VERSION 2.8)
project( arPet )
find_package( OpenCV REQUIRED )

include_directories(include)
set(HEADER_FILES include/PointCloud.hpp include/CameraCalibration.hpp
                 include/GeometryTypes.hpp include/DrawingContext.hpp)
add_library(mcvARTools src/PointCloud.cpp src/DrawingContext.cpp
                       src/CameraCalibration src/GeometryTypes.cpp ${HEADER_FILES})
add_executable( arPet samples/io_example.cpp ${HEADER_FILES})
target_link_libraries( arPet ${OpenCV_LIBS} mcvARTools)
