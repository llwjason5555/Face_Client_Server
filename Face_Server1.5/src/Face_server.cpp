#include "facerecognize.h"

#include <muduo/base/Atomic.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>

#include <utility>

#include <stdio.h>
#include <unistd.h>
#include <algorithm>

using namespace std;
using namespace muduo;
using namespace muduo::net;

const int PACK_SIZE = 474584;

typedef struct recv_data
{
    muduo::string request;
    muduo::string left_str;
} recv_data;

class FaceServer
{
 public:
  FaceServer(EventLoop* loop, const InetAddress& listenAddr, int numThreads)
    : server_(loop, listenAddr, "FaceServer"),
      numThreads_(numThreads),
      startTime_(Timestamp::now())
  {
    server_.setConnectionCallback(
        boost::bind(&FaceServer::onConnection, this, _1));
    server_.setMessageCallback(
        boost::bind(&FaceServer::onMessage, this, _1, _2, _3));
    server_.setThreadNum(numThreads);
  }

  void start()
  {
    LOG_INFO << "starting " << numThreads_ << " threads.";
    server_.start();
  }

 private:
  void onConnection(const TcpConnectionPtr& conn)
  {
    LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
        << conn->localAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");
    recv_data temp_data;
    conn->setContext(temp_data);
  }

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
  {
    LOG_DEBUG << conn->name();
    size_t len = buf->readableBytes();
    cout<<"accept:"<<len<<endl;

    recv_data temp_data=boost::any_cast<recv_data>(conn->getContext());
    while (len)
    {
        muduo::string temp=buf->toStringPiece().as_string();

        if(len>PACK_SIZE-temp_data.request.size())
        {

            temp_data.left_str=temp.substr(PACK_SIZE-temp_data.request.size());
            temp=temp.substr(0,PACK_SIZE-temp_data.request.size());
            temp_data.request+=temp;

            buf->retrieveAll();
            len = buf->readableBytes();
            break;
        }

        temp_data.request+=temp;
        buf->retrieveAll();
        len = buf->readableBytes();

    }
    if(temp_data.request.size()==PACK_SIZE)
    {
        processRequest(conn, temp_data.request);
        temp_data.request=temp_data.left_str;
        temp_data.left_str.clear();

    }
    conn->setContext(temp_data);

  }

  bool processRequest(const TcpConnectionPtr& conn, const muduo::string& request)
  {
    std::string id;
    muduo::string puzzle;
    bool goodRequest = true;

    muduo::string::const_iterator colon = find(request.begin(), request.end(), ':');
    if (colon != request.end())
    {
      id.assign(request.begin(), colon);
      puzzle.assign(colon+1, request.end());
    }
    else
    {
      puzzle = request;
    }

    if (puzzle.size()>0)
    {
      LOG_DEBUG << conn->name();
      std::vector<faceData> result= solveFacerec(request);

      conn->send(process_res(result));

    }
    else
    {
      goodRequest = false;
    }
    return goodRequest;
  }

  TcpServer server_;
  int numThreads_;
  Timestamp startTime_;
};

int main(int argc, char* argv[])
{
  LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  assert(argc == 3);
  int numThreads = stoi(argv[2]);
  cout<<numThreads<<endl;
  EventLoop loop;
  InetAddress listenAddr(stoi(argv[1]));
  FaceServer server(&loop, listenAddr,numThreads);

  Face_Server_Init();

  server.start();
  loop.loop();

  Face_Server_Close();
}

