#! /bin/bash

rm main
clang main.c functions.c -o main --std=c99
./main
