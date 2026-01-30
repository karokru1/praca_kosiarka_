#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <opencv2/opencv.hpp>

class Camera
{
public:
    Camera(int deviceIndex = 0, float pixelsPerCm = 20.0f);

    bool init();
    float estimateGrassHeightCm();
    void setPixelsPerCm(float value) { pixelsPerCm = value; }

private:
    int deviceIndex;
    float pixelsPerCm;
    cv::VideoCapture cap;

    float computeHeightFromMask(const cv::Mat& mask) const;
};

#endif // CAMERA_HPP
