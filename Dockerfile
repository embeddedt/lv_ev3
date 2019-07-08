FROM ev3dev/debian-stretch-cross
ARG uid
RUN if ! test -n "$uid"; then echo "UID must be specified as argument"; exit 1; fi
RUN sudo apt-get update
RUN sudo apt-get install -y openssh-client git
RUN sudo groupadd -r -g $uid appuser
RUN sudo useradd -m -r -u $uid -g appuser appuser
RUN sudo apt-get install -y libffi-dev pkg-config python python3
USER appuser

