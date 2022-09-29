#ifndef HAVE_LINUX_PERF_EVENT_H

static inline void perfstats_init(void) {}
static inline void perfstats_deinit(void) {}
static inline void perfstats_enable(void) {}
static inline void perfstats_disable(void) {}
static void perfstats_print_header(char *, char *){};
static void perfstats_print(char *preamble, char *filename, char *epilogue){};

#else
void restore_cpufrequnecy();
void change_cpufrequnecy(int MHz);
void flush_caches(void);
void perfstats_init(void);
void perfstats_deinit(void);
void perfstats_enable(void);
void perfstats_disable(void);
void perfstats_print_header(char *, char *);
void perfstats_print(char *preamble, char *filename, char *epilogue);


#endif
