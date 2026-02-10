#include "Camera.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <sys/wait.h>

namespace {
bool captureFrameWithLibcamera(cv::Mat& frame)
{
    const char* path = "/tmp/grass_frame.jpg";
    const char* logPath = "/tmp/libcamera_still.log";
    std::string cmd = std::string("/usr/bin/libcamera-still -n -t 1000 -o ") + path +
                      " >" + logPath + " 2>&1";
    int result = std::system(cmd.c_str());
    if (result != 0) {
        int exitCode = result;
        if (WIFEXITED(result)) {
            exitCode = WEXITSTATUS(result);
        }
        std::cerr << "libcamera-still nie powiodlo sie" std::endl;
        return false;
    }

    frame = cv::imread(path);
    if (frame.empty()) {
        std::cerr << "Nie udalo sie wczytac obrazu" << std::endl;
        return false;
    }
    return true;
}
} // namespace

Camera::Camera(int deviceIndex, float pixelsPerCm)
    : deviceIndex(deviceIndex), pixelsPerCm(pixelsPerCm)
{
}

bool Camera::init()
{
    (void)deviceIndex;
    return true;
}

float Camera::estimateGrassHeightCm()
{
    cv::Mat frame;
    if (!captureFrameWithLibcamera(frame)) {
        return -1.0f;
    }

    int startRow = static_cast<int>(frame.rows * 0.4);
    cv::Rect roi(0, startRow, frame.cols, frame.rows - startRow);
    cv::Mat cropped = frame(roi);
    cv::Mat hsv;
    cv::cvtColor(cropped, hsv, cv::COLOR_BGR2HSV);
    cv::Mat mask;
    cv::Scalar lower(35, 50, 50);
    cv::Scalar upper(90, 255, 255);
    cv::inRange(hsv, lower, upper, mask);

    cv::erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 1);
    cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);

    float heightCm = computeHeightFromMask(mask);
    if (heightCm < 0.0f) {
        std::cerr << "Nie wykryto trawy" << std::endl;
        return -1.0f;
    }

    return heightCm;
}

float Camera::computeHeightFromMask(const cv::Mat& mask) const
{
    std::vector<cv::Point> points;
    cv::findNonZero(mask, points);
    if (points.empty()) {
        return -1.0f;
    }

    int highestY = mask.rows;
    for (const auto& p : points) {
        if (p.y < highestY) {
            highestY = p.y;
        }
    }

    int pixelsHeight = mask.rows - highestY;
    return static_cast<float>(pixelsHeight) / pixelsPerCm;
}
