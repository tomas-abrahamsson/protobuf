protoc = ../src/protoc
protobuf_jar = ../java/protobuf.jar

all:	benchmarks-code

topclasses =	tmp/benchmarks/GoogleSpeed.class \
		tmp/benchmarks/GoogleSize.class


benchmarks-code: $(topclasses)

$(topclasses): $(protobuf_jar) $(wildcard *.java) $(wildcard *.proto)
	[ -d tmp ] || mkdir tmp
	javac -d tmp -cp $(protobuf_jar) ProtoBench.java
	$(protoc) --java_out=tmp google_size.proto google_speed.proto
	cd tmp && javac -d . -cp ../$(protobuf_jar) benchmarks/*.java


clean:
	$(RM) -r tmp

benchmark: benchmarks-code
	@echo CPU info
	@egrep '^model name' /proc/cpuinfo | head -1
	@egrep '^cache' /proc/cpuinfo | head -1
	@printf "# cores/threads : %s\n" `egrep -c '^processor' /proc/cpuinfo`
	@egrep '^bogomips' /proc/cpuinfo | head -1
	@echo
	java -version
	@echo
	cd tmp && \
	java -cp ".:../$(protobuf_jar)" com.google.protocolbuffers.ProtoBench \
          benchmarks.GoogleSize'$$'SizeMessage1   ../google_message1.dat \
          benchmarks.GoogleSpeed'$$'SpeedMessage1 ../google_message1.dat \
          benchmarks.GoogleSize'$$'SizeMessage2   ../google_message2.dat \
          benchmarks.GoogleSpeed'$$'SpeedMessage2 ../google_message2.dat
