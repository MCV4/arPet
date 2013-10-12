/*****************************************************************************
*   Based partially on:
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*****************************************************************************/

#ifndef PointCloudViewer_HPP
#define PointCloudViewer_HPP

#include "PointCloud.hpp"
#include "GeometryTypes.hpp"
#include <opencv2/opencv.hpp>

namespace mcv {
void PointCloudViewerDrawCallback(void* param);

class PointCloudViewer
{
public:
    PointCloudViewer(std::string windowName, cv::Size frameSize);
    ~PointCloudViewer();

    void updatePointCloud(const mcv::Point3Cloud& cloud);
    void updateWindow();

private:
    friend void PointCloudViewerDrawCallback(void* param);
    //! Render entire scene in the OpenGl window
    void draw();

    //! Draw the Points
    void drawScene();
    void drawPointCloud();

private:
    bool m_isTextureInitialized;
    unsigned int m_backgroundTextureId;
    mcv::Point3Cloud m_pointCloud;
    std::string m_windowName;
    cv::Size size;
};
}// mcv
#endif
