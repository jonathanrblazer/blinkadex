FROM --platform=linux/arm64 debian:bookworm-slim

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        g++ \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY src/main.cpp .

RUN g++ -O2 -std=c++17 main.cpp -o pwm-blink

CMD ["./pwm-blink"]
