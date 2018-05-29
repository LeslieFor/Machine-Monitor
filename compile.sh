#!/bin/sh
gcc -o demo demo.c system_info.c -lpthread

gcc -o net_demo net_demo.c system_net.c les_external_ip.c les_curl.c les_nat_check.c -lcurl
