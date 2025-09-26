#include "scheduler.h"
#include <stddef.h>
#include <string.h>

// ---- External symbols ----
// Context-switch stub implemented in assembly:
//   void ctx_switch(uint32_t **old_sp, uint32_t *new_sp);
extern void ctx_switch(uint32_t **old_sp, uint32_t *new_sp);

// ---- Config/state ----
static volatile uint64_t s_ticks = 0;
static uint32_t s_tick_hz = 100;             // PIT frequency
static uint32_t s_ms_per_tick = 10;          // derived
static uint32_t s_default_timeslice = 5;     // ticks

static Task s_tasks[MAX_TASKS];
static uint8_t s_stacks[MAX_TASKS][TASK_STACK_SIZE] __attribute__((aligned(16)));
static Task *s_current = NULL;
static uint32_t s_task_count = 0;
static uint32_t s_next_id = 1;

// A dummy stack pointer to hold the bootstrap context when we first switch
static uint32_t *s_bootstrap_sp = NULL;

// ---- Helpers ----
uint64_t Scheduler_Ticks(void) { return s_ticks; }
Task* Scheduler_Current(void) { return s_current; }

static int next_runnable_index(int start) {
    // Round-robin: scan for next RUNNABLE task
    for (int i = 1; i <= MAX_TASKS; ++i) {
        int idx = (start + i) % MAX_TASKS;
        if (s_tasks[idx].state == TASK_RUNNABLE) return idx;
    }
    return -1;
}

static void make_task_stack(Task *t, void (*entry)(void*), void *arg);
static void task_trampoline(struct Task *t);

// ---- Public ----
void Scheduler_Init(uint32_t tick_hz, uint32_t default_timeslice_ticks) {
    memset(s_tasks, 0, sizeof(s_tasks));
    s_ticks = 0;
    s_tick_hz = (tick_hz ? tick_hz : 100);
    s_ms_per_tick = 1000 / s_tick_hz;
    s_default_timeslice = (default_timeslice_ticks ? default_timeslice_ticks : 5);
}

Task* Scheduler_AddTask(void (*entry)(void*), void *arg, const char *name) {
    for (int i = 0; i < MAX_TASKS; ++i) {
        if (s_tasks[i].state == TASK_UNUSED) {
            Task *t = &s_tasks[i];
            memset(t, 0, sizeof(*t));
            t->id = s_next_id++;
            t->name = name;
            t->entry = entry;
            t->arg = arg;
            t->state = TASK_RUNNABLE;
            t->base_timeslice = s_default_timeslice;
            t->time_slice = t->base_timeslice;
            make_task_stack(t, entry, arg);
            ++s_task_count;
            return t;
        }
    }
    return NULL; // no slots
}

__attribute__((noreturn)) void Scheduler_Start(void) {
    // Pick first runnable
    int first = next_runnable_index(0);
    if (first < 0) {
        // No tasks; halt
        for (;;) { __asm__ __volatile__("hlt"); }
    }
    s_current = &s_tasks[first];

    // Switch from bootstrap (this CPU context) to first task
    ctx_switch(&s_bootstrap_sp, s_current->sp);

    // We never return here; tasks run from their trampoline
    __builtin_unreachable();
}

void Scheduler_Yield(void) {
    if (!s_current) return;
    s_current->time_slice = 0; // force resched
    // Trigger scheduler path (software yield)
    // We emulate an "ISR epilogue" style switch by calling the dispatcher directly.
    // Disable interrupts briefly to avoid racing with PIT ISR.
    __asm__ __volatile__("cli");
    // Find next
    int cur_idx = (int)(s_current - s_tasks);
    int next = next_runnable_index(cur_idx);
    if (next >= 0 && &s_tasks[next] != s_current) {
        Task *old = s_current;
        Task *newt = &s_tasks[next];
        s_current = newt;
        newt->time_slice = newt->base_timeslice;
        ctx_switch(&old->sp, newt->sp);
    }
    __asm__ __volatile__("sti");
}

void Scheduler_Sleep(uint32_t ms) {
    if (!s_current) return;
    uint64_t ticks = (ms + s_ms_per_tick - 1) / s_ms_per_tick; // ceil
    s_current->wake_tick = s_ticks + ticks;
    s_current->state = TASK_SLEEPING;
    Scheduler_Yield();
}

void Scheduler_Exit(void) {
    if (!s_current) for(;;){__asm__ __volatile__("hlt");}
    s_current->state = TASK_ZOMBIE;
    // Find next runnable; if none, halt
    int cur_idx = (int)(s_current - s_tasks);
    int next = next_runnable_index(cur_idx);
    if (next < 0) {
        for(;;){ __asm__ __volatile__("hlt"); }
    }
    Task *dead = s_current;
    Task *newt = &s_tasks[next];
    s_current = newt;
    newt->time_slice = newt->base_timeslice;
    ctx_switch(&dead->sp, newt->sp);
    __builtin_unreachable();
}

// Call this from your PIT ISR (IRQ0) once per tick.
// It MUST run with interrupts disabled already (inside ISR).
void Scheduler_OnTickISR(void) {
    ++s_ticks;

    // Wake sleepers
    for (int i = 0; i < MAX_TASKS; ++i) {
        if (s_tasks[i].state == TASK_SLEEPING && s_tasks[i].wake_tick <= s_ticks) {
            s_tasks[i].state = TASK_RUNNABLE;
            if (s_tasks[i].time_slice == 0) s_tasks[i].time_slice = s_tasks[i].base_timeslice;
        }
    }

    if (!s_current || s_current->state != TASK_RUNNABLE) {
        // Pick a runnable task if current isn't runnable
        int next = next_runnable_index(s_current ? (int)(s_current - s_tasks) : 0);
        if (next >= 0 && &s_tasks[next] != s_current) {
            Task *old = s_current;
            Task *newt = &s_tasks[next];
            s_current = newt;
            newt->time_slice = newt->base_timeslice;
            ctx_switch(old ? &old->sp : &s_bootstrap_sp, newt->sp);
        }
        return;
    }

    // Preemption: decrement slice; on zero, rotate
    if (s_current->time_slice > 0) {
        --s_current->time_slice;
    }

    if (s_current->time_slice == 0) {
        int cur_idx = (int)(s_current - s_tasks);
        int next = next_runnable_index(cur_idx);
        if (next >= 0 && &s_tasks[next] != s_current) {
            Task *old = s_current;
            Task *newt = &s_tasks[next];
            s_current = newt;
            newt->time_slice = newt->base_timeslice;
            ctx_switch(&old->sp, newt->sp);
        } else {
            // If no one else runnable, give current a fresh slice
            s_current->time_slice = s_current->base_timeslice;
        }
    }
}

// ---- Stack setup & trampoline ----

// Thunk that every new task starts in.
// It receives Task* as its first stack argument.
__attribute__((noreturn))
static void task_trampoline(struct Task *t) {
    // Enable interrupts for the task context
    __asm__ __volatile__("sti");
    t->entry(t->arg);
    // If task returns, cleanly exit
    Scheduler_Exit();
    __builtin_unreachable();
}

static void make_task_stack(Task *t, void (*entry)(void*), void *arg) {
    // Build a fresh stack so that when we switch to it and RET,
    // we jump into task_trampoline(t).
    uint8_t *stack_top = &s_stacks[t - s_tasks][TASK_STACK_SIZE];

    // Align down to 16
    uintptr_t sp_aligned = ((uintptr_t)stack_top) & ~(uintptr_t)0xF;

    uint32_t *sp = (uint32_t*)sp_aligned;

    // Push initial stack frame (cdecl):
    // [sp-4] = return address => task_trampoline
    // [sp-8] = argument to trampoline => Task*
    *(--sp) = (uint32_t)t;                               // arg to task_trampoline
    *(--sp) = (uint32_t)task_trampoline;                 // "return" address (RET will jump here)

    // When ctx_switch sets ESP=new_sp and executes RET,
    // CPU will pop EIP=task_trampoline, and the next pop (for its arg) is available as parameter.

    t->sp = sp;
}
