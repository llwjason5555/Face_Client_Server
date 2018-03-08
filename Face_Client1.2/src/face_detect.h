#ifndef FACE_DETECT_H_INCLUDED
#define FACE_DETECT_H_INCLUDED

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "codec.h"

using namespace cv;
using namespace std;

static int send_id=10000000;

string process(string path)
{
    Mat mat;
    mat=imread(path);
    if(mat.empty())
    {
        cout<<"image path error(func process)"<<endl;
        return to_string(0);
    }
    resize(mat,mat,Size(358,441));
    std::vector<uchar> data_encode;
    imencode(".bmp", mat, data_encode);
    std::string str_encode(data_encode.begin(), data_encode.end());
    //cout<<str_encode.size()<<endl;
    //std::string res=to_string(str_encode.size())+":"+str_encode;
    cout<<path<<endl;
    string data_str;
    data::Data d;
    send_id++;
    encode_data(to_string(send_id),str_encode,data_str);
    cout<<data_str.size()<<endl;
    return data_str;
}

#endif // FACE_DETECT_H_INCLUDED
