FROM ubuntu:xenial as builder

RUN apt-get clean
RUN apt-get update && \
    apt-get install -y --no-install-recommends ca-certificates gnupg \
           build-essential python wget subversion && \
    rm -rf /var/lib/apt/lists/*
RUN wget https://github.com/llvm/llvm-project/releases/download/llvmorg-11.1.0-rc3/clang+llvm-11.1.0-rc3-x86_64-linux-gnu-ubuntu-16.04.tar.xz -O /tmp/clang.tar.xz
RUN cd /tmp && \
    tar -xf clang.tar.xz --no-same-owner && \
    cd clang+llvm-11.1.0-rc3-x86_64-linux-gnu-ubuntu-16.04 && \
    cp -r /tmp/clang+llvm-11.1.0-rc3-x86_64-linux-gnu-ubuntu-16.04/* /usr/local/