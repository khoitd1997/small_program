FROM ubuntu:focal
USER root

RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install git-lfs xxd build-essential datamash gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat libsdl1.2-dev xterm python3 tar libtinfo5 -y