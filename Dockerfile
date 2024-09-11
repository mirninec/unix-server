FROM gcc:14

RUN apt-get update && \
    apt-get install -y \
    libjson-c-dev dnsutils \
    libmaxminddb0 libmaxminddb-dev mmdb-bin \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . /app

RUN gcc -o unix-server unix-server.c geo_lookup.c -lmaxminddb -ljson-c

CMD ["/app/unix-server"]

