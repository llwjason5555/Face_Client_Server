#ifndef WCDCFACERECOGNIZE_H_INCLUDED
#define WCDCFACERECOGNIZE_H_INCLUDED

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/opencv.h>

#include "ExtractFeature.h"
#include "MyDB.h"
#include "face_detection.h"

typedef unsigned char BYTE;

class faceData
{
public:
    faceData(){}
    faceData(int _id,float _rate):id(_id),rate(_rate){}
    int id;
    float rate;
    bool operator<(const faceData& fD)
    {
        if(rate<fD.rate)
            return true;
        else
            return false;
    }
};

class wcdCFacerecognize
{
public:
    wcdCFacerecognize();

    wcdCFacerecognize(std::string path,std::string prototxt,std::string caffemodel,std::string seeta_detect_path,std::string dlib_path);

    int m_AddVerifyTarget(std::string VTGUID, cv::Mat iptFrame);

	int m_DelVerifyTarget(std::string VTGUID);

	int m_ClearVerifyTarget();

    std::vector<faceData> m_FaceListVerify(cv::Mat iptFrame, std::string& VTGUID);

    void Caffe_Init();

    ~wcdCFacerecognize();

private:
    std::string head_path;
	std::vector<std::string> m_name;
	std::vector<std::vector<float> > m_features;

    std::unique_ptr<ExtractFeature> ex;
	std::unique_ptr<MyDB> m_sql;

	std::string model_path;
	std::string face_align;

	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
	dlib::shape_predictor sp;//Already get

	std::vector<cv::Mat> FaceDetect(cv::Mat iptFrame);

	std::vector<cv::Rect> FaceDetect(cv::Mat iptFrame,int flag);

    cv::Mat Vector2dToMat(std::vector<float> feature);

	void SaveMat(cv::Mat &saveFeature, const std::string& filename);

	void SaveName(const std::string& name, const std::string& filename);

	float cosine(const std::vector<float>& v1, const std::vector<float>& v2);

	float dotProduct(const std::vector<float>& v1, const std::vector<float>& v2);

	float module(const std::vector<float>& v);

	std::vector<std::string> LoadName();

	std::vector<float> Mat2vector(cv::Mat &FaceMatrix_Mat);

	std::vector<std::vector<float> > LoadFaceMatrix();

	std::string vector2string(std::vector<float> v);

	std::vector<float> string2vector(const std::string& s);

	int gauss(float x[], float y[], int length, float sigma);

	cv::Mat gaussianfilter(cv::Mat img, double sigma0, double sigma1, double shift1, double shift2);

	cv::Mat FaceProcessing(const cv::Mat &img_, double gamma = 0.2, double sigma0 = 1, double sigma1 = -2, double mask = 0, double do_norm = 10);

	double MatMaxMin(cv::Mat im, std::string flag = "MAX");
};


#endif // WCDCFACERECOGNIZE_H_INCLUDED
