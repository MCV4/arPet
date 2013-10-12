/*****************************************************************************
*   Based partially on:
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*****************************************************************************/

#include "DrawingContext.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

namespace mcv {

void DrawingContextDrawCallback(void* param){
    DrawingContext * ctx = static_cast<DrawingContext*>(param);
    if (ctx)
        ctx->draw();
}

DrawingContext::DrawingContext(std::string windowName, cv::Size frameSize, const CameraCalibration& c)
  : m_isTextureInitialized(false)
  , m_calibration(c)
  , m_windowName(windowName){
    // Create window with OpenGL support
    cv::namedWindow(windowName, CV_WINDOW_OPENGL);

    // Resize it exactly to video size
    cv::resizeWindow(windowName, frameSize.width, frameSize.height);

    // Initialize OpenGL draw callback:
    cv::setOpenGlContext(windowName);
    cv::setOpenGlDrawCallback(windowName, DrawingContextDrawCallback, this);
}

DrawingContext::~DrawingContext(){
    cv::setOpenGlDrawCallback(m_windowName, 0, 0);
}

void DrawingContext::updateBackground(const cv::Mat& frame){
    frame.copyTo(m_backgroundImage);
}

void DrawingContext::updateWindow(){
    cv::updateWindow(m_windowName);
}

void DrawingContext::draw(){
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // Clear entire screen:
    drawCameraFrame();                                  // Render background
    drawAugmentedScene();                               // Draw AR
    glFlush();
}


void DrawingContext::drawCameraFrame(){
    // Initialize texture for background image
    if (!m_isTextureInitialized){
        glGenTextures(1, &m_backgroundTextureId);
        glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        m_isTextureInitialized = true;
    }

    int w = m_backgroundImage.cols;
    int h = m_backgroundImage.rows;

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId);

    // Upload new texture data:
    if (m_backgroundImage.channels() == 3)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, m_backgroundImage.data);
    else if(m_backgroundImage.channels() == 4)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_backgroundImage.data);
    else if (m_backgroundImage.channels()==1)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_backgroundImage.data);

    const GLfloat bgTextureVertices[] = { 0, 0, w, 0, 0, h, w, h };
    const GLfloat bgTextureCoords[]   = { 1, 0, 1, 1, 0, 0, 0, 1 };
    const GLfloat proj[]              = { 0, -2.f/w, 0, 0, -2.f/h, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 };

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(proj);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_backgroundTextureId);

    // Update attribute values.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, bgTextureVertices);
    glTexCoordPointer(2, GL_FLOAT, 0, bgTextureCoords);

    glColor4f(1,1,1,1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

void DrawingContext::drawAugmentedScene(){
    // Init augmentation projection
    Matx44f projectionMatrix;
    int w = m_backgroundImage.cols;
    int h = m_backgroundImage.rows;
    buildProjectionMatrix(m_calibration, w, h, projectionMatrix);
    projectionMatrix = projectionMatrix.t(); // Transpose the matrix because OpenCV is row-major and OpenGL is column-major
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix.val);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawColorBar(15,100,10,20);

    if (isPatternPresent)
    {
    // Set the pattern transformation
    Matx44f glMatrix = patternPose.getMat44();
    glLoadMatrixf(reinterpret_cast<const GLfloat*>(&glMatrix.val[0]));

    // Render model
    drawCoordinateAxis();
    drawCubeModel();
    } else {
      drawCoordinateAxis();
    }
}

void DrawingContext::buildProjectionMatrix(const CameraCalibration& calibration, int screen_width, int screen_height, Matx44f& projectionMatrix){
    float nearPlane = 0.01f;  // Near clipping distance
    float farPlane  = 100.0f;  // Far clipping distance

    // Camera parameters
    float f_x = calibration.fx(); // Focal length in x axis
    float f_y = calibration.fy(); // Focal length in y axis (usually the same?)
    float c_x = calibration.cx(); // Camera primary point x
    float c_y = calibration.cy(); // Camera primary point y

    projectionMatrix(0,0) = -2.0f * f_x / screen_width;
    projectionMatrix(1,0) = 0.0f;
    projectionMatrix(2,0) = 0.0f;
    projectionMatrix(3,0) = 0.0f;

    projectionMatrix(0,1) = 0.0f;
    projectionMatrix(1,1) = 2.0f * f_y / screen_height;
    projectionMatrix(2,1) = 0.0f;
    projectionMatrix(3,1) = 0.0f;

    projectionMatrix(0,2) = 2.0f * c_x / screen_width - 1.0f;
    projectionMatrix(1,2) = 2.0f * c_y / screen_height - 1.0f;
    projectionMatrix(2,2) = -( farPlane + nearPlane) / ( farPlane - nearPlane );
    projectionMatrix(3,2) = -1.0f;

    projectionMatrix(0,3) = 0.0f;
    projectionMatrix(1,3) = 0.0f;
    projectionMatrix(2,3) = -2.0f * farPlane * nearPlane / ( farPlane - nearPlane );
    projectionMatrix(3,3) = 0.0f;
}


void DrawingContext::drawCoordinateAxis(){
    static float lineX[] = {0,0,0,1,0,0};
    static float lineY[] = {0,0,0,0,1,0};
    static float lineZ[] = {0,0,0,0,0,1};

    glLineWidth(2);

    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3fv(lineX);
    glVertex3fv(lineX + 3);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3fv(lineY);
    glVertex3fv(lineY + 3);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3fv(lineZ);
    glVertex3fv(lineZ + 3);

    glEnd();
}

void DrawingContext::drawCubeModel(){
    static const GLfloat LightAmbient[]=  { 0.25f, 0.25f, 0.25f, 1.0f };    // Ambient Light Values
    static const GLfloat LightDiffuse[]=  { 0.1f, 0.1f, 0.1f, 1.0f };    // Diffuse Light Values
    static const GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };    // Light Position

    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT);

    glColor4f(0.2f,0.35f,0.3f,0.75f);         // Full Brightness, 50% Alpha ( NEW )
    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);       // Blending Function For Translucency Based On Source Alpha
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
    glEnable(GL_COLOR_MATERIAL);

    glScalef(0.25,0.25, 0.25);
    glTranslatef(0,0, 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);
    // Front Face
    glNormal3f( 0.0f, 0.0f, 1.0f);    // Normal Pointing Towards Viewer
    glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 1 (Front)
    glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 2 (Front)
    glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Front)
    glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 4 (Front)
    // Back Face
    glNormal3f( 0.0f, 0.0f,-1.0f);    // Normal Pointing Away From Viewer
    glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Back)
    glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 2 (Back)
    glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 3 (Back)
    glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 4 (Back)
    // Top Face
    glNormal3f( 0.0f, 1.0f, 0.0f);    // Normal Pointing Up
    glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 1 (Top)
    glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 2 (Top)
    glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Top)
    glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 4 (Top)
    // Bottom Face
    glNormal3f( 0.0f,-1.0f, 0.0f);    // Normal Pointing Down
    glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Bottom)
    glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 2 (Bottom)
    glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 3 (Bottom)
    glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 4 (Bottom)
    // Right face
    glNormal3f( 1.0f, 0.0f, 0.0f);    // Normal Pointing Right
    glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 1 (Right)
    glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 2 (Right)
    glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Right)
    glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 4 (Right)
    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
    glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Left)
    glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 2 (Left)
    glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 3 (Left)
    glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 4 (Left)
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor4f(0.2f,0.65f,0.3f,0.35f); // Full Brightness, 50% Alpha ( NEW )
    glBegin(GL_QUADS);
    // Front Face
    glNormal3f( 0.0f, 0.0f, 1.0f);    // Normal Pointing Towards Viewer
    glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 1 (Front)
    glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 2 (Front)
    glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Front)
    glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 4 (Front)
    // Back Face
    glNormal3f( 0.0f, 0.0f,-1.0f);    // Normal Pointing Away From Viewer
    glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Back)
    glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 2 (Back)
    glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 3 (Back)
    glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 4 (Back)
    // Top Face
    glNormal3f( 0.0f, 1.0f, 0.0f);    // Normal Pointing Up
    glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 1 (Top)
    glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 2 (Top)
    glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Top)
    glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 4 (Top)
    // Bottom Face
    glNormal3f( 0.0f,-1.0f, 0.0f);    // Normal Pointing Down
    glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Bottom)
    glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 2 (Bottom)
    glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 3 (Bottom)
    glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 4 (Bottom)
    // Right face
    glNormal3f( 1.0f, 0.0f, 0.0f);    // Normal Pointing Right
    glVertex3f( 1.0f, -1.0f, -1.0f);  // Point 1 (Right)
    glVertex3f( 1.0f,  1.0f, -1.0f);  // Point 2 (Right)
    glVertex3f( 1.0f,  1.0f,  1.0f);  // Point 3 (Right)
    glVertex3f( 1.0f, -1.0f,  1.0f);  // Point 4 (Right)
    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);    // Normal Pointing Left
    glVertex3f(-1.0f, -1.0f, -1.0f);  // Point 1 (Left)
    glVertex3f(-1.0f, -1.0f,  1.0f);  // Point 2 (Left)
    glVertex3f(-1.0f,  1.0f,  1.0f);  // Point 3 (Left)
    glVertex3f(-1.0f,  1.0f, -1.0f);  // Point 4 (Left)
    glEnd();

    glPopAttrib();
}

void DrawingContext::drawColorBar(int size_x, int size_y,int x_init,int y_init){
    std::vector<cv::Vec3f> colorSteps;
    cv::Vec3f COL_BLUE(0,0,1);
    cv::Vec3f COL_CYAN(0,1,1);
    cv::Vec3f COL_GREEN(0,1,0);
    cv::Vec3f COL_YELLOW(1,1,0);
    cv::Vec3f COL_RED(1,0,0);

    colorSteps.push_back(COL_BLUE);
    colorSteps.push_back(COL_CYAN);
    colorSteps.push_back(COL_GREEN);
    colorSteps.push_back(COL_YELLOW);
    colorSteps.push_back(COL_RED);

    // store draw mode (light and polygon mode)
    GLint polygon_draw_mode[2];
    glGetIntegerv(GL_POLYGON_MODE,polygon_draw_mode);

    std::vector<cv::Vec3f>::iterator it(colorSteps.begin());
    cv::Vec3d current,prev;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    glDisable ( GL_LIGHTING );
    glPolygonMode(GL_FRONT,GL_FILL);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, viewport[2], 0, viewport[3]);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glTranslatef(x_init, y_init,0);
        //double coef_x(size_x);
        double coef_y(size_y/double(colorSteps.size()-1) );

        for(unsigned int i=0;i<colorSteps.size()-1;i++){
            prev=*it;
            it++;
            current=*it;

            glBegin(GL_QUADS);

            glNormal3d(0,0,1.0);
            glColor3d(prev[0], prev[1], prev[2]);
            glVertex3d(size_x,(i)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(current[0],current[1],current[2]);
            glVertex3d(size_x,(i+1)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(current[0],current[1],current[2]);
            glVertex3d(0,(i+1)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(prev[0], prev[1], prev[2]);
            glVertex3d(0,(i)*coef_y,0);

            glEnd();

            double t0=(double)(i)/((double)colorSteps.size()-1);
            double t1=(double)(i+1)/((double)colorSteps.size()-1);

            glBegin(GL_QUADS);

            glNormal3d(0,0,1.0);
            glColor3d(t0,t0,t0);
            glVertex3d(2*size_x+10,i*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(t1,t1,t1);
            glVertex3d(2*size_x+10,(i+1)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(t1,t1,t1);
            glVertex3d(size_x+10,(i+1)*coef_y,0);

            glNormal3d(0,0,1.0);
            glColor3d(t0,t0,t0);
            glVertex3d(size_x+10,i*coef_y,0);

            glEnd();
        }

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    //old polygon drawing mode restoration
    glPolygonMode(GL_FRONT,polygon_draw_mode[0]|polygon_draw_mode[1]);
}
}//mcv
