# run code with $ make
# to remove temporary variables, run $ make clean

CXXFLAGS = -std=c++17 -O3 -w -I./ -I./lib -Wfatal-errors # include current directory and ./lib in search path.
DEBUGFLAGS = -g -rdynamic -D BOOST_STACKTRACE_USE_ADDR2LINE -ldl -fno-pie -no-pie
EMITFLAGS = -emit-llvm -S
BUILD_NAME = main
EMIT_NAME = $(word 1, $(notdir $(f:.cpp=.ll)))

all: build 

build: builds
	g++ $(CXXFLAGS) $(f) $(arg) -o ./builds/$(BUILD_NAME)

experiments: builds system_info.txt
	cd ./experiments/Experiments_9_12_20/ && python3 Driver.py compile && python3 Driver.py run && python3 Analyser.py && python3 CollectResults.py

debug: builds
	g++ $(CXXFLAGS) $(f) $(arg) -o ./builds/$(BUILD_NAME)

emit: builds
	g++ $(EMITFLAGS) $(f) $(arg) $(CXXFLAGS) -o ./builds/$(EMIT_NAME)

builds:
	[ -d builds ] || mkdir -p builds

system_info.txt:
	uname -a > system_info.txt && lscpu >> system_info.txt && cat /proc/cpuinfo >> system_info.txt
clean:
	rm -rf builds
	rm -f *.trace

.PHONY: all build emit clean
