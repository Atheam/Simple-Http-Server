cc = g++ -std=c++17
files = main.cpp TcpConnectionHandler.cpp HttpRequestHandler.cpp -o main
run: 
	$(cc) $(files) -lpthread
	./main
	


