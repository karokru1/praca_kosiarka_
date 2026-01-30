FROM alpine:3.21.3

WORKDIR /app

RUN apk add --no-cache \
    cmake=3.31.1-r0 \
    make=4.4.1-r2 \
    g++=14.2.0-r4 \
    git=2.47.3-r0 \
    llvm19=19.1.4-r1 \
    lcov=2.0-r2 \
    gzip=1.13-r0