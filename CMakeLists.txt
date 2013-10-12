cmake_minimum_required(VERSION 2.8)
project( arPet )

find_package( OpenCV REQUIRED )
find_package( OpenGL REQUIRED )

include_directories(include)
set(HEADER_FILES include/PointCloud.hpp include/CameraCalibration.hpp
                 include/GeometryTypes.hpp include/DrawingContext.hpp
                 include/PointCloudViewer.hpp)
add_library(mcvARTools src/PointCloud.cpp src/DrawingContext.cpp
                       src/CameraCalibration src/GeometryTypes.cpp
                       src/PointCloudViewer.cpp ${HEADER_FILES})
add_executable( write_example samples/write_example.cpp ${HEADER_FILES})
add_executable( read_example samples/read_example.cpp ${HEADER_FILES})
add_executable( ar_sample samples/ar_sample.cpp ${HEADER_FILES})
add_executable( pointcloud_sample samples/pointcloud_sample.cpp ${HEADER_FILES})
target_link_libraries( write_example ${OPENGL_LIBRARIES} ${OpenCV_LIBS} mcvARTools)
target_link_libraries( read_example ${OPENGL_LIBRARIES} ${OpenCV_LIBS} mcvARTools)
target_link_libraries( ar_sample ${OPENGL_LIBRARIES} ${OpenCV_LIBS} mcvARTools)
target_link_libraries( pointcloud_sample ${OPENGL_LIBRARIES} ${OpenCV_LIBS} mcvARTools)
