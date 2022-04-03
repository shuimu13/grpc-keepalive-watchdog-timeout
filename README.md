1.build image
docker build -t ubuntu-grpc .

2.create container
docker run -it --rm --name grpc-mytest -v ~/grpc-timeout/grpc-keepalive-watchdog-timeout:/data  ubuntu-grpc  /bin/bash

3.compile
make

4.run
# Run the server on the current terminal.
export CPLUS_INCLUDE_PATH=/usr/local/include
export LD_LIBRARY_PATH=/usr/local/lib
export GRPC_VERBOSITY=debug
export GRPC_TRACES=all
./server

# Open another terminal to run the client.
docker exec -it grpc-mytest /bin/bash
export CPLUS_INCLUDE_PATH=/usr/local/include
export LD_LIBRARY_PATH=/usr/local/lib
export GRPC_VERBOSITY=debug
export GRPC_TRACES=all
./client


5.NOTE:
After the gRPC times out, no operation is seen to start ping. As a result, the program exits.
GRPC_ARG_KEEPALIVE_TIME_MS = 60s
GRPC_ARG_KEEPALIVE_TIMEOUT_MS = 20s ( The default value )

After the 80s, there was no ping alive operation, resulting in the program quitting.

(1)Server log:
I0403 03:48:12.942834499     201 ev_epoll1_linux.cc:121]     grpc epoll fd: 3
D0403 03:48:12.942908876     201 ev_posix.cc:172]            Using polling engine: epoll1
D0403 03:48:12.943009747     201 lb_policy_registry.cc:42]   registering LB policy factory for "grpclb"
D0403 03:48:12.943032374     201 lb_policy_registry.cc:42]   registering LB policy factory for "priority_experimental"
D0403 03:48:12.943060669     201 lb_policy_registry.cc:42]   registering LB policy factory for "weighted_target_experimental"
D0403 03:48:12.943076971     201 lb_policy_registry.cc:42]   registering LB policy factory for "pick_first"
D0403 03:48:12.943091058     201 lb_policy_registry.cc:42]   registering LB policy factory for "round_robin"
D0403 03:48:12.943107097     201 lb_policy_registry.cc:42]   registering LB policy factory for "ring_hash_experimental"
D0403 03:48:12.943125369     201 dns_resolver_ares.cc:619]   Using ares dns resolver
D0403 03:48:12.943153032     201 certificate_provider_registry.cc:33] registering certificate provider factory for "file_watcher"
D0403 03:48:12.943158013     201 lb_policy_registry.cc:42]   registering LB policy factory for "cds_experimental"
D0403 03:48:12.943160402     201 lb_policy_registry.cc:42]   registering LB policy factory for "xds_cluster_impl_experimental"
D0403 03:48:12.943164586     201 lb_policy_registry.cc:42]   registering LB policy factory for "xds_cluster_resolver_experimental"
D0403 03:48:12.943167444     201 lb_policy_registry.cc:42]   registering LB policy factory for "xds_cluster_manager_experimental"
I0403 03:48:12.943265480     201 server_builder.cc:327]      Synchronous server. Num CQs: 1, Min pollers: 1, Max Pollers: 2, CQ timeout (msec): 10000
I0403 03:48:12.943573472     201 socket_utils_common_posix.cc:429] Disabling AF_INET6 sockets because ::1 is not available.
I0403 03:48:12.943627564     201 socket_utils_common_posix.cc:353] TCP_USER_TIMEOUT is available. TCP_USER_TIMEOUT will be used thereafter
I0403 03:48:12.943713514     201 tcp_server_posix.cc:333]    Failed to add :: listener, the environment may not support IPv6: {"created":"@1648957692.943609768","description":"Address family not supported by protocol","errno":97,"file":"/var/local/git/grpc/src/core/lib/iomgr/socket_utils_common_posix.cc","file_line":446,"os_error":"Address family not supported by protocol","syscall":"socket","target_address":"[::]:50055"}

I0403 03:50:08.903984140     204 chttp2_transport.cc:2831]   ipv4:127.0.0.1:55468: Keepalive watchdog fired. Closing transport.
Clear!


(2)Client log:
I0403 03:48:48.902848924     229 ev_epoll1_linux.cc:121]     grpc epoll fd: 3
D0403 03:48:48.902923032     229 ev_posix.cc:172]            Using polling engine: epoll1
D0403 03:48:48.902969970     229 lb_policy_registry.cc:42]   registering LB policy factory for "grpclb"
D0403 03:48:48.903000208     229 lb_policy_registry.cc:42]   registering LB policy factory for "priority_experimental"
D0403 03:48:48.903011842     229 lb_policy_registry.cc:42]   registering LB policy factory for "weighted_target_experimental"
D0403 03:48:48.903014369     229 lb_policy_registry.cc:42]   registering LB policy factory for "pick_first"
D0403 03:48:48.903016471     229 lb_policy_registry.cc:42]   registering LB policy factory for "round_robin"
D0403 03:48:48.903018283     229 lb_policy_registry.cc:42]   registering LB policy factory for "ring_hash_experimental"
D0403 03:48:48.903024058     229 dns_resolver_ares.cc:619]   Using ares dns resolver
D0403 03:48:48.903053371     229 certificate_provider_registry.cc:33] registering certificate provider factory for "file_watcher"
D0403 03:48:48.903068524     229 lb_policy_registry.cc:42]   registering LB policy factory for "cds_experimental"
D0403 03:48:48.903080068     229 lb_policy_registry.cc:42]   registering LB policy factory for "xds_cluster_impl_experimental"
D0403 03:48:48.903086444     229 lb_policy_registry.cc:42]   registering LB policy factory for "xds_cluster_resolver_experimental"
D0403 03:48:48.903089271     229 lb_policy_registry.cc:42]   registering LB policy factory for "xds_cluster_manager_experimental"
I0403 03:48:48.903624162     229 socket_utils_common_posix.cc:429] Disabling AF_INET6 sockets because ::1 is not available.
I0403 03:48:48.903812470     229 socket_utils_common_posix.cc:353] TCP_USER_TIMEOUT is available. TCP_USER_TIMEOUT will be used thereafter
hello 1
hello 2
hello 3
hello 4
hello 5
hello 6
hello 7
hello 8
hello 9
hello 10
hello 11
hello 12
hello 13
hello 14
hello 15
hello 16
Client StreamMessages grpc: keepalive watchdog timeout
D0403 03:50:08.904567497     229 init.cc:248]                grpc_shutdown starts clean-up now


