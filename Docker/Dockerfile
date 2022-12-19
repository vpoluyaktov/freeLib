#FROM ubuntu
FROM ghcr.io/linuxserver/baseimage-guacgui:amd64-version-1e387109

# Install dependencies
RUN apt-get update -y  \
    && DEBIAN_FRONTEND="noninteractive" apt-get install -y \
       build-essential \
       git \
       libquazip*  \
       qt5-default \
       qttools5-dev-tools \
       qttools5-dev \
       qtwebengine5-dev \
       libqt5xmlpatterns5-dev 
#       qtcreator

# http://download.qt.io/official_releases/qt/5.15/5.15.2/single/qt-everywhere-src-5.15.2.tar.xz
RUN git clone --recurse-submodules https://github.com/DikBSD/freeLib.git \
 && cd freeLib \
 && git checkout ec87dc30a471fb196422c8c708a91b968bbef36a \
 && cp freeLib/src/quazip/quazip/minizip_crypt.h freeLib/src/quazip/quazip/crypt.h \
 && qmake freeLib/Lib_linux.pro -r -spec linux-g++ \
 && make all

COPY root/ /

