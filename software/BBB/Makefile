SERVER_OBJS = server.o tcpSocket.o commSocket.o videoSocket.o streamSocket.o dataHandler.o
CLIENT_OBJS = client.o tcpSocket.o controllerInput.o i2cDevice.o i2cManager.o IRTempSensor.o MotorController.o AmbientTempSensor.o RGBColorSensor.o
#HumiditySensor.o
#PressureSensor.o
#LightSensor.o

CXX = g++
LD = g++

# These are the options we pass to the compiler.
# -std=c++1y means we want to use the C++14 standard (called 1y in this version of Clang).
# -stdlib=libc++ specifies that we want to use the standard library implementation called libc++
# -c specifies making an object file, as you saw before
# -g specifies that we want to include "debugging symbols" which allows us to use a debugging program.
# -O0 specifies to do no optimizations on our code.
# -Wall, -Wextra, and -pedantic tells the compiler to look out for common problems with our code. -Werror makes it so that these warnings stop compilation.
CXXFLAGS = -std=c++0x -g -c
# -std=c++1y -stdlib=libc++ -c -g -O0 -Wall -Wextra -Werror -pedantic -std=c++0x

# These are the options we pass to the linker.
# The first two are the same as the compiler flags.
# -l<something> tells the linker to go look in the system for pre-installed object files to link with.
# Here we want to link with the object files from libpng (since we use it in our code) and libc++. Remember libc++ is the standard library implementation.
LDFLAGS = -std=c++0x -pthread -lssl -lcrypto
#-std=c++1y -stdlib=libc++ -lpng -lc++abi

all: server client

server: $(SERVER_OBJS)
	$(LD) $(SERVER_OBJS) $(LDFLAGS) -o server

client: $(CLIENT_OBJS)
	$(LD) $(CLIENT_OBJS) $(LDFLAGS) -o client

server.o: server.cpp
	$(CXX) $(CXXFLAGS) server.cpp

client.o: client.cpp
	$(CXX) $(CXXFLAGS) client.cpp

tcpSocket.o: tcpSocket.cpp
	$(CXX) $(CXXFLAGS) tcpSocket.cpp

commSocket.o: commSocket.cpp
	$(CXX) $(CXXFLAGS) commSocket.cpp

videoSocket.o: videoSocket.cpp
	$(CXX) $(CXXFLAGS) videoSocket.cpp

streamSocket.o: streamSocket.cpp
	$(CXX) $(CXXFLAGS) streamSocket.cpp

dataHandler.o: dataHandler.cpp
	$(CXX) $(CXXFLAGS) dataHandler.cpp

controllerInput.o: controllerInput.cpp
	$(CXX) $(CXXFLAGS) controllerInput.cpp

i2cDevice.o: i2cDevice.cpp
	$(CXX) $(CXXFLAGS) i2cDevice.cpp

i2cManager.o: i2cManager.cpp i2cManager.h
	$(CXX) $(CXXFLAGS) i2cManager.cpp

IRTempSensor.o : IRTempSensor.cpp IRTempSensor.h
	$(CXX) $(CXXFLAGS) IRTempSensor.cpp

MotorController.o : MotorController.cpp MotorController.h
	$(CXX) $(CXXFLAGS) MotorController.cpp

AmbientTempSensor.o : AmbientTempSensor.cpp
	$(CXX) $(CXXFLAGS) AmbientTempSensor.cpp

#HumiditySensor.o : HumiditySensor.cpp
#	$(CXX) $(CXXFLAGS) HumiditySensor.cpp

#LightSensor.o : LightSensor.cpp
#	$(CXX) $(CXXFLAGS) LightSensor.cpp

RGBColorSensor.o : RGBColorSensor.cpp
	$(CXX) $(CXXFLAGS) RGBColorSensor.cpp

#PressureSensor.o : PressureSensor.cpp
#	$(CXX) $(CXXFLAGS) PressureSensor.cpp

clean:
	-rm -rf *.o server client
