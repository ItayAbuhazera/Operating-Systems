struct shmem_request {
  int src_pid;
  int dst_pid;
  
  uint64 src_va;
  uint64 size;
};