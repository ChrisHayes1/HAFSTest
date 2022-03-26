#include <google/protobuf/empty.pb.h>
#include <grpc++/grpc++.h>
#include <iostream>
#include <thread>
#include "hafs.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using hafs::HAFS;
using hafs::BeatReq;
using hafs::BeatResp;



class HAFSClient {
 public:
    HAFSClient(std::shared_ptr<Channel> channel)
      : stub_(HAFS::NewStub(channel)) {}
    
    int heartBeat(){
        // Follows the same pattern as SayHello.
        BeatReq request;    
        request.set_req("S_R_V_R__H_R_T");

        BeatResp reply;
        ClientContext context;

        // Here we can use the stub's newly available method we just added.
        Status status = stub_->heartBeat(&context, request, &reply);
        if (status.ok()) {
            return reply.resp();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                    << std::endl;
            return -1;
        }
    }
 private:
  std::unique_ptr<HAFS::Stub> stub_;
};

class HAFSServiceImpl final : public HAFS::Service {
  // Server implementation for heartBeat
  grpc::Status heartBeat (grpc::ServerContext *context,
                          const BeatReq *request,
                          BeatResp *reply) {
    std::cout << "Server got msg: " << request->req() <<std::endl;                             
    reply->set_resp(22);
    return Status::OK;
  }
};


void run_server(std::string my_address){
    std::cout << "Initializing server on port " << my_address << std::endl;   

    HAFSServiceImpl primary;

    grpc::ServerBuilder builder;

    builder.AddListeningPort(my_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&primary);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    
    server->Wait();
}

void run_client(std::string my_address){
    std::cout << "client is writing to " << my_address;    
    HAFSClient client(grpc::CreateChannel(my_address, grpc::InsecureChannelCredentials()));

    while (true){
        int response = client.heartBeat();
        std::cout << "Ack received value: " << response << std::endl;
        sleep(3);
    }
    
}

int main(int argc, char* argv[]){
  
  if (std::string(argv[1]) == "-b"){
    std::cout << "Running as backup" << std::endl;
    std::thread t1(run_client, argv[3]);
    std::thread t2(run_server, argv[2]);    
    
    t1.join();
    std::cout << "MAIN: client ends" << std::endl;
    t2.join();
  } else {
      run_server(argv[2]);
  }
  
  

  return 0;
}