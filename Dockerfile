FROM ev3dev/debian-stretch-cross
ARG uid
RUN if ! test -n "$uid"; then echo "UID must be specified as argument"; exit 1; fi
RUN sudo apt-get update
RUN sudo apt-get install -y openssh-client
RUN sudo groupadd -r -g $uid appuser
RUN sudo useradd -m -r -u $uid -g appuser appuser
USER appuser
