protoc = ../src/protoc
protobuf_jar = ../java/protobuf.jar

all:	benchmarks-java-code benchmarks-python-code benchmarks-cc-code
	@echo
	@echo "consider $(MAKE) benchmarks"
	@echo "      or $(MAKE) java-benchmarks"
	@echo "      or $(MAKE) python-benchmarks"
	@echo

topjavaclasses =	jtmp/benchmarks/GoogleSpeed.class \
			jtmp/benchmarks/GoogleSize.class

toppythonmodules =	pytmp/google_speed_pb2.py \
			pytmp/google_size_pb2.py

topccfiles =	cctmp/google_speed.pb.o \
		cctmp/google_size.pb.o


benchmarks-java-code: $(topjavaclasses)

$(topjavaclasses): $(protobuf_jar) $(wildcard *.java) $(wildcard *.proto)
	[ -d jtmp ] || mkdir jtmp
	javac -d jtmp -cp $(protobuf_jar) ProtoBench.java
	$(protoc) --java_out=jtmp google_size.proto google_speed.proto
	cd jtmp && javac -d . -cp ../$(protobuf_jar) benchmarks/*.java

benchmarks-python-code: $(toppythonmodules)

$(toppythonmodules): $(wildcard *.proto)
	[ -d pytmp ] || mkdir pytmp
	$(protoc) --python_out=pytmp google_size.proto google_speed.proto

benchmarks-cc-code: $(topccfiles) cctmp/protobench
benchmarks-cc-static-code: $(topccfiles) cctmp/protobench-static

$(topccfiles): $(wildcard *.proto)
	[ -d cctmp ] || mkdir cctmp
	$(protoc) --cpp_out=cctmp google_size.proto google_speed.proto
	cd cctmp; g++ -I../../src -c google_speed.pb.cc
	cd cctmp; g++ -I../../src -c google_size.pb.cc

cctmp/protobench: protobench.cc
	g++ -Wall -I../src -Icctmp -o $@ $< \
	  cctmp/google_speed.pb.o cctmp/google_size.pb.o \
	  -L../src/.libs -lprotobuf -Wl,-rpath=`pwd`/../src/.libs

cctmp/protobench-static: protobench.cc
	g++ -Wall -I../src -Icctmp -o $@ $< \
	  cctmp/google_speed.pb.o cctmp/google_size.pb.o \
	  ../src/.libs/libprotobuf.a -lpthread


clean:
	$(RM) -r jtmp pytmp cctmp

benchmarks: java-benchmarks python-benchmarks cc-benchmarks

java-benchmarks: benchmarks-java-code
	@echo CPU info
	@egrep '^model name' /proc/cpuinfo | head -1
	@egrep '^cache' /proc/cpuinfo | head -1
	@printf "# cores/threads : %s\n" `egrep -c '^processor' /proc/cpuinfo`
	@egrep '^bogomips' /proc/cpuinfo | head -1
	@echo
	java -version
	@echo
	cd jtmp && \
	java -cp ".:../$(protobuf_jar)" com.google.protocolbuffers.ProtoBench \
          benchmarks.GoogleSize'$$'SizeMessage1   ../google_message1.dat \
          benchmarks.GoogleSpeed'$$'SpeedMessage1 ../google_message1.dat \
          benchmarks.GoogleSize'$$'SizeMessage2   ../google_message2.dat \
          benchmarks.GoogleSpeed'$$'SpeedMessage2 ../google_message2.dat

python-benchmarks:  benchmarks-python-code
	@echo CPU info
	@egrep '^model name' /proc/cpuinfo | head -1
	@egrep '^cache' /proc/cpuinfo | head -1
	@printf "# cores/threads : %s\n" `egrep -c '^processor' /proc/cpuinfo`
	@egrep '^bogomips' /proc/cpuinfo | head -1
	@echo
	python -V
	@echo
	env PYTHONPATH=pytmp:../python python protobench.py \
          google_speed_pb2.SpeedMessage1 google_message1.dat \
          google_speed_pb2.SpeedMessage2 google_message2.dat

cc-benchmarks:  benchmarks-cc-code
	@echo CPU info
	@egrep '^model name' /proc/cpuinfo | head -1
	@egrep '^cache' /proc/cpuinfo | head -1
	@printf "# cores/threads : %s\n" `egrep -c '^processor' /proc/cpuinfo`
	@egrep '^bogomips' /proc/cpuinfo | head -1
	@echo
	gcc --version
	@echo
	cctmp/protobench

cc-static-benchmarks:  benchmarks-cc-static-code
	@echo CPU info
	@egrep '^model name' /proc/cpuinfo | head -1
	@egrep '^cache' /proc/cpuinfo | head -1
	@printf "# cores/threads : %s\n" `egrep -c '^processor' /proc/cpuinfo`
	@egrep '^bogomips' /proc/cpuinfo | head -1
	@echo
	gcc --version
	@echo
	cctmp/protobench-static
