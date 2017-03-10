#!/bin/sh

killall vsftpd

ip link set eth0 up
ip addr add 192.168.0.17/24 dev eth0

./tinyftp -p 21 -c $PWD &

