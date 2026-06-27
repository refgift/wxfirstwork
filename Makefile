.cpp.o:
	c++ -c $< `wx-config --cxxflags` -g

wxfirstwork: wxfirstwork.o
	c++ $< `wx-config core base adv --libs` -o $@

clean:
	rm *.o

install:
	cp wxfirstwork /home/larry/bin
