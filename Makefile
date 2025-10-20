DEFINE_VARS = ARCHIVE_LOCATION='"$(shell pwd)/archive/"'

all: commander.s
	g++ -std=c++23 main.cpp commander.s -D $(DEFINE_VARS) -I ./src -o skinner
	rm ./commander.s

commander.s:
	g++ -std=c++23 ./src/SkinningCommander.cpp -D $(DEFINE_VARS) -S -o commander.s