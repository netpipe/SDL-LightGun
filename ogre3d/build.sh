#!/bin/bash
g++ main.cpp -lOgreMain -lSDL2_mixer -lSDL2 -lSDL2_image -lGL -lOgreOverlay -L./ogre/lib -I./ogre/Samples/Common/include -I./ogre/OgreMain/include -I./ogre/include -I./ogre/Components/Overlay/include/ -I./ogre/Components/Bites/include/ -o flow

#-I/usr/local/include/OGRE/Overlay
#-I/usr/local/include/OGRE/Bites
#-I/usr/include/OGRE/Overlay
#-I/usr/include/OGRE/Bites

export LD_LIBRARY_PATH=$PWD/ogre/lib
./flow
