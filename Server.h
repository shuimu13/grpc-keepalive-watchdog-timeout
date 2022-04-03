#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "streamer.grpc.pb.h"
#include "streamer.pb.h"

using namespace grpc;
using namespace streamer;

class GreeterServer
{
public:
  GreeterServer();
  ~GreeterServer();

  void HandleGrpc();
  
private:
   std::unique_ptr<ServerCompletionQueue> m_pCq;
   std::unique_ptr<StreamService::AsyncService> m_pService;
   std::unique_ptr<Server> m_pServer;

   // Class encompasing the state and logic needed to serve a request.
   class CallData
   {
   public:
      // Start listening for a new request.
      CallData( std::unique_ptr<StreamService::AsyncService>& pService,
                std::unique_ptr<ServerCompletionQueue>& pCq );
      ~CallData();

      void Proceed( const std::string& message = std::string() );

   private:
      enum CallStatus
      {
         CREATE,
         START_PROCESSING,
         CONTINUE_PROCESSING,
         DESTROY
      };

      // The means of communication with the gRPC runtime for an asynchronous server.
      std::unique_ptr<StreamService::AsyncService>& m_pService;
      // The completion queue where we are notified that a we have a new event.
      std::unique_ptr<ServerCompletionQueue>& m_pCq;
      // Context for the rpc, allowing to tweak aspects of it such as the use of compression,
      // authentication, as well as to send metadata back to the client.
      grpc::ServerContext m_Ctx;
      StreamRequest m_Request;
      ServerAsyncWriter<StreamResponse> m_Responder;
      CallStatus m_Status;
   };
};