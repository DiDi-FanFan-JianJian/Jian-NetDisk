CXX = g++
CXXFLAGS = -Wall -std=c++11

objs = dbController.o userController.o message_parser.o
mysql_flags = `mysql_config --cxxflags --libs`

build: $(objs)
	$(CXX) $(CXXFLAGS) -I./include -o server server.cpp $(objs) $(mysql_flags)
	$(CXX) $(CXXFLAGS) -I./include -o client client.cpp

dbController.o: src/dbController.cpp
	$(CXX) $(CXXFLAGS) -c -I./include src/dbController.cpp -o dbController.o

userController.o: src/userController.cpp
	$(CXX) $(CXXFLAGS) -c -I./include src/userController.cpp -o userController.o

message_parser.o: src/message_parser.cpp
	$(CXX) $(CXXFLAGS) -c -I./include src/message_parser.cpp -o message_parser.o

clean:
	rm -f *.o server client