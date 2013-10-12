/*****************************************************************************
*   Based partially on:
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*****************************************************************************/

#ifndef DrawingContext_HPP
#define DrawingContext_HPP

#include "GeometryTypes.hpp"
#include "CameraCalibration.hpp"

#include <opencv2/opencv.hpp>

namespace mcv {
void DrawingContextDrawCallback(void* param);

class DrawingContext
{
public:
    DrawingContext(std::string windowName, cv::Size frameSize, const CameraCalibration& c);
    ~DrawingContext();
  
    bool isPatternPresent;
    Transformation patternPose;

    //! Set the new frame for the background
    void updateBackground(const cv::Mat& frame);
    void updateWindow();

private:
    friend void DrawingContextDrawCallback(void* param);
    //! Render entire scene in the OpenGl window
    void draw();

    //! Draws the background with video
    void drawCameraFrame();

    //! Draws the AR
    void drawAugmentedScene();

    //! Builds the right projection matrix from the camera calibration for AR
    void buildProjectionMatrix(const CameraCalibration& calibration, int w, int h, Matx44f& result);

    //! Draws the coordinate axis
    void drawCoordinateAxis();

    //! Draw the cube model
    void drawCubeModel();

    //! Draw a Color Bar
    void drawColorBar(int size_x, int size_y,int x_init,int y_init);

private:
    bool m_isTextureInitialized;
    unsigned int m_backgroundTextureId;
    CameraCalibration m_calibration;
    cv::Mat m_backgroundImage;
    std::string m_windowName;
};
}// mcv
#endif
