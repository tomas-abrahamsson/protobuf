protoc = ../src/protoc

descriptors = src/main/java/com/google/protobuf/DescriptorProtos.java

javasrc := $(wildcard src/main/java/com/google/protobuf/*.java)

all: $(descriptors) protobuf.jar

clean:
	$(RM) protobuf.jar
	$(RM) src/main/java/com/google/protobuf/*.class
	$(RM) $(descriptors)

protobuf.jar: $(descriptors) $(javasrc)
	cd src/main/java && \
	javac com/google/protobuf/*.java && \
	jar cvf ../../../protobuf.jar com/google/protobuf/*.class

$(descriptors): ../src/google/protobuf/descriptor.proto
	$(protoc) --java_out=src/main/java \
		-I../src ../src/google/protobuf/descriptor.proto
