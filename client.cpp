
#include <grpc++/grpc++.h>
#include <iostream>
#include <google/protobuf/empty.pb.h>
#include <unistd.h>

#include "hafs.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using hafs::BeatReq;
using hafs::BeatResp;
using hafs::HAFS;

class HAFSClient {
 public:
    HAFSClient(std::shared_ptr<Channel> channel)
      : stub_(HAFS::NewStub(channel)) {}
    
    int heartBeat(){
        // Follows the same pattern as SayHello.
        BeatReq request;    
        request.set_req("H_E_A_R_T");

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


int main(){
    std::string address("0.0.0.0:5000");

    HAFSClient client(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()));

    for (int i = 0; i < 10; i++){
        int response = client.heartBeat();
        std::cout << "Ack received value: " << response << std::endl;
        sleep(1);
    }
    
    return 0;
}