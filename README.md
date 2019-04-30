Introduction
========

yijinjing-lite is a standalone version of yijinjing in [kungfu](https://github.com/taurusai/kungfu).
We truly focus on low-latency in-process communication.

Compile
========
Please setup proper location of Boost in file env-config.cmake and follow a standard way of cmake process:
```
cd path-to/yijinjing-lite
mkdir build && cd build
cmake -C ../env-config.cmake ..
make
```

Run && Test
========
run these three commands in three different terminals:
```
./yjj_page_service path-to/yijinjing-lite/test/page_engine.json
./reader
./writer
```

License
==========

Apache License 2.0 


