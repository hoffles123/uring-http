# uring-http

`uring-http` is a simple HTTP server that serves static files. It leverages the Linux io_uring for asynchronous I/O. This project was an attempt to explore basic computer networking concepts such as socket programming, event-based I/O and HTTP.

## Design

* Leverages `liburing` (a interface wrapper simplifying io_uring operations) for handling async I/O, such as accept(), send(), recv(), reducing the number of system calls.
* Implements a thread pool to utilize all logical processors for processing HTTP requests.
* Serves contents of a static file to the client based on path provided in the request

## Build

* Linux Kernel >= 5.19
* GCC >= 13 or Clang >= 14
* liburing >= 2.3

Generate build configuration with CMake:
```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ -B build -G "Unix Makefiles"
```
Build and run uring_http, which will listen on localhost:8080:
```
make -C build -j$(nproc)
./build/uring_http
```

## Benchmark
The benchmark is performed with the hey benchmark tool, which sends 200 batches of requests, with each batch containing 5,000 concurrent clients requesting a file of 1024 bytes in size. uring-http serves 57,012 requests per second and handles 99% of requests within 0.2 seconds. 

Benchmark was performed on UTM running on a M1 Pro with Linux kernel version 6.4.2. The virtual machine has 4 cores and 8 GB memories.
```
hey -n 1000000 -c 5000 http://127.0.0.1:8080/test

Summary:
  Total:        17.5400 secs
  Slowest:      0.3872 secs
  Fastest:      0.0001 secs
  Average:      0.0824 secs
  Requests/sec: 57012.6903

  Total data:   1024000000 bytes
  Size/request: 1024 bytes

Response time histogram:
  0.000 [1]     |
  0.039 [18601] |■
  0.077 [516164]        |■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  0.116 [344028]        |■■■■■■■■■■■■■■■■■■■■■■■■■■■
  0.155 [89199] |■■■■■■■
  0.194 [22404] |■■
  0.232 [3841]  |
  0.271 [3404]  |
  0.310 [1907]  |
  0.348 [221]   |
  0.387 [230]   |


Latency distribution:
  10% in 0.0519 secs
  25% in 0.0611 secs
  50% in 0.0752 secs
  75% in 0.0973 secs
  90% in 0.1213 secs
  95% in 0.1392 secs
  99% in 0.1922 secs

Details (average, fastest, slowest):
  DNS+dialup:   0.0002 secs, 0.0001 secs, 0.3872 secs
  DNS-lookup:   0.0000 secs, 0.0000 secs, 0.0000 secs
  req write:    0.0001 secs, 0.0000 secs, 0.0966 secs
  resp wait:    0.0413 secs, 0.0000 secs, 0.2908 secs
  resp read:    0.0397 secs, 0.0000 secs, 0.3499 secs

Status code distribution:
  [200] 1000000 responses
```
