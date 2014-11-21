CC = g++
CFLAGS = -c
TARGET = VolumeRenderer.exe
LIB = -lGL -lGLEW -lglut -lopencv_core -lopencv_highgui -lopencv_imgproc -lGLU
SOURCES =  Main.cpp TransformationManager.cpp
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET) : $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIB)
	@rm *.o

.cpp.o:
	$(CC) $(CFLAGS) $<

clean:
	@rm *.o
	@rm $(TARGET)

