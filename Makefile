CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread
SOURCES = src/main.cpp src/ConnectionMonitor.cpp
TARGET = monitor

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean run