/* Dumps the amount of messages sent from one process to another */

#include "inc.h"
#include <minix/timers.h>
#include "kernel/proc.h"

#define PROCESSES_COUNT NR_PROCS + NR_TASKS

void
messages_dmp(void)
{
   register struct proc *rp;
   static struct proc *oldrp = BEG_PROC_ADDR;
   int r;

   if ((r = sys_getproctab(proc)) != OK) {
      printf("IS: warning: couldn't get copy of process table: %d\n", r);
      return;
   }

   printf("\nMessages sent:\n---\n");
   printf("FROM\tTO\tCOUNT\n");
   for (rp = oldrp; rp < END_PROC_ADDR; rp++) {
      oldrp = BEG_PROC_ADDR;
      if (isemptyp(rp)) continue;
      for (int j = 0; j < PROCESSES_COUNT; j++)
         if (rp->p_messages_sent[j] != 0)
            printf("%d\t%d\t%llu\n", rp->p_nr, j, rp->p_messages_sent[j]);
   }
}