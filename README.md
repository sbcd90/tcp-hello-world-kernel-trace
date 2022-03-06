tcp-hello-world-kernel-tracing
==============================

This is a simple project which shows how to use Linux Kernel static tracepoints to trace details of tcp packets.

C++ files
=========

- [HelloServer.cpp](src/HelloServer.cpp)
- [HelloClient.cpp](src/HelloClient.cpp)

Tracepoints used
================

```
events/net/netif_receive_skb/enable
events/tcp/enable
```

Execute
=======

```
cd /sys/kernel/debug/tracing
echo 0 > events/enable
echo 1 > events/net/netif_receive_skb/enable
echo 0 > trace
echo nop > current_tracer (other options function, function_graph)
echo 1 > tracing_on

./tcp_hello_server 8080
./tcp_hello_client 127.0.0.1 8080

echo 0 > tracing_on
cat trace | head -100
```

Traces
======

- [netif_receive_skb trace](trace/net_receive_skb_trace.txt)
- [tcp trace](trace/tcp_trace.txt)

Next steps
==========

- [Analyze a packet's journey](https://blog.yadutaf.fr/2017/07/28/tracing-a-packet-journey-using-linux-tracepoints-perf-ebpf/)
