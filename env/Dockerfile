FROM debian:stable-slim

# Install basic tools
RUN apt-get update && apt-get install -y curl python3 python3-pip git && \
    apt-get clean

# Install Arduino CLI
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

# Create workspace
WORKDIR /workspace

CMD ["bash"]
