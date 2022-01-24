FROM ubuntu:focal
USER root

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install locales git-lfs xxd build-essential datamash gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat libsdl1.2-dev xterm python3 tar libtinfo5 cpio -y
RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8