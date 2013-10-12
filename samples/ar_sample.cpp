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

// std //
#include <string>
#include <iostream>
#include <cmath>
// mcv //
#include "PointCloud.hpp"
#include "DrawingContext.hpp"
// cv/gl //
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <GL/gl.h>
#include <GL/glu.h>


using namespace cv;
using namespace std;


int main( int argc, char * argv[] ){

    mcv::Point3Cloud mypc;
    cv::Mat bgrImage;
    mcv::CameraCalibration calibration(1000.0f, 1500.0f, 333.33f, 200.0f);
    mcv::DrawingContext drawer("MCV AR", cv::Size(640,480), calibration);
    cv::Matx33f myR( 1, 0.0, 0.0, -0.0, 1.0, -0.0, -0.0, 0.0, 1.0 );
    cv::Vec3f myT(0.0,-0.0,-10);
    drawer.isPatternPresent = true;
    drawer.patternPose = mcv::Transformation( myR, myT );

    bool loop=true;
    float linSpeed=0.1;
    float angSpeed=0.1;
    float angY=0.0;
    float angZ=0.0;

    if (argc>1){
        mypc.readFrame(argv[1]);
        mypc.getBgr(bgrImage);

        while (loop){
            if (!bgrImage.empty()){
                cv::Mat img = bgrImage.clone();
                drawer.updateBackground(img);

                int keyCode = cv::waitKey(30);
                //if (keyCode>0) std::cout<<keyCode<<std::endl;
                switch (keyCode){
                    case 27:
                        loop=false;
                        break;

                    case 'w':
                    case 'W':
                        myT -= cv::Vec3f(0.0,0.0,linSpeed);
                        break;

                    case 's':
                    case 'S':
                        myT += cv::Vec3f(0.0,0.0,linSpeed);
                        break;

                    case 'a':
                    case 'A':
                        angY+=angSpeed;
                        myR = Matx33f(cos(angY),0,sin(angY),
                                      0,1,0,
                                      -sin(angY),0,cos(angY))*
                              Matx33f(1,0,0,
                                      0,cos(angZ),-sin(angZ),
                                      0,sin(angZ),cos(angZ));
                        break;

                    case 'd':
                    case 'D':
                        angY-=angSpeed;
                        myR = Matx33f(cos(angY),0,sin(angY),
                                      0,1,0,
                                      -sin(angY),0,cos(angY))*
                              Matx33f(1,0,0,
                                      0,cos(angZ),-sin(angZ),
                                      0,sin(angZ),cos(angZ));
                        break;

                    case 'q':
                    case 'Q':
                        angZ+=angSpeed;
                        myR = Matx33f(cos(angY),0,sin(angY),
                                      0,1,0,
                                      -sin(angY),0,cos(angY))*
                              Matx33f(1,0,0,
                                      0,cos(angZ),-sin(angZ),
                                      0,sin(angZ),cos(angZ));
                        break;

                    case 'e':
                    case 'E':
                        angZ-=angSpeed;
                        myR = Matx33f(cos(angY),0,sin(angY),
                                      0,1,0,
                                      -sin(angY),0,cos(angY))*
                              Matx33f(1,0,0,
                                      0,cos(angZ),-sin(angZ),
                                      0,sin(angZ),cos(angZ));
                        break;

                    case 65361: // LEFT ARROW
                        myT += cv::Vec3f(linSpeed,0.0,0.0);
                        break;
                    case 65363: // RIGHT ARROW
                        myT -= cv::Vec3f(linSpeed,0.0,0.0);
                        break;
                    case 65362: // UP ARROW
                        myT += cv::Vec3f(0.0,linSpeed,0.0);
                        break;
                    case 65364: // DOWN ARROW
                        myT -= cv::Vec3f(0.0,linSpeed,0.0);
                        break;
                }

                drawer.patternPose = mcv::Transformation( myR, myT );
                drawer.updateWindow();
            }else{
                std::cout<<"No Kinect Data Received"<<std::endl;
                break;
            }
        }
    }
    return 0;
}
