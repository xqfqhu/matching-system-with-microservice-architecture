all: clientA clientB central serverT serverS serverP
clientA: util.cpp client/util.cpp client/clientA.cpp 
	g++ -std=gnu++11 -g -o clientA util.cpp client/util.cpp client/clientA.cpp
clientB: util.cpp client/util.cpp client/clientB.cpp 
	g++ -std=gnu++11 -g -o clientB util.cpp client/util.cpp client/clientB.cpp
central: util.cpp server/central.cpp
	g++ -std=gnu++11 -g -o central util.cpp server/central.cpp
serverT: util.cpp server/serverT.cpp
	g++ -std=gnu++11 -g -o serverT util.cpp server/serverT.cpp
serverS: util.cpp server/serverS.cpp
	g++ -std=gnu++11 -g -o serverS util.cpp server/serverS.cpp
serverP: util.cpp server/serverP.cpp
	g++ -std=gnu++11 -g -o serverP util.cpp server/serverP.cpp