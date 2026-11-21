FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential gcc-mingw-w64 g++-mingw-w64 flex bison gettext python3 git patch autoconf ca-certificates \
    libx11-dev libxext-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libxcomposite-dev \
    libxfixes-dev libxrender-dev libxkbcommon-dev libxxf86vm-dev libxshmfence-dev \
    libfreetype-dev libfontconfig-dev libgnutls28-dev libdbus-1-dev libgl-dev libegl-dev libvulkan-dev \
    libasound2-dev libpulse-dev libunwind-dev libkrb5-dev libcups2-dev libudev-dev libusb-1.0-0-dev \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libsdl2-dev libpcap-dev libv4l-dev libwayland-dev \
    && rm -rf /var/lib/apt/lists/*
