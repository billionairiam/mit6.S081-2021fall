#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
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

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  backtrace();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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
  int ticks0;
  uint64 addr;
  if(argint(0, &ticks0) < 0 ||
     argaddr(1, &addr) < 0){
    return -1;
  }

  acquire(&tickslock);
  myproc()->tick = ticks;
  myproc()->interval = ticks0;
  myproc()->handler = addr;
  release(&tickslock);
  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc* p = myproc();

  p->trapframe->a0 = p->sigframe->a0;
  p->trapframe->a1 = p->sigframe->a1;
  p->trapframe->a2 = p->sigframe->a2;
  p->trapframe->a3 = p->sigframe->a3;
  p->trapframe->a4 = p->sigframe->a4;
  p->trapframe->a5 = p->sigframe->a5;
  p->trapframe->a6 = p->sigframe->a6;
  p->trapframe->a7 = p->sigframe->a7;
  p->trapframe->epc = p->sigframe->epc;
  p->trapframe->gp = p->sigframe->gp;
  p->trapframe->kernel_hartid = p->sigframe->kernel_hartid;
  p->trapframe->kernel_satp = p->sigframe->kernel_satp;
  p->trapframe->kernel_trap = p->sigframe->kernel_trap;
  p->trapframe->ra = p->sigframe->ra;
  p->trapframe->s0 = p->sigframe->s0;
  p->trapframe->s10 = p->sigframe->s10;
  p->trapframe->s11 = p->sigframe->s11;
  p->trapframe->s1 = p->sigframe->s1;
  p->trapframe->s2 = p->sigframe->s2;
  p->trapframe->s3 = p->sigframe->s3;
  p->trapframe->s4 = p->sigframe->s4;
  p->trapframe->s5 = p->sigframe->s5;
  p->trapframe->s6 = p->sigframe->s6;
  p->trapframe->s7 = p->sigframe->s7;
  p->trapframe->s8 = p->sigframe->s8;
  p->trapframe->s9 = p->sigframe->s9;
  p->trapframe->sp = p->sigframe->sp;
  p->trapframe->t1 = p->sigframe->t1;
  p->trapframe->t2 = p->sigframe->t2;
  p->trapframe->t3 = p->sigframe->t3;
  p->trapframe->t4 = p->sigframe->t4;
  p->trapframe->t5 = p->sigframe->t5;
  p->trapframe->t6 = p->sigframe->t6;
  p->trapframe->tp = p->sigframe->tp;

  p->reentrant = 0;
  return 0;
}