# lv_ev3
LittlevGL configured to run on the LEGO EV3 brick

# Setup

1. You need [ev3dev](https://www.ev3dev.org/) installed on your brick, and the ability to transfer programs to it.
2. Run `docker build --build-arg uid=$UID --tag lv_ev3cc --file Dockerfile .` to download the cross compiler.
3. Run `docker run --rm -it -v $PWD:/src -w /src lv_ev3cc make -j4` to build.
4. A file named `demo` will be present in the root directory. Copy this to ev3dev and run it as you would any other program or Python script.
