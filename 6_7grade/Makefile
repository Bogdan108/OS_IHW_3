all: TCPClientRead TCPClientWrite TCPServer TCPClientVisit
TCPClientRead: TCPClientReader.c
	gcc TCPClientReader.c  -o TCPClientRead
TCPClientWrite: TCPClientWriter.c 
	gcc TCPClientWriter.c  -o TCPClientWrite
TCPClientVisit: TCPClientVisitor.c
	gcc TCPClientVisitor.c  -o TCPClientVisit
TCPServer: TCPServer.c 
	gcc TCPServer.c -o TCPServer
