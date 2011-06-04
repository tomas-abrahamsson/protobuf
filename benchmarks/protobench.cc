#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <google_speed.pb.h>
#include <google_size.pb.h>
#include <google/protobuf/message_lite.h>

static void benchmark(const char *, const char *,
                      ::google::protobuf::Message *,
                      unsigned char *, int);
static int read_message(const char *, unsigned char **, int *);

int
main(int argc, char **argv)
{
    benchmarks::SpeedMessage1 *m1;
    benchmarks::SpeedMessage2 *m2;
    benchmarks::SizeMessage1 *m3;
    benchmarks::SizeMessage2 *m4;
    unsigned char *data1;
    int data1Len;
    unsigned char *data2;
    int data2Len;

    if (!read_message("google_message1.dat", &data1, &data1Len))
    {
        fprintf(stderr, "Failed top read \"%s\"\n", "google_message1.dat");
        return 1;
    }
    if (!read_message("google_message2.dat", &data2, &data2Len))
    {
        fprintf(stderr, "Failed top read \"%s\"\n", "google_message2.dat");
        return 1;
    }
    /*
      printf("Read %d bytes into %x\n", data1Len, (unsigned int)data1);
      printf("Read %d bytes into %x\n", data2Len, (unsigned int)data2);
    */


    m1 = new benchmarks::SpeedMessage1();
    m2 = new benchmarks::SpeedMessage2();
    m3 = new benchmarks::SizeMessage1();
    m4 = new benchmarks::SizeMessage2();

    benchmark("SpeedMessage1", "google_message1.dat", m1, data1, data1Len);
    benchmark("SpeedMessage2", "google_message2.dat", m2, data2, data2Len);
    benchmark("SizeMessage1",  "google_message1.dat", m3, data1, data1Len);
    benchmark("SizeMessage2",  "google_message2.dat", m4, data2, data2Len);

    delete m1;
    delete m2;
    delete m3;
    delete m4;
    return 0;
}

#define TIME_ACTION(numIterations, action, result)              \
    {                                                           \
        struct timeval t0, t1, res;                             \
        long i = numIterations;                                 \
        gettimeofday(&t0, NULL);                                \
        for (i = 0; i < numIterations; i++)                     \
        {                                                       \
            action ;                                            \
        }                                                       \
        gettimeofday(&t1, NULL);                                \
        timersub(&t1, &t0, &res);                                       \
        result = (double)(res.tv_sec) + (double)res.tv_usec / 1000000.0; \
    }

#define BENCHMARK(descr, minSampleTime, targetTime, op, dataLen)        \
    {                                                                   \
        double minSampleT = minSampleTime;                              \
        double targetT = targetTime;                                    \
        double elapsed;                                                 \
        long iterations;                                                \
                                                                        \
        iterations = 1;                                                 \
        TIME_ACTION(iterations, op, elapsed);                           \
        while (elapsed < minSampleT)                                    \
        {                                                               \
            iterations *= 2;                                            \
            TIME_ACTION(iterations, op, elapsed);                       \
        }                                                               \
        iterations = (long)((targetT / elapsed) * (double)iterations);  \
                                                                        \
        TIME_ACTION(iterations, op, elapsed);                           \
        printf("%s: %ld iterations in %.3fs; %.2fMB/s\n",               \
               descr, iterations, elapsed,                              \
               ((double)iterations*(double)dataLen) /                   \
               (elapsed * 1024.0 * 1024.0));                            \
    }

static void
benchmark(const char *classname, const char *filename,
          ::google::protobuf::Message *m,
          unsigned char *src, int len)
{
    unsigned char dest[len+1000];
    int destLen = len+100;
    double minSampleTime = 2.0;
    double targetTime = 30.0;

    if (!m->ParseFromArray(src, len))
    {
        printf("parsing message...failed :(\n");
        return;
    }
    if (!m->SerializeToArray(dest, destLen))
    {
        printf("serializing message...failed :(\n");
        return;
    }

    printf("Benchmarking %s with file %s\n", classname, filename);
    BENCHMARK("Serialize to array", minSampleTime, targetTime,
              (m->SerializeToArray(dest,destLen)),
              len);

    BENCHMARK("Deserialize from array", minSampleTime, targetTime,
              (m->ParseFromArray(src,len)),
              len);
    printf("\n");
}

static int
read_message(const char *filename, unsigned char **dest, int *len)
{
    struct stat st;
    int fd, numRemaining, numRead, error;

    if (stat(filename, &st)==-1)
    {
        return 0;
    }

    *len = st.st_size;
    if ((*dest = (unsigned char *)malloc(st.st_size)) == NULL)
        return 0;

    if ((fd = open(filename, O_RDONLY)) == -1)
    {
        free(*dest);
        return 0;
    }

    numRemaining = (int)st.st_size;
    numRead = 0;
    error = 0;
    do
    {
        int res;
        switch (res = read(fd, (*dest) + numRead, numRemaining))
        {
            case 0:
                numRemaining = 0;
                break;
            case -1:
                switch (errno) {
                    case EINTR: break;
                    default: error = 1;
                }
                break;
            default:
                numRemaining -= (int)res;
                numRead += (int)res;
                break;
        }
    } while (numRemaining > 0 && !error);

    close(fd);

    if (error)
    {
        free(*dest);
        return 0;
    }

    return 1;
}
