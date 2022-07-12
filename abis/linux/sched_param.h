#ifndef __ABIS_LINUX_SCHED_PARAM_H
#define __ABIS_LINUX_SCHED_PARAM_H

/* Standard */
#define SCHED_OTHER 0
#define SCHED_BATCH 3
#define SCHED_IDLE 5

/* SCHED_ISO: reserves the number 4, but is not implemented */

/* Linux specific */
#define SCHED_FIFO 1
#define SCHED_RR 2
#define SCHED_DEADLINE 6
#define SCHED_RESET_ON_FORK 0x40000000

struct sched_param {
	int sched_priority;
};

#endif /* __ABIS_LINUX_SCHED_PARAM_H */
