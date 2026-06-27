.cpp.o:
	c++ -c $< `wx-config --cxxflags` -O

wxfirstwork: wxfirstwork.o
	c++ $< `wx-config core base adv --libs` -o $@

clean:
	rm *.o

install:
	cp wxfirstwork /home/larry/bin
