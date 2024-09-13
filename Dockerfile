FROM  ubuntu:20.04

RUN apt-get update && apt-get install dnsutils -y

WORKDIR /app

COPY GeoLite2-City.mmdb unix-server ./

CMD ["/app/unix-server"]
