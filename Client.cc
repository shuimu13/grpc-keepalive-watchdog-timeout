#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/grpcpp.h>
#include "streamer.grpc.pb.h"
#include "streamer.pb.h"

using namespace grpc;
using namespace streamer;

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : m_Stub( StreamService::NewStub(channel) ) {}

  void StreamMessages()
  {
    // Data we are sending to the server.
    StreamRequest request;
    std::string message = "hello";
    request.set_request( message );

    // Container for the data we expect from the server.
    StreamResponse response;

    // The actual RPC.
    std::unique_ptr<ClientReader<StreamResponse>> reader = m_Stub->StreamMessages( &m_Context, request );
    while( reader->Read( &response ) )
    {
      std::cout << response.response() << std::endl;
    }

   // Read data is finished.( e.g., The connection has been disconnected. )
   Status status = reader->Finish();
   if( status.ok() )
   {
      std::cout<< "Client StreamMessages grpc finished." <<std::endl;
   }
   else
   {
      std::cout<< "Client StreamMessages grpc: " << status.error_message() << std::endl;
   }

  }

  void Cancel()
  {
    m_Context.TryCancel();
  }

 private:
  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext m_Context;
  std::unique_ptr<StreamService::Stub> m_Stub;
};



int main()
{
  std::string address = "localhost:50055";
  GreeterClient greeter( CreateChannel( address, grpc::InsecureChannelCredentials() ) );

  greeter.StreamMessages();

  return 0;
}
