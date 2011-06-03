#! /usr/bin/env python

import gc
import sys
import timeit

import google.protobuf
import google_size_pb2
import google_speed_pb2

def get_class_by_name(c):
    parts = c.split('.')
    module = ".".join(parts[:-1])
    m = __import__(module)
    for comp in parts[1:]:
        m = getattr(m, comp)
    return m

class Decoder:
    def __init__(self, msgClass, data):
        self.msgClass = msgClass
        self.data = data

    def __call__(self):
        msg = self.msgClass()
        msg.ParseFromString(self.data)
        return True

class Encoder:
    def __init__(self, msg):
        self.msg = msg

    def __call__(self):
        return self.msg.SerializeToString()

def timeAction(numIterations, action):
    t = timeit.Timer(stmt=action)
    gc.collect()
    return t.timeit(number=numIterations)

def benchmark(descr, action, dataSize):
    minSampleTime = 2
    targetTime = 5

    # exercise it a little, to ensure it is loaded and what not
    timeAction(10, action)

    # find out how many iterations we need to make it run for about 30s
    iterations = 1
    elapsed = timeAction(iterations, action)
    while elapsed < minSampleTime:
        iterations = iterations * 2
        elapsed = timeAction(iterations, action)
    iterations = int(round((targetTime / elapsed) * iterations))

    # now make it run
    elapsed = timeAction(iterations, action)
    print "%s: %d iterations in %.3fs; %.2fMB/s" % \
          (descr, iterations,
           elapsed,
           float(iterations * dataSize) / (elapsed * 1024*1024))
    sys.stdout.flush()

def main(args):
    programName = args[0]
    args = args[1:]
    while len(args) >= 2:
        className = args[0]
        msgFile = args[1]
        args = args[2:]

        print "Benchmarking %s with file %s" % (className, msgFile)
        sys.stdout.flush()

        f = open(msgFile, "rb")
        data = f.read()
        f.close()
        c = get_class_by_name(className)
        d = Decoder(c, data)
        msg = c()
        msg.ParseFromString(data)
        e = Encoder(msg)

        benchmark("Serialize to string", e, len(data))
        benchmark("Deserialize from string", d, len(data))

        print
        sys.stdout.flush()

if __name__ == '__main__':
    main(sys.argv)
