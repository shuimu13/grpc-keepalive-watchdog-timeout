#include <iostream>
#include <memory>
#include <string>

#include "Server.h"

#include <grpcpp/grpcpp.h>
#include "streamer.grpc.pb.h"
#include "streamer.pb.h"

using namespace grpc;
using namespace streamer;


GreeterServer::GreeterServer()
{
  ServerBuilder serverBuilder;
  serverBuilder.AddChannelArgument( GRPC_ARG_KEEPALIVE_TIME_MS, 60000 );
  serverBuilder.AddChannelArgument( GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1 );
  serverBuilder.AddChannelArgument( GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA, 0 );
  serverBuilder.AddChannelArgument( GRPC_ARG_HTTP2_MIN_SENT_PING_INTERVAL_WITHOUT_DATA_MS, 10000 );

  // Listen on the given address without any authentication mechanism.
  const std::string serverAddress = "0.0.0.0:50055";
  serverBuilder.AddListeningPort( serverAddress, InsecureServerCredentials() );
  // Register service as the instance through which we'll communicate with clients.
  m_pService.reset( new StreamService::AsyncService() );
  serverBuilder.RegisterService( m_pService.get() );
  // Get hold of the completion queue used for the asynchronous communication with the gRPC runtime.
  m_pCq = serverBuilder.AddCompletionQueue();
  // Finally assemble the server.
  m_pServer = serverBuilder.BuildAndStart();
  // Spawn a new CallData instance to serve new clients.
  new CallData( m_pService, m_pCq );
}

GreeterServer::~GreeterServer()
{
}

void GreeterServer::HandleGrpc()
{
  void* pTag = nullptr;
  bool ok = false;
  size_t index = 0;
  while( m_pCq->Next( &pTag, &ok ) )
  {
    if( pTag && ok )
    {
      std::string message = "hello " + std::to_string( ++index );
      static_cast<CallData*>( pTag )->Proceed( message );
      sleep( 5 );
    }
    else
    {
      if( pTag )
      {
        CallData* pCallData = static_cast<CallData*>( pTag );
        delete pCallData;

        std::cout << "Clear!" << std::endl;
      }
    }
  }

  std::cout << "Server finish!" << std::endl;
}

GreeterServer::CallData::CallData( std::unique_ptr<StreamService::AsyncService>& pService,
                                  std::unique_ptr<ServerCompletionQueue>& pCq ) :
  m_pService( pService ),
  m_pCq( pCq ),
  m_Responder( &m_Ctx ),
  m_Status( CREATE )
{
  Proceed();
}

GreeterServer::CallData::~CallData()
{
}

void GreeterServer::CallData::Proceed( const std::string& message )
{
  if( m_Status == CREATE )
  {
    // As part of the initial CREATE state, we *request* that the system
    // start processing requests. In this request, "this" acts are
    // the tag uniquely identifying the request (so that different CallData
    // instances can serve different requests concurrently), in this case
    // the memory address of this CallData instance.
    m_pService->RequestStreamMessages( &m_Ctx, &m_Request, &m_Responder, m_pCq.get(), m_pCq.get(), this );
    m_Status = START_PROCESSING;
  }
  else if( ( m_Status == START_PROCESSING ) || ( m_Status == CONTINUE_PROCESSING ) )
  {
    if( m_Status == START_PROCESSING )
    {
      // Spawn a new CallData instance to serve new clients while we process
      // the one for this CallData. The instance will deallocate itself as
      // part of its FINISH state.
      new CallData( m_pService, m_pCq );
      m_Status = CONTINUE_PROCESSING;
    }

    try
    {
      StreamResponse response;
      response.set_response( message );
      m_Responder.Write( response, this );
    }
    catch( std::runtime_error& err )
    {
      std::cout << "Error occurred: " << err.what() << std::endl;
      m_Status = DESTROY;
    }
    catch( ... )
    {
      std::cout << "Internal error occurred!" << std::endl;
      m_Status = DESTROY;
    }
  }
  else
  {
    // Once in the DESTROY state, deallocate ourselves (CallData).
    delete this;
  }
}

int main()
{
  GreeterServer server;
  server.HandleGrpc();

  return 0;
}