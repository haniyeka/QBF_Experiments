/* 
 * File:   auxiliary.hh
 * Author: mikolas
 *
 * Created on October 12, 2011
 */

#ifndef AUXILIARY_HH
#define  AUXILIARY_HH

#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>
#include "unistd.h"

static inline double read_cpu_time()
{
  struct rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return (double) ru.ru_utime.tv_sec + (double) ru.ru_utime.tv_usec / 1000000;
}

// taken from
// https://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-run-time-in-c
//
unsigned long read_mem_usage()
{
  using std::ios_base;
  using std::ifstream;
  using std::string;
  
  double resident_set = 0.0;
  
  // 'file' stat seems to give the most reliable results
  //
  ifstream stat_stream("/proc/self/stat",ios_base::in);
  
  // dummy vars for leading entries in stat that we don't care about
  //
  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;
  
  // the two fields we want
  //
  unsigned long vsize;
  long rss;
  
  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
              >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
              >> utime >> stime >> cutime >> cstime >> priority >> nice
              >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest
  
  stat_stream.close();
  
  long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
  resident_set = rss * page_size_kb;
  return (unsigned long)(resident_set);
}
#endif	/* AUXILIARY_HH */

