/*****************************************************************************
*   Based partially on:
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*****************************************************************************/

#include "PointCloudViewer.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

namespace mcv {
void PointCloudViewerDrawCallback(void* param){
    PointCloudViewer * ctx = static_cast<PointCloudViewer*>(param);
    if (ctx)
        ctx->draw();
}

PointCloudViewer::PointCloudViewer(std::string windowName, cv::Size frameSize)
  : m_isTextureInitialized(false)
  , m_windowName(windowName)
  , size(frameSize){
    // Create window with OpenGL support
    cv::namedWindow(windowName, CV_WINDOW_OPENGL);
    // Resize it exactly to video size
    cv::resizeWindow(windowName, frameSize.width, frameSize.height);
    // Initialize OpenGL draw callback:
    cv::setOpenGlContext(windowName);
    cv::setOpenGlDrawCallback(windowName, PointCloudViewerDrawCallback, this);
}

PointCloudViewer::~PointCloudViewer(){
    cv::setOpenGlDrawCallback(m_windowName, 0, 0);
}

void PointCloudViewer::updatePointCloud(const Point3Cloud &cloud){
    m_pointCloud = Point3Cloud(cloud);
}

void PointCloudViewer::updateWindow(){
    cv::updateWindow(m_windowName);
}

void PointCloudViewer::draw(){
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear entire screen:
    drawScene(); // Draw PC
    glFlush();
}

void PointCloudViewer::drawScene(){

    glPolygonMode(GL_FRONT,GL_FILL);  // set the drawing mode to full rendering
    glEnable(GL_DEPTH_TEST);          //activate Z buffer (hide elements in the back)

    glEnable(GL_POLYGON_OFFSET_FILL); //useful if you want to superpose the rendering in full mode and in wireless mode
    glPolygonOffset(1.0,1.0);         //convenient settings for polygon offset, do not change this

    glEnable(GL_NORMALIZE);           // unit normals, in case you would forget to compute them
    glEnable(GL_COLOR_MATERIAL);      // now you can associate a color to a point...

    glClearColor(1.0f,1.0f,1.0f,0.0f);    //background color is white (better for screenshot when writing a paper)
    glEnable( GL_BLEND );                 //you can activate blending for better rendering...

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // careful with those parameters, results depend on your graphic card
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable( GL_POINT_SMOOTH );
    glHint( GL_POINT_SMOOTH, GL_NICEST );


    drawPointCloud();
}

void PointCloudViewer::drawPointCloud(){
    glPushMatrix();
    glViewport(0, 0, size.width, size.height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double znear = m_pointCloud.bBDistance*0.1;
    double zfar = m_pointCloud.bBDistance*5;

    gluPerspective(
                    45, // 45 deg is ok
                    (float)size.width/size.height,
                    znear,
                    zfar // same
                    );

    cv::Vec3f cameraTarget = m_pointCloud.bBCenter;
    cv::Vec3f cameraPosition = cameraTarget + cv::Vec3f(m_pointCloud.bBDistance,0,m_pointCloud.bBDistance);

    gluLookAt(
        cameraPosition[0], cameraPosition[1], cameraPosition[2],
        cameraTarget[0], cameraTarget[1], cameraTarget[2],
        0,1,0); //Up Vector, do not change this

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);

    glPointSize(1.0);
    glBegin(GL_POINTS);

    //bool color_on=false;
    cv::Mat bgr, points;
    m_pointCloud.getBgr(bgr);
    m_pointCloud.getData(points);

    for( int i=0 ; i<points.rows; i++ ){
        for( int j=0 ; j<points.cols; j++ ){
            //if(color_on){
                cv::Vec3b bgrPixel = bgr.at<Vec3b>(i,j);
                glColor3b(bgrPixel[2],bgrPixel[1],bgrPixel[0]);
            //}
            cv::Vec3f theP = points.at<Vec3f>(i,j);
            glVertex3f(theP[0],theP[1],theP[2]);
        }
    }

    glEnd();

    ///////
    glEnable(GL_COLOR_MATERIAL);

    //roughly speaking : color of reflected light
    GLfloat specular[]={1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT,GL_SPECULAR,specular);

    //intensity of the shining...
    GLfloat shine[]={100};
    glMaterialfv(GL_FRONT,GL_SHININESS,shine);

    //ambient and diffuse light
    GLfloat ambient[]={0.0, 0.0, 0.0, 1.0};
    GLfloat diffuse[]={0.4, 0.4, 0.4, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    //set spot light cone, direction, angle, etc
    //GLfloat position[]={CamPosition[0],CamPosition[1],CamPosition[2] + 10 ,1};
    cv::Vec3f Dir = -cv::norm(cameraPosition - cameraTarget);
    GLfloat spot_direction[] = {Dir[0],Dir[1],Dir[2]};
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 360.0);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);

    //activate the first light
    glEnable(GL_LIGHT0);
    //////
}

}//mcv
