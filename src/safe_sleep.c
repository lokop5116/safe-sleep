asm(".section .text\n"
    ".weak _start\n"
    ".global _start\n"
    "_start:\n"
    "pop %rdi\n"                // argc   (first arg, %rdi)
    "mov %rsp, %rsi\n"          // argv[] (second arg, %rsi)
    "lea 8(%rsi,%rdi,8),%rdx\n" // then a NULL then envp (third arg, %rdx)
    "xor %ebp, %ebp\n"          // zero the stack frame
    "and $-16, %rsp\n" // x86 ABI : esp must be 16-byte aligned before call
    "call main\n"      // main() returns the status code, we'll exit with it.
    "mov %eax, %edi\n" // retrieve exit code (32 bit)
    "mov $60, %eax\n"  // NR_exit == 60
    "syscall\n"        // really exit
    "hlt\n"            // ensure it does not return
    "");

#define SYSCALL_WRITE 1
#define SYSCALL_NANOSLEEP 35

#define syscall_1(num, arg1)                                                   \
  ({                                                                           \
    long _ret;                                                                 \
    register long _num asm("rax") = (num);                                     \
    register long _arg1 asm("rdi") = (long)(arg1);                             \
                                                                               \
    asm volatile("syscall\n"                                                   \
                 : "=a"(_ret)                                                  \
                 : "r"(_arg1), "0"(_num)                                       \
                 : "rcx", "r11", "memory", "cc");                              \
    _ret;                                                                      \
  })

#define syscall_2(num, arg1, arg2)                                             \
  ({                                                                           \
    long _ret;                                                                 \
    register long _num asm("rax") = (num);                                     \
    register long _arg1 asm("rdi") = (long)(arg1);                             \
    register long _arg2 asm("rsi") = (long)(arg2);                             \
                                                                               \
    asm volatile("syscall\n"                                                   \
                 : "=a"(_ret)                                                  \
                 : "r"(_arg1), "r"(_arg2), "0"(_num)                           \
                 : "rcx", "r11", "memory", "cc");                              \
    _ret;                                                                      \
  })

#define syscall_3(num, arg1, arg2, arg3)                                       \
  ({                                                                           \
    long _ret;                                                                 \
    register long _num asm("rax") = (num);                                     \
    register long _arg1 asm("rdi") = (long)(arg1);                             \
    register long _arg2 asm("rsi") = (long)(arg2);                             \
    register long _arg3 asm("rdx") = (long)(arg3);                             \
                                                                               \
    asm volatile("syscall\n"                                                   \
                 : "=a"(_ret)                                                  \
                 : "r"(_arg1), "r"(_arg2), "r"(_arg3), "0"(_num)               \
                 : "rcx", "r11", "memory", "cc");                              \
    _ret;                                                                      \
  })

int write(int fd, char *buf, int n) {
  return syscall_3(SYSCALL_WRITE, fd, buf, n);
}

struct timespec {

  long tv_sec;
  long tv_nsec;
};

int sleep(long int duration) {

  struct timespec req, rem;

  req.tv_sec = duration;
  req.tv_nsec = 0;

  rem.tv_nsec = 0, rem.tv_sec = 0;

  int ret = syscall_2(SYSCALL_NANOSLEEP, &req, &rem);

  return ret;
}

int stringLen(char *string) {

  int n = 0;

  while (*string != '\0') {
    string++;
    n++;
  }

  return n;
}

void printString(char *string) {

  int n = stringLen(string);

  write(1, string, n);
}

long int parseInt(char *num) {

  long int duration = 0;

  for (int i = 0; num[i] != '\0'; i++) {

    if (num[i] >= '0' && num[i] <= '9') {
      duration = (duration * 10) + (num[i] - '0');
    } else {
      printString("Enter a valid duration\n");
      return -1;
    }
  }
  return duration;
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printString("Usage: sleep [time]\n");
    return 1;
  }

  long int duration = parseInt(argv[1]);

  if (duration == -1) {

    return 0;
  }

  printString("Sleeping for ");
  printString(argv[1]);
  printString(" seconds...\n");

  sleep(duration);

  return 0;
}
