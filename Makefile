.cpp.o:
	g++ -c $< `wx-config --cxxflags` -g

wxfirstwork: wxfirstwork.o
	g++ $< `wx-config core base adv --libs` -o $@

clean:
	rm *.o

install:
	cp wxfirstwork /home/larry/bin
