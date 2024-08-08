#!/bin/sh

set -xe

gcc -g main.c -o dolus -lm

./dolus
