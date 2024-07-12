#include "kernel/types.h"
#include "user/user.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "kernel/fcntl.h"
#include "kernel/crypto.h"

int main(void) {
  // Ensure this process is started by the kernel with PID 2
  if(getpid() != 2) {
    printf("Process not started by kernel with PID 2, exiting.\n");
    exit(1);
  }

  // Open the console for logging
  if(open("console", O_RDWR) < 0) {
    printf("Opening console failed, creating new console.\n");
    mknod("console", CONSOLE, 0);
    if(open("console", O_RDWR) < 0) {
      printf("Failed to open console after creating.\n");
      exit(1);
    }
  }
  dup(0);  // stdout
  dup(0);  // stderr

  printf("crypto_srv: starting\n");

  void* addr;
  uint64 size;

  while(1) {
    // Take a shared memory request
    if(take_shared_memory_request(&addr, &size) < 0) {
      printf("No shared memory request available, continuing.\n");
      continue;
    }

    printf("Shared memory request taken: addr=%p, size=%lu\n", addr, size);

    struct crypto_op* op = (struct crypto_op*) addr;

    // Validate the request
    if(op->state != CRYPTO_OP_STATE_INIT ||
       (op->type != CRYPTO_OP_TYPE_ENCRYPT && op->type != CRYPTO_OP_TYPE_DECRYPT)) {
      printf("Invalid crypto operation request: state=%d, type=%d\n", op->state, op->type);
      op->state = CRYPTO_OP_STATE_ERROR;
      remove_shared_memory_request(addr, size);
      continue;
    }

    // Ensure sizes are within reasonable limits
    if(op->key_size == 0 || op->data_size == 0 || op->key_size > size || op->data_size > size) {
      printf("Invalid sizes: key_size=%lu, data_size=%lu, memory size=%lu\n", op->key_size, op->data_size, size);
      op->state = CRYPTO_OP_STATE_ERROR;
      remove_shared_memory_request(addr, size);
      continue;
    }

    // Perform the cryptographic operation (XOR encryption/decryption)
    printf("Performing cryptographic operation: type=%d, key_size=%lu, data_size=%lu\n", op->type, op->key_size, op->data_size);
    for(uint64 i = 0; i < op->data_size; i++) {
      op->payload[op->key_size + i] ^= op->payload[i % op->key_size];
    }

    // Memory barrier to ensure the data write completes before setting the state
    asm volatile ("fence rw,rw" : : : "memory");

    // Mark the request as done
    op->state = CRYPTO_OP_STATE_DONE;
    printf("Cryptographic operation completed: state=%d\n", op->state);

    // Remove the shared memory mapping
    remove_shared_memory_request(addr, size);
    printf("Shared memory request removed: addr=%p, size=%lu\n", addr, size);
  }

  exit(0);
}
