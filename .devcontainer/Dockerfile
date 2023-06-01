FROM ubuntu:22.04

ARG ZEPHYR_VERSION=v3.3.0
ARG ZSDK_VERSION=0.15.2
ARG DOXYGEN_VERSION=1.9.4
ARG CMAKE_VERSION=3.20.5
ARG LLVM_VERSION=15
ARG SPARSE_VERSION=9212270048c3bd23f56c20a83d4f89b870b2b26e
ARG PROTOC_VERSION=21.7
ARG WGET_ARGS="-q --show-progress --progress=bar:force:noscroll --no-check-certificate"

ARG UID=1000
ARG GID=1000

# Set default shell during Docker image build to bash
SHELL ["/bin/bash", "-c"]

# Set non-interactive frontend for apt-get to skip any user confirmations
ENV DEBIAN_FRONTEND=noninteractive

# Install base packages
RUN apt-get -y update && \
	apt-get -y upgrade && \
	apt-get install --no-install-recommends -y \
		software-properties-common \
		lsb-release \
		autoconf \
		automake \
		bison \
		build-essential \
		ca-certificates \
		ccache \
		chrpath \
		cpio \
		device-tree-compiler \
		dfu-util \
		diffstat \
		dos2unix \
		doxygen \
		file \
		flex \
		g++ \
		gawk \
		gcc \
		gcovr \
		git \
		git-core \
		gnupg \
		gperf \
		gtk-sharp2 \
		help2man \
		iproute2 \
		lcov \
		libglib2.0-dev \
		libgtk2.0-0 \
		liblocale-gettext-perl \
		libncurses5-dev \
		libpcap-dev \
		libpopt0 \
		libsdl1.2-dev \
		libsdl2-dev \
		libssl-dev \
		libtool \
		libtool-bin \
        libusb-1.0-0-dev \
		locales \
		make \
		net-tools \
		ninja-build \
		openssh-client \
		pkg-config \
		python3-dev \
		python3-pip \
		python3-ply \
		python3-setuptools \
		python-is-python3 \
		qemu \
		rsync \
		socat \
		srecord \
		sudo \
		texinfo \
		unzip \
		valgrind \
		wget \
		ovmf \
		xz-utils


# Install multi-lib gcc (x86 only)
RUN if [ "${HOSTTYPE}" = "x86_64" ]; then \
	apt-get install --no-install-recommends -y \
		gcc-multilib \
		g++-multilib \
	; fi

# Install i386 packages (x86 only)
RUN if [ "${HOSTTYPE}" = "x86_64" ]; then \
	dpkg --add-architecture i386 && \
	apt-get -y update && \
	apt-get -y upgrade && \
	apt-get install --no-install-recommends -y \
		libsdl2-dev:i386 \
	; fi

# Initialise system locale
RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# Install Doxygen (x86 only)
# NOTE: Pre-built Doxygen binaries are only available for x86_64 host.
RUN if [ "${HOSTTYPE}" = "x86_64" ]; then \
	wget ${WGET_ARGS} https://downloads.sourceforge.net/project/doxygen/rel-${DOXYGEN_VERSION}/doxygen-${DOXYGEN_VERSION}.linux.bin.tar.gz && \
	tar xf doxygen-${DOXYGEN_VERSION}.linux.bin.tar.gz -C /opt && \
	ln -s /opt/doxygen-${DOXYGEN_VERSION}/bin/doxygen /usr/local/bin && \
	rm doxygen-${DOXYGEN_VERSION}.linux.bin.tar.gz \
	; fi

# Install CMake
RUN wget ${WGET_ARGS} https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-${HOSTTYPE}.sh && \
	chmod +x cmake-${CMAKE_VERSION}-Linux-${HOSTTYPE}.sh && \
	./cmake-${CMAKE_VERSION}-Linux-${HOSTTYPE}.sh --skip-license --prefix=/usr/local && \
	rm -f ./cmake-${CMAKE_VERSION}-Linux-${HOSTTYPE}.sh

# Install Python dependencies
RUN pip3 install wheel pip -U &&\
	pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/zephyr/master/scripts/requirements.txt && \
	pip3 install -r https://raw.githubusercontent.com/zephyrproject-rtos/mcuboot/master/scripts/requirements.txt && \
	pip3 install west &&\
	pip3 install sh &&\
	pip3 install awscli PyGithub junitparser pylint \
		     statistics numpy \
		     imgtool \
		     protobuf \
		     GitPython

# # Install LLVM and Clang
# RUN wget ${WGET_ARGS} https://apt.llvm.org/llvm.sh && \
# 	chmod +x llvm.sh && \
# 	./llvm.sh ${LLVM_VERSION} all && \
# 	rm -f llvm.sh

# Install protobuf-compiler
RUN mkdir -p /opt/protoc && \
	cd /opt/protoc && \
	PROTOC_HOSTTYPE=$(case $HOSTTYPE in x86_64) echo "x86_64";; aarch64) echo "aarch_64";; esac) && \
	wget ${WGET_ARGS} https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOC_VERSION}/protoc-${PROTOC_VERSION}-linux-${PROTOC_HOSTTYPE}.zip && \
	unzip protoc-${PROTOC_VERSION}-linux-${PROTOC_HOSTTYPE}.zip && \
	ln -s /opt/protoc/bin/protoc /usr/local/bin && \
	rm -f protoc-${PROTOC_VERSION}-linux-${PROTOC_HOSTTYPE}.zip

# Install Zephyr SDK
RUN mkdir -p /opt/toolchains && \
	cd /opt/toolchains && \
	wget ${WGET_ARGS} https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZSDK_VERSION}/zephyr-sdk-${ZSDK_VERSION}_linux-${HOSTTYPE}.tar.gz && \
	tar xf zephyr-sdk-${ZSDK_VERSION}_linux-${HOSTTYPE}.tar.gz && \
	zephyr-sdk-${ZSDK_VERSION}/setup.sh -t all -h -c && \
	rm zephyr-sdk-${ZSDK_VERSION}_linux-${HOSTTYPE}.tar.gz

# Clean up stale packages
RUN apt-get clean -y && \
	apt-get autoremove --purge -y && \
	rm -rf /var/lib/apt/lists/*

# Create 'user' account
RUN groupadd -g $GID -o user

RUN useradd -u $UID -m -g user -G plugdev user \
	&& echo 'user ALL = NOPASSWD: ALL' > /etc/sudoers.d/user \
	&& chmod 0440 /etc/sudoers.d/user

# Run the Zephyr SDK setup script as 'user' in order to ensure that the
# `Zephyr-sdk` CMake package is located in the package registry under the
# user's home directory.
USER user

RUN sudo -E -- bash -c ' \
	/opt/toolchains/zephyr-sdk-${ZSDK_VERSION}/setup.sh -c && \
	chown -R user:user /home/user/.cmake \
	'

USER root

# Set the locale
ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
ENV PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig
ENV OVMF_FD_PATH=/usr/share/ovmf/OVMF.fd

# Install Zephyr
RUN \
	mkdir /zephyrproject && cd /zephyrproject && \
	west init -m https://github.com/zephyrproject-rtos/zephyr --mr ${ZEPHYR_VERSION} && \
    west update --narrow -o=--depth=1 && \
	west zephyr-export

# Remove un-needed and large packages
RUN \
	rm -rf /zephyrproject/modules/lib/canopennode && \
	rm -rf /zephyrproject/modules/lib/gui && \
	rm -rf /zephyrproject/modules/lib/openthread && \
	rm -rf /zephyrproject/modules/lib/tflite-micro && \
	rm -rf /zephyrproject/modules/hal/altera && \
	rm -rf /zephyrproject/modules/hal/atmel && \
	rm -rf /zephyrproject/modules/hal/espressif && \
	rm -rf /zephyrproject/modules/hal/gigadevice && \
	rm -rf /zephyrproject/modules/hal/infineon && \
	rm -rf /zephyrproject/modules/hal/microchip && \
	rm -rf /zephyrproject/modules/hal/nuvoton && \
	rm -rf /zephyrproject/modules/hal/nxp && \
	rm -rf /zephyrproject/modules/hal/openisa && \
	rm -rf /zephyrproject/modules/hal/quicklogic && \
	rm -rf /zephyrproject/modules/hal/renesas && \
	rm -rf /zephyrproject/modules/hal/rpi_pico && \
	rm -rf /zephyrproject/modules/hal/silabs && \
	rm -rf /zephyrproject/modules/hal/telink && \
	rm -rf /zephyrproject/modules/hal/ti && \
	rm -rf /zephyrproject/modules/hal/wurthelektronik && \
	rm -rf /zephyrproject/modules/hal/xtensa

# Install udev for serial monitor
RUN apt-get -y update && \
	apt-get -y upgrade && \
	apt-get install --no-install-recommends -y udev

# Install gdb for native debugging
RUN apt-get -y update && \
	apt-get -y upgrade && \
	apt-get install --no-install-recommends -y gdb && \
	apt-get install usbutils \
	apt install linux-tools-virtual hwdata

ENV ZEPHYR_BASE=/zephyrproject/zephyr