#!/bin/sh

killall vsftpd

ip link set eth0 up


ifconfig eth0 192.168.0.17

./tinyftp -p 21 -c $PWD &

