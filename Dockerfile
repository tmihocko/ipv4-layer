FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
	build-essential \
	cmake \
	clang \
	iproute2 \
	iputils-ping \
	tcpdump \
	&& rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake \
	-S . \
	-B build \
	-DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_CXX_COMPILER=clang++ \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

RUN cmake --build build -j

CMD ["./bin/Ipv4Stack"]