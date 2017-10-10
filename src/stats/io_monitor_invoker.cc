#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stdio.h>
#include "../common/context_singleton.hh"

#ifndef IO_STAT_SCRIPT
#define IO_STAT_SCRIPT "read_io_stats.sh"
#endif

void invoke_io_reporter() {
  if (GET_STR("addons.zk.enabled") != "true")
    return;

  pid_t pid = 0;

  pid = fork();

  // I am child
  if (pid == 0) {

    //prctl(PR_SET_PDEATHSIG, SIGKILL); # It does not work correctly
    execlp(IO_STAT_SCRIPT, IO_STAT_SCRIPT, NULL);

    perror("Error has happened ");
  } 
}
