#!/bin/bash

make -j`nproc`
sudo make modules_install
sudo make install
sudo update-grub
