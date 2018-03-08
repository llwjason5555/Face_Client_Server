#include "facerecognize.h"
#include <fstream>
#include <memory>

using namespace std;

unique_ptr<wcdCFacerecognize> w;

void Face_Server_Init()
{
    ifstream in("/home/llw/Project/Face_cmake/Face_Attri/model/Face_Server.conf");
    vector<string> all_path;
    string line;
    while(getline(in,line))
        all_path.push_back(line);
    assert(5==all_path.size());

    w.reset(new wcdCFacerecognize(all_path[0],all_path[1],all_path[2],all_path[3],all_path[4]));
    //w=w1;

    w->Caffe_Init();
    std::cout<<"Caffe Initialize Success!"<<std::endl;

}
std::vector<faceData> solveFacerec(const muduo::StringPiece& s)
{
    muduo::string str_tmp=s.as_string();
    std::cout<<str_tmp.size()<<std::endl;
    cv::Mat img_decode;
    data::Data d=decode_data(std::string(str_tmp.begin(),str_tmp.end()));
    std::vector<uchar> data(d.code().begin(),d.code().end());
    img_decode = imdecode(data,1);
    if(img_decode.empty())
    {
        std::cout<<"accept error"<<std::endl;
        assert(false);
    }

    std::string VTGUID;
    std::vector<faceData> res=w->m_FaceListVerify(img_decode,VTGUID);
    if(res.size()>0)
    {
        for(int i=0;i<res.size();i++)
           std::cout<<"id:"<<res[i].id<<" "<<"rate:"<<res[i].rate<<std::endl;
    }

    cout<<"4"<<endl;
    return res;
}

std::string process_res(std::vector<faceData> result)
{
    std::string res;
    for(int i=0;i<result.size();i++)
    {

        int sz=std::to_string(result[i].id).size();

        if(sz>8)
        {
            std::cout<<"too long(func process_res)"<<std::endl;
            assert(false);
        }
        std::string rate_str=std::to_string(result[i].rate);
        if(rate_str.size()>8)
        {
             rate_str=rate_str.substr(0,8);
        }

        std::string zero_str(8-sz,0);
        res+=zero_str+std::to_string(result[i].id)+rate_str;
    }
    assert(res.size()==48);
    cout<<"5"<<endl;
    return res;
}

void Face_Server_Close()
{
   // delete w;
}
