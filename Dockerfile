FROM debian:bookworm-slim AS builder

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        gcc \
        make \
        gengetopt \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . /app

RUN make clean
RUN make 

FROM debian:bookworm-slim

WORKDIR /app

COPY --from=builder /app/bin/server ./server
COPY --from=builder /app/src/lib/libqueue.so ./libqueue.so

RUN mkdir -p /app/logs/srv /app/logs/cli

ENV LD_LIBRARY_PATH=/app
EXPOSE 8080

ENTRYPOINT ["./server"]
CMD ["--port", "8080"]
