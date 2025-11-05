DEFINE_VARS = ARCHIVE_LOCATION='"$(shell pwd)/archive/"'

all: controller.s
	g++ -std=c++23 main.cpp controller.s -D $(DEFINE_VARS) -I ./src -o skinner
	rm ./controller.s

controller.s:
	g++ -std=c++23 ./src/Skinningcontroller.cpp -D $(DEFINE_VARS) -S -o controller.s