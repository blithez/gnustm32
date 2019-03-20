#!/bin/sh
if [ ! -d "./out" ]; then
  mkdir ./out
fi
make -j4
