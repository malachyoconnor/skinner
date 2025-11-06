DEFINE_VARS = ARCHIVE_LOCATION='"$(shell pwd)/archive/"'

hmm:
	g++ -std=c++23 main.cpp "./src/SkinningController.cpp" "./src/SkinningInterval.cpp" -D $(DEFINE_VARS) -I ./src -o skinner
