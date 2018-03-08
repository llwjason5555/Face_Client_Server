#ifndef EXTRACTFEATURE_H_INCLUDED
#define EXTRACTFEATURE_H_INCLUDED

#include "caffe/layers/input_layer.hpp"
#include "caffe/layers/inner_product_layer.hpp"
#include "caffe/layers/dropout_layer.hpp"
#include "caffe/layers/conv_layer.hpp"
#include "caffe/layers/relu_layer.hpp"
#include <iostream>
#include "caffe/caffe.hpp"
#include <opencv2/opencv.hpp>
#include <caffe/layers/memory_data_layer.hpp>
#include "caffe/layers/pooling_layer.hpp"
#include "caffe/layers/lrn_layer.hpp"
#include "caffe/layers/softmax_layer.hpp"

//using namespace std;
//using namespace cv;

const int FEATURE_NUM=4096;

class ExtractFeature
{
public:
    ExtractFeature();
    ExtractFeature(std::string s1,std::string s2);
    std::vector<float> Extract(cv::Mat FaceROI);
    void Caffe_Predefine();
    //template <typename Dtype>
    caffe::Net<float>* Net_Init_Load();

private:
    caffe::MemoryDataLayer<float> *memory_layer;
    caffe::Net<float>* net;
    std::string param_file;
    std::string pretrained_param_file;
    caffe::Phase phase;

};



#endif // EXTRACTFEATURE_H_INCLUDED
