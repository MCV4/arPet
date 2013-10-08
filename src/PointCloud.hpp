/*M///////////////////////////////////////////////////////////////////////////////////////
Copyright (c) 2013, Master in Computer Vision Project, France
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  Neither the name of the {organization} nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//M*/

#ifndef __POINTCLOUD_HPP__
#define __POINTCLOUD_HPP__

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string>

/*! PointCloud class */
namespace mcv {
    
class Point3Cloud
{
public:
    /*! Constructors */
    Point3Cloud();
    Point3Cloud( const cv::Mat& data );
    Point3Cloud( const cv::Mat& data, const cv::Mat& bgr );
    /*! Destructors */
    ~Point3Cloud();
    
    /*! Load/Read/Write */
    void grabFrame( cv::VideoCapture capturer, bool grabColor = true );
    void readFrame( const std::string &name );
    void writeFrame( const std::string &name );
    
    /*! Public Methods */
    void applyTransformation( const cv::Matx33f& rotation,
                              const cv::Vec3f translation );   
    void applyRotation( const cv::Matx33f& rotX, const cv::Matx33f& rotY,
                        const cv::Matx33f& rotZ );
    void applyTranslation( const cv::Matx33f& translation );
    void displayColor2D( const std::string windowName );
    
protected:
    /*! Atributes */
    cv::Mat data;
    cv::Mat bgr;
};

} // mcv

#endif
