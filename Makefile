LIBS = -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_softcascade -lopencv_stitching -lopencv_video -lopencv_videostab
OBJS = main.o sand.o util.o
TARGET = main

CXXFLAGS = -std=c++0x -Wall -O3
INCLUDES = -I/usr/src/opencv/opencv/modules/gpu/include -I/usr/src/opencv/opencv/modules/videostab/include -I/usr/src/opencv/opencv/modules/stitching/include -I/usr/src/opencv/opencv/modules/contrib/include -I/usr/src/opencv/opencv/modules/legacy/include -I/usr/src/opencv/opencv/modules/calib3d/include -I/usr/src/opencv/opencv/modules/features2d/include -I/usr/src/opencv/opencv/modules/softcascade/include -I/usr/src/opencv/opencv/modules/nonfree/include -I/usr/src/opencv/opencv/modules/photo/include -I/usr/src/opencv/opencv/modules/objdetect/include -I/usr/src/opencv/opencv/modules/video/include -I/usr/src/opencv/opencv/modules/ml/include -I/usr/src/opencv/opencv/modules/highgui/include -I/usr/src/opencv/opencv/modules/imgproc/include -I/usr/src/opencv/opencv/modules/flann/include -I/usr/src/opencv/opencv/modules/core/include -I/usr/src/opencv/opencv/include -I/usr/src/opencv/opencv
LDFLAGS = -L/usr/src/opencv/opencv/lib

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $? $(LDFLAGS) $(LIBS)
	$(RM) $(OBJS)

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) -c $< $(INCLUDES) $(CXXFLAGS)

clean:
	$(RM) $(OBJS)
	$(RM) main
