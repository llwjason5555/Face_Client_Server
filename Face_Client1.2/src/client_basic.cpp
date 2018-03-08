#include "face_detect.h"

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpClient.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <fstream>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

int i=0;

class FaceClient : boost::noncopyable
{
 public:
  FaceClient(EventLoop* loop,
                const InetAddress& serverAddr,
               const muduo::string& name
               )
    : name_(name),
      client_(loop, serverAddr, name_)
  {
    client_.setConnectionCallback(
        boost::bind(&FaceClient::onConnection, this, _1));
    client_.setMessageCallback(
        boost::bind(&FaceClient::onMessage, this, _1, _2, _3));
  }

  void connect()
  {
    client_.connect();
  }

 private:
  void onConnection(const TcpConnectionPtr& conn)
  {
    if (conn->connected())
    {
      LOG_INFO << name_ << " connected";
      start_ = Timestamp::now();


    std::string temp;
    //i++;
    //if(i<100)
    //{
        std::string temp_path="/home/llw/Project/Face_cmake/Face_Client1.2/src/01.bmp";
        temp=process(temp_path);
        if(temp!="0")
            conn->send(temp);
    //}
      LOG_INFO << name_ << " sent requests";
    }
    else
    {
      LOG_INFO << name_ << " disconnected";
    }
  }

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
  {

    size_t len = buf->readableBytes();
    cout<<"get face:"<<len<<endl;
    if(len==0)
        cout<<"No face!"<<endl;
    while (len)
    {
        std::string temp(buf->peek(), len);
        int num_len=len/8;
        for(int i=0;i<num_len;i++)
            cout<<temp.substr(i*8,8)<<endl;
        //cout<<temp<<endl;
        buf->retrieveAll();
        len=buf->readableBytes();

    }

    //std::string temp;
    //i++;
    //if(i<100)
    //{
    //    std::string temp_path="/home/llw/Project/wcdCFacerecognize/registerimg/"+to_string(i)+".bmp";
    //    temp=process(temp_path);
    //    if(temp!="0")
    //        conn->send(temp);
    //}
  }

  muduo::string name_;
  TcpClient client_;
  Timestamp start_;
};


int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    assert(argc==3);
    EventLoop loop;

    InetAddress seraddr(argv[1],stoi(argv[2]));
    FaceClient client(&loop,seraddr,"FaceClient");

    client.connect();
    loop.loop();
}
