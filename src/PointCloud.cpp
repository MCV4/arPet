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

#include "PointCloud.hpp"

/*! PointCloud class */
namespace mcv {

/*! Constructors */    
Point3Cloud::Point3Cloud(){
}

Point3Cloud::Point3Cloud( const cv::Mat& data_ ){
    data = data_.clone();
}

Point3Cloud::Point3Cloud( const cv::Mat& data_, const cv::Mat& bgr_ ){
    data = data_.clone();
    bgr = bgr_.clone();
}

/*! Destructors */   
Point3Cloud::~Point3Cloud(){
}

/*! Load/Read/Write */
void Point3Cloud::grabFrame( cv::VideoCapture capturer, bool grabColor ){
    capturer.grab();
    if ( grabColor )
        capturer.retrieve( bgr, CV_CAP_OPENNI_BGR_IMAGE );
        
    capturer.retrieve( data, CV_CAP_OPENNI_POINT_CLOUD_MAP );
}

void Point3Cloud::readFrame( const std::string &name ){
    cv::FileStorage fs( name, cv::FileStorage::READ );
    fs["3data"]>>data;
    fs["Cdata"]>>bgr;
}

void Point3Cloud::writeFrame( const std::string &name ){
    cv::FileStorage fs( name, cv::FileStorage::WRITE );
    fs<<"3data"<<data;
    fs<<"Cdata"<<bgr;
}

/*! Public Methods */
void Point3Cloud::applyTransformation( const cv::Matx33f& rotation,
                                       const cv::Vec3f translation ){

}

void Point3Cloud::applyRotation( const cv::Matx33f& rotX, const cv::Matx33f& rotY,
                    const cv::Matx33f& rotZ ){

}

void Point3Cloud::applyTranslation( const cv::Matx33f& translation ){

}

void Point3Cloud::displayColor2D( const std::string name ){
    if ( !bgr.empty() )
        cv::imshow( name, bgr );
}

} // mcv
