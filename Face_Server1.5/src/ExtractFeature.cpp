#include "ExtractFeature.h"

using namespace std;
using namespace cv;

namespace caffe
{
	extern INSTANTIATE_CLASS(InputLayer);
	extern INSTANTIATE_CLASS(InnerProductLayer);
	extern INSTANTIATE_CLASS(DropoutLayer);
	extern INSTANTIATE_CLASS(ConvolutionLayer);
	//REGISTER_LAYER_CLASS(Convolution);
	extern INSTANTIATE_CLASS(ReLULayer);
	//REGISTER_LAYER_CLASS(ReLU);
	extern INSTANTIATE_CLASS(PoolingLayer);
	//REGISTER_LAYER_CLASS(Pooling);
	extern INSTANTIATE_CLASS(LRNLayer);
	//REGISTER_LAYER_CLASS(LRN);
	extern INSTANTIATE_CLASS(SoftmaxLayer);
	//REGISTER_LAYER_CLASS(Softmax);
	extern INSTANTIATE_CLASS(MemoryDataLayer);
}

ExtractFeature::ExtractFeature()
{

}

ExtractFeature::ExtractFeature(string s1,string s2)
{
    param_file=s1;
    pretrained_param_file=s2;
    phase=caffe::TEST;
}

caffe::Net<float>* ExtractFeature::Net_Init_Load()
{
    caffe::Net<float>* net(new caffe::Net<float>(param_file, phase));
	net->CopyTrainedLayersFrom(pretrained_param_file);
	return net;
}

void ExtractFeature::Caffe_Predefine()//when our code begining run must add it
{
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
	net = Net_Init_Load();
	memory_layer = (caffe::MemoryDataLayer<float> *)net->layers()[0].get();
}

std::vector<float> ExtractFeature::Extract(Mat FaceROI)
{
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
	std::vector<Mat> test;
	std::vector<int> testLabel;
	std::vector<float> test_vector;
	test.push_back(FaceROI);
	testLabel.push_back(0);
	memory_layer->AddMatVector(test, testLabel);// memory_layer and net , must be define be a global variable.
	test.clear(); testLabel.clear();
	std::vector<caffe::Blob<float>*> input_vec;
	net->Forward(input_vec);
	boost::shared_ptr<caffe::Blob<float> > fc7 = net->blob_by_name("fc7");
	int test_num = 0;
	while (test_num < FEATURE_NUM)
	{
		test_vector.push_back(fc7->data_at(0, test_num++, 1, 1));
	}
	return test_vector;
}
