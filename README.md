# build image
docker build -t ubuntu-grpc .

# create container
docker run -it --rm --name grpc-mytest -v ~/grpc-timeout/grpc-keepalive-watchdog-timeout:/data  ubuntu-grpc  /bin/bash

# compile
make

# Run the server on the current terminal.
export CPLUS_INCLUDE_PATH=/usr/local/include

export LD_LIBRARY_PATH=/usr/local/lib

export GRPC_VERBOSITY=debug

export GRPC_TRACE=http_keepalive

./server

# Open another terminal to run the client.
docker exec -it grpc-mytest /bin/bash

export CPLUS_INCLUDE_PATH=/usr/local/include

export LD_LIBRARY_PATH=/usr/local/lib

export GRPC_VERBOSITY=debug

export GRPC_TRACE=http_keepalive

./client
