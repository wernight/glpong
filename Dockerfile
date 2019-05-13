FROM debian:stretch

# Package dependencies.
RUN apt update && apt install -y \
		build-essential \
		cmake \
		libglu1-mesa-dev \
		libsdl1.2-dev \
		libsdl-image1.2-dev \
 && apt clean \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /code
CMD ["/code/make.sh"]
