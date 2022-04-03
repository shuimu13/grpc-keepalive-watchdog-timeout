FROM ubuntu:18.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN set -eux \
   && apt-get update \
   && apt-get install --no-install-recommends --yes \
      # Needed to build everything
      build-essential \
      wget \
      git \
      ca-certificates \
      zlib1g-dev \
      libssl-dev \
      autoconf \
      libtool \
      pkg-config \
   # cmake
   && wget -O cmake-linux.sh https://cmake.org/files/v3.19/cmake-3.19.6-Linux-x86_64.sh \
   && sh cmake-linux.sh -- --skip-license --prefix=/usr/local/ \
   && rm cmake-linux.sh \
   # grpc
   && git clone -v -b v1.45.1 https://github.com/grpc/grpc /var/local/git/grpc \
   && cd /var/local/git/grpc \
   && git submodule update --init \
   && cmake -DBUILD_SHARED_LIBS=ON \
      -DgRPC_INSTALL=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -DgRPC_BUILD_TESTS=OFF \
      -DgRPC_SSL_PROVIDER=package \
      -DgRPC_ZLIB_PROVIDER=package \
   && make -j4 \
   && make install \
   && make clean \
   # grpc submodule
   && cd /var/local/git/grpc/third_party/abseil-cpp/ \
   && cmake -DBUILD_SHARED_LIBS=ON \
      -DgRPC_INSTALL=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
   && make -j4 \
   && make install \
   && make clean \
   && cd \
   && rm -rf /var/local/git/grpc \
   # Clean up
   && apt-get autoremove --yes \
   && apt-get clean --yes \
   && rm --recursive --force /var/lib/apt/lists/*

WORKDIR /data

CMD ["bash"]
