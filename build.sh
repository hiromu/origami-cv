#!/bin/bash

g++ -I/usr/src/opencv/opencv/modules/gpu/include -I/usr/src/opencv/opencv/modules/videostab/include -I/usr/src/opencv/opencv/modules/stitching/include -I/usr/src/opencv/opencv/modules/contrib/include -I/usr/src/opencv/opencv/modules/legacy/include -I/usr/src/opencv/opencv/modules/calib3d/include -I/usr/src/opencv/opencv/modules/features2d/include -I/usr/src/opencv/opencv/modules/softcascade/include -I/usr/src/opencv/opencv/modules/nonfree/include -I/usr/src/opencv/opencv/modules/photo/include -I/usr/src/opencv/opencv/modules/objdetect/include -I/usr/src/opencv/opencv/modules/video/include -I/usr/src/opencv/opencv/modules/ml/include -I/usr/src/opencv/opencv/modules/highgui/include -I/usr/src/opencv/opencv/modules/imgproc/include -I/usr/src/opencv/opencv/modules/flann/include -I/usr/src/opencv/opencv/modules/core/include -I/usr/src/opencv/opencv/include -I/usr/src/opencv/opencv -o main.cpp.o -c main.cpp -std=c++0x -g
g++ main.cpp.o -o main /usr/src/opencv/opencv/lib/libopencv_core.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_flann.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_imgproc.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_highgui.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_ml.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_video.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_objdetect.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_photo.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_nonfree.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_softcascade.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_features2d.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_calib3d.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_legacy.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_contrib.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_stitching.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_videostab.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_gpu.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_nonfree.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_gpu.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_objdetect.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_photo.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_legacy.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_ml.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_video.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_calib3d.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_features2d.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_flann.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_highgui.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_imgproc.so.2.4.9 /usr/src/opencv/opencv/lib/libopencv_core.so.2.4.9 -Wl,-rpath,/usr/src/opencv/opencv/lib -std=c++0x -g
