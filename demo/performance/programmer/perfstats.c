#ifdef HAVE_LINUX_PERF_EVENT_H
#include "perfstats.h"

#include <linux/perf_event.h>
#include <linux/unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <string.h>
#define CACHE_CONTROL_FLUSH_CACHES _IO('q', 1)

struct timeval time_start, time_end;
double elapsed_time;

enum {
    COUNTER_PAGE_FAULTS,
    COUNTER_CPU_CYCLES,
    STALLED_CYCLES_FRONTEND,
    STALLED_CYCLES_BACKEND,
    INSTRUCTIONS,    
    DTLB_ACCESSES,
    DTLB_MISSES,
    BRANCHES,
    BRANCH_MISSES,
    CONTEXT_SWITCHES,
};

static struct perf_event_attr attrs[] = {
    { .type = PERF_TYPE_SOFTWARE, .config = PERF_COUNT_SW_PAGE_FAULTS},
    { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_CPU_CYCLES},
    { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_STALLED_CYCLES_FRONTEND},
    { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_STALLED_CYCLES_BACKEND},
    { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_INSTRUCTIONS},
    { .type = PERF_TYPE_HW_CACHE, .config = PERF_COUNT_HW_CACHE_DTLB | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16) },
    { .type = PERF_TYPE_HW_CACHE, .config = PERF_COUNT_HW_CACHE_DTLB | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16) },
    { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS},
    { .type = PERF_TYPE_HARDWARE, .config = PERF_COUNT_HW_BRANCH_MISSES},
    { .type = PERF_TYPE_SOFTWARE, .config = PERF_COUNT_SW_CONTEXT_SWITCHES},
    };

#define STAT_COUNT (sizeof(attrs) / sizeof(*attrs))
double total_time;
static int fds[STAT_COUNT];
int cpu=0;

static inline int
sys_perf_event_open(struct perf_event_attr *attr,
                    pid_t pid, int cpu, int group_fd,
                    unsigned long flags)
{
    attr->size = sizeof(*attr);
    return syscall(__NR_perf_event_open, attr, pid, cpu,
                   group_fd, flags);
}

void restore_cpufrequnecy() {
    char cmdline[1024];
    sprintf(cmdline,"/usr/sbin/changefreq %d 1550000 3200000",cpu);
    system(cmdline);
}
void change_cpufrequnecy(int MHz) {
    int KHz = MHz*1000;
    char cmdline[1024];
    cpu = sched_getcpu();
    sprintf(cmdline,"/usr/sbin/changefreq %d %d %d",cpu,KHz,KHz);
    system(cmdline);
    return;
}

void flush_caches() {

        if( access( "/dev/cache_control", R_OK|W_OK ) != 0) {
//                fprintf(stderr, "Couldn't open '/dev/cache_control'.  Not flushing caches.\n");
                return;
        }

        int fd = open("/dev/cache_control", O_RDWR);
        if (fd == -1) {
//                fprintf(stderr, "Couldn't open '/dev/cache_control' to flush caches: \n");
                return;
        }

        int r = ioctl(fd, CACHE_CONTROL_FLUSH_CACHES);
        if (r == -1) {
//                fprintf(stderr, "Flushing caches failed: ");
                exit(1);
        }
}
void perfstats_init(void)
{
    int pid = getpid();
    int i;

    for (i = 0; i < STAT_COUNT; i++) {
        attrs[i].inherit = 1;
        attrs[i].disabled = 1;
        attrs[i].exclude_kernel = 0;
        attrs[i].enable_on_exec = 0;
        fds[i] = sys_perf_event_open(&attrs[i], pid, -1, -1, 0);
//        fprintf(stderr,"PC: %d %d %X\n",i, fds[i], attrs[i].config);
    }
}

void perfstats_deinit(void)
{
    int i;
    for (i = 0; i < STAT_COUNT; i++) {
        close(fds[i]);
        fds[i] = -1;
    }
}


void perfstats_enable(void)
{
    int i;
    for (i = 0; i < STAT_COUNT; i++) {
        if (fds[i] <= 0)
            continue;

        ioctl(fds[i], PERF_EVENT_IOC_ENABLE);
    }
    gettimeofday(&time_start, NULL);
}

void perfstats_disable(void)
{
    int i;
    gettimeofday(&time_end, NULL);
    for (i = 0; i < STAT_COUNT; i++) {
        if (fds[i] <= 0)
            continue;

        ioctl(fds[i], PERF_EVENT_IOC_DISABLE);
    }
    gettimeofday(&time_end, NULL);
    elapsed_time = ((time_end.tv_sec * 1000000.0 + time_end.tv_usec) - (time_start.tv_sec * 1000000.0 + time_start.tv_usec));
    elapsed_time/=1000000.0;
}

static uint64_t readcounter(int i)
{
    uint64_t ret;
    read(fds[i], &ret, sizeof(ret));

    return ret;
}

void perfstats_print_header(char *filename, char *header)
{
    FILE *fout;
    fout = fopen(filename, "w");
    fprintf(fout, "%s\n", header);
    fclose(fout);
}

void perfstats_print(char *preamble, char *filename, char *epilogue)
{
    FILE *fout;
    uint64_t ic, cycles;
    fout = fopen(filename, "a");
    ic = readcounter(INSTRUCTIONS);
    cycles = readcounter(COUNTER_CPU_CYCLES);
    fprintf(fout, "%s %lu,%lu,%lf,%lf,%lf,%lf%s", 
                     preamble, 
                     ic, 
                     cycles,
                     (double)cycles/ic,
                     cycles/((double)elapsed_time*1000000.0), ((double)elapsed_time*1000000000.0)/cycles,
                     elapsed_time, epilogue);
    fclose(fout);
}
#endif
