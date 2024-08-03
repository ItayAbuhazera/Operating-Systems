#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#include "shmem_queue.h"

static struct shmem_queue {
  struct spinlock lock;

  struct shmem_request requests[NSHMEM_REQS];

  int read_idx;
  int write_idx;
} shmem_queue;

static void
shmem_request_init(struct shmem_request *req)
{
  req->src_pid  = -1;
  req->dst_pid  = -1;
  req->src_va   = 0;
  req->size     = 0;
}

void
shmem_queue_init(void)
{
  initlock(&shmem_queue.lock, "shmem_queue");
  
  shmem_queue.read_idx = -1;
  shmem_queue.write_idx = 0;

  for (int i = 0; i < NSHMEM_REQS; i++) {
    shmem_request_init(&shmem_queue.requests[i]);
  }
}

void
shmem_queue_insert(int src_pid, int dst_pid, uint64 src_va, uint64 size)
{
  acquire(&shmem_queue.lock);

  while (shmem_queue.write_idx == shmem_queue.read_idx - 1)
      sleep(&shmem_queue, &shmem_queue.lock);

  const uint idx = shmem_queue.write_idx;
  shmem_queue.requests[idx].src_pid = src_pid;
  shmem_queue.requests[idx].dst_pid = dst_pid;
  shmem_queue.requests[idx].src_va = src_va;
  shmem_queue.requests[idx].size = size;

  shmem_queue.write_idx = (shmem_queue.write_idx + 1) % NSHMEM_REQS;

  if (shmem_queue.read_idx == -1)
      shmem_queue.read_idx = 0;

  wakeup(&shmem_queue);

  release(&shmem_queue.lock);
}

struct shmem_request
shmem_queue_remove(void)
{
  acquire(&shmem_queue.lock);

  while (shmem_queue.read_idx == -1 || shmem_queue.read_idx == shmem_queue.write_idx)
      sleep(&shmem_queue, &shmem_queue.lock);

  const uint idx = shmem_queue.read_idx;
  const struct shmem_request req = shmem_queue.requests[idx];
  shmem_request_init(&shmem_queue.requests[idx]);

  shmem_queue.read_idx = (shmem_queue.read_idx + 1) % NSHMEM_REQS;

  wakeup(&shmem_queue);

  release(&shmem_queue.lock);

  return req;
}