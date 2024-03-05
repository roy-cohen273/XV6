#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  backtrace();

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sigalarm(void)
{
  int ticks;
  argint(0, &ticks);

  void (*handler)();
  argaddr(1, (uint64 *)&handler);

  struct proc *p = myproc();
  if (ticks == 0 && handler == 0) {
    p->alarm.ticks_left = 0;
  } else {
    p->alarm.handler = handler;
    p->alarm.ticks = ticks;
    p->alarm.ticks_left = ticks;
  }
  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();

  // reset ticks counter
  p->alarm.ticks_left = p->alarm.ticks;

  // restore the original trapframe
  memmove(p->trapframe, p->alarm.return_trapframe, sizeof(*p->alarm.return_trapframe));

  // the value we return here overrides p->trapframe->a0 by syscall()
  // we don't want this to happen, so we do this hack
  return p->trapframe->a0;
}
