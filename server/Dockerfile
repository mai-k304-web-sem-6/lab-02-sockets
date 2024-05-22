FROM debian:latest

WORKDIR /usr/src/app

RUN apt-get update && apt-get install -y \
    gcc \
    libc-dev \
    make \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN gcc -o server server.c

EXPOSE 5000

CMD ["./server"]
