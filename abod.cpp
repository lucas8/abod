
#include "abod.hpp"
#include <iostream>
using namespace cv;

Abod::Abod()
{
}

Abod::~Abod()
{
}

void Abod::addGround(const cv::Mat& pict)
{
    /* Preparing picture */
    cv::Mat used;
    GaussianBlur(pict, used, Size(5,5), 1.8);
    cvtColor(used, used, CV_BGR2HSV);

    /* Create mask */
    Mat mask(480, 640, CV_8U);
    mask = Scalar(0);
    Point poly[] = { Point(20,480), Point(60,200), Point(580,200), Point(620,480) };
    const Point* polys[] = { poly };
    int npts[] = { 4 };
    fillPoly(mask, polys, npts, 1, Scalar(255, 255, 255));

    /* Compute hue histogram */
    int histSize[] = { 30 };
    float hranges[] = { 0, 180 };
    const float* ranges[] = { hranges };
    MatND hhist;
    int channels[] = { 0 };
    calcHist(&used, 1, channels, Mat(), hhist, 1, histSize, ranges, true, false);

    /* Compute intensity histogram */
    histSize[0] = 30;
    hranges[2] = 255;
    MatND vhist;
    channels[0] = 2;
    calcHist(&used, 1, channels, mask, vhist, 1, histSize, ranges, true, false);

    /* Debug plot */
    Point hpt(0,0);
    Point vpt(0,0);
    const float scale = .05f;
    for(int i = 0; i < 30; ++i) {
        Point npt(i*21, hhist.at<float>(i) * scale);
        line(used, hpt, npt, Scalar(255,0,0), 3);
        hpt = npt;
        npt.y = vhist.at<float>(i) * scale;
        line(used, vpt, npt, Scalar(0,255,0), 3);
        vpt = npt;
    }
    imshow("Hists HSV", used);

    /* Store them. */
    if(m_vhist.empty())
        m_vhist = vhist;
    else {
        for(int i = 0; i < 30; ++i)
            m_vhist.at<unsigned int>(i, m_vhist.at<unsigned int>(i) | vhist.at<unsigned int>(i));
    }

    if(m_hhist.empty())
        m_hhist = hhist;
    else {
        for(int i = 0; i < 30; ++i)
            m_hhist.at<unsigned int>(i, m_hhist.at<unsigned int>(i) | hhist.at<unsigned int>(i));
    }
}

