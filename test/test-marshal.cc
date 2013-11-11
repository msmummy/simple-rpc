#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

#include "base/all.h"
#include "rpc/marshal.h"

using namespace rpc;

TEST(marshal, content_size) {
    Marshal m;
    rpc::i32 a = 4;
    EXPECT_EQ(m.content_size(), 0u);
    m << a;
    EXPECT_EQ(m.content_size(), 4u);
    rpc::i32 b = 9;
    m >> b;
    EXPECT_EQ(m.content_size(), 0u);
    EXPECT_EQ(a, b);
}


const i64 g_chunk_size = 1000;
const i64 g_bytes_per_writer = 1000 * 1000 * 1000;
Marshal g_mt_benchmark_marshal;
pthread_mutex_t g_mt_benchmark_mutex = PTHREAD_MUTEX_INITIALIZER;

static void* start_mt_benchmark_writers(void* args) {
    char* dummy_data = new char[g_chunk_size];
    memset(dummy_data, 0, g_chunk_size);
    int n_bytes_written = 0;
    while (n_bytes_written < g_bytes_per_writer) {
        int n_write = std::min(g_chunk_size, g_bytes_per_writer - n_bytes_written);
        Pthread_mutex_lock(&g_mt_benchmark_mutex);
        int ret = g_mt_benchmark_marshal.write(dummy_data, n_write);
        g_mt_benchmark_marshal.update_read_barrier();
        Pthread_mutex_unlock(&g_mt_benchmark_mutex);
        verify(ret > 0);
        n_bytes_written += ret;
    }
    delete[] dummy_data;
    pthread_exit(nullptr);
    return nullptr;
}

// multi-thread benchmark
TEST(marshal, mt_benchmark) {
    const int n_writers = 10;
    pthread_t th_writers[n_writers];

    for (int i = 0; i < n_writers; i++) {
        Pthread_create(&th_writers[i], nullptr, start_mt_benchmark_writers, nullptr);
    }

    io_ratelimit no_rate;
    int null_fd = open("/dev/null", O_WRONLY);
    i64 n_bytes_read = 0;
    double report_time = -1.0;
    Timer xfer_timer;
    xfer_timer.start();
    while (n_bytes_read < n_writers * g_bytes_per_writer) {
        Pthread_mutex_lock(&g_mt_benchmark_mutex);
        Marshal::read_barrier rb = g_mt_benchmark_marshal.get_read_barrier();
        int ret = g_mt_benchmark_marshal.write_to_fd(null_fd, rb, no_rate);
        Pthread_mutex_unlock(&g_mt_benchmark_mutex);
        verify(ret >= 0);
        n_bytes_read += ret;

        struct timeval tm;
        gettimeofday(&tm, NULL);
        double now = tm.tv_sec + tm.tv_usec / 1000.0 / 1000.0;
        if (now - report_time > 1) {
            Log::info("bytes transferred = %ld (%.2lf%%)", n_bytes_read, n_bytes_read * 100.0 / (n_writers * g_bytes_per_writer));
            report_time = now;
        }
    }
    xfer_timer.stop();
    Log::info("marshal xfer speed = %.2lf M/s (%d writers, %d bytes per write)",
        n_bytes_read / 1024.0 / 1024.0 / xfer_timer.elapsed(), n_writers, g_chunk_size);
    close(null_fd);

    for (int i = 0; i < n_writers; i++) {
        Pthread_join(th_writers[i], nullptr);
    }
}
