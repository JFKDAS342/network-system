CXX = g++
CXXFLAGS = -std=c++11 -Wall
INCLUDES = -Iinclude
SOURCES = src/main.cpp src/ConnectionMonitor.cpp
TARGET = netmon

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean
