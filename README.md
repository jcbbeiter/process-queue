CSE.30341.FA17: Project 02
==========================

This is the documentation for [Project 02] of [CSE.30341.FA17].

Members
-------

1. Jacob Beiter (jbeiter@nd.edu)

Design
------

> 1. You will need at least two types of structures:
>
>   - **Process**: This should keep track of each job along with any accounting
>     information such as **command** and **PID**.
>
>   - **Scheduler**: This should keep track of the jobs in running and waiting
>     queues along with other information such as **policy**, **number of
>     CPUs**, and **time slice duration**.
>
>   With these in mind, consider the following questions:
>
>   - What sort of **accounting** with you need to keep track of for each
>     **Process**?
>
>   - How will you compute the turnaround and response times for each
>     **Process**?
>
>   - What information do you need to store in the **Scheduler**?  How will it
>     maintain a running and waiting queue?
>
>   - How will you compute the average turnaround and response times for the
>     whole process queue?

For each process, we will need to keep track of: PID, command, arrival time, start time(first scheduling), state (running/sleeping), how much CPU it has used, and the usage and size of the current time slice (for MLFQ).
Turnaround time for each process is the time between its arrival and its completion. Response time is the the time between its arrival and its first scheduling.
The scheduler needs to hold a queue of waiting processes, and the group of processes that are running. It also needs to store information relevant to the type of scheduling. FIFO has nothing extra, for round robin it needs to store the interrupt time, and for MLFQ it needs to have the different process queues, which processes are where, and the parameters of each (e.g. time slice size).
Average turnaround and respones times for the whole process queue can be computed by simply taking the average of the individual values over the whole queue. A process may not be included in the computation if it hasn't completed (for turnaround time), or been scheduled (response time).

> 2. Debugging this project will be difficult without good logging.  Because
>    timing is an important component of scheduling, it will be useful to have
>    a consistent logging mechanism that includes timestamps.
>
>   - How will you go about logging information in your program?
>
>   - What sort of information will you log?

We can implement logging by simply printing to stdout in the server process when things happen. Each print will include a timestamp, and we will log any important events to the running of the program: programs received, starting execution, programs finishing, etc. Erring on the side of more logging will make it easier to understand what is happening during execution.

> 3. Jobs in the process queue eventually become processes which need to be
>    created, preempted, resumed, and terminated.
>
>   - How will you create a process and execute its command?
>
>       Note: you will not want to use `/bin/sh -c command` in this assignment
>       since that creates two processes and you would only have direct control
>       of `/bin/sh` rather than `command`
>
>   - How will you implement preemption?  That is, how will you **stop** or
>     **pause** a running process?
>
>   - How will you **resume** a process that has been preempted?
>
>   - How will you **terminate** an active process?
>
>   - How will you gather statistics or accounting information about each
>     process?  What will you store?
>
>       [Hint](https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat)

Create a new process using fork(), and use one of the exec() family of commands to run the command.
Preemption can be implemented by sending the process the SIGSTOP signal to pause its execution
To resume a preempted process, send it the SIGCONT signal.
To terminate an active process, send it the SIGKILL signal.
We can gather information about each running program by looking at the directory /proc/$PID. The file stat has a lot of information about its running, including CPU time spent in user code. We will store the CPU time in user code (utime) to use for the scheduling.

> 4. The scheduler will need to activated under two conditions: when a process
>    dies and after some period of time (ie. time slice has expired).
>
>   - How will you trigger your scheduler when a process dies?  What must
>     happen when a process dies (consider both the **Scheduler** and the
>     **Process**)?
>
>   - How will you ensure your scheduler runs periodically after a time slice
>     has expired?
>
>       Note: you may wish to consider how your response to question 6 can help
>       with this.

Trigger the scheduler when a process dies by setting a signal handler for the SIGCHILD signal. When a process dies the scheduler must calculate its turnaround and response times and remember them for accounting. The child process must also be reaped, which is done by calling wait() in the parent.

> 5. The client and server need to communicate via a request and response
>    model.
>
>   - Which IPC mechanism will you use: named pipes or unix domain sockets?
>
>   - How will you utilize this IPC mechanism?
>
>       Note: you may wish to consider this response in light of your answer in
>       question 6.

I will use a unix domain socket to communicate between server and client so that the communication can be two-way. The server will periodically to see if there is input to be read from the socket, and if so will read it. It will send the response across the same socket, and the client can read it and echo it to stdout.

> 6. The server will need to perform its scheduling duties and process requests
>    from clients.
>
>   - How will you multiplex I/O and computation?
>
>   - How will you ensure that your I/O will not block indefinitely?
>
>   - How will you allow events such as a child process dying interrupt your
>     I/O, but block such an event from interrupting your normal scheduling
>     functions?  Why would this be necessary?

Can multiplex I/O one of two ways: event-based concurrency, where it all occurs in one thread, or by having two threads: one for I/O and one for computation. Need to discuss with TA about timing.
To prevent I/O from blocking, the server will use the poll() system call to check if there is input to read before reading it. If there isn't any, it won't do any I/O.
Can prevent events from interrupting scheduling by using sigprocmask to temporarily block the signals while doing scheduling, and then unblock it when the scheduling is done. This would be necessary because interacting with the processes and process queues is an important part of the code, and leaving it in a half-done state might cause undesirable behavior.

> 7. Although FIFO is straightforward, both Round Robin and Multi-Level
>    Feedback Queue require preemption and some extract accounting.
>
>   - How will you perform preemption?  What happens to a process when it is
>     prempted?
>
>   - How will MLFQ determine if a process needs to be lowered in priority?
>     What information must be tracked and how it be updated?
>
>   - How will MFLQ determine if a priority boost is required?

Preemption can be performed by sending a running process the SIGSTOP signal and a different one the SIGCONT signal to start it. When a process is preempted it is paused, and its state is saved to be started up again later.
MLFQ will determine that a process needs to be lowered in priority if it has used up its allotment of CPU time at the current time slice. It has to track the CPU usage since entering the current level of the queue, which will be updated by checking the utime value in /proc/$PID/stat.
MLFQ will simply do a priority boost at regular intervals.


Question for TA:
In the MLFQ, if a program that is running blocks for I/O and relinquishes the CPU, another program should run while it's blocking. How should we accomplish this? Should the scheduler run more often than once each time slice, and check if the running program is blocking? Is there a way to do that? Some advice would be appreciated

Demonstration
-------------

> Place a link to your demonstration slides on [Google Drive].

Errata
------

> Describe any known errors, bugs, or deviations from the requirements.

Extra Credit
------------

> Describe what extra credit (if any) that you implemented.




[Project 02]:       https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/project02.html
[CSE.30341.FA17]:   https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/
[Google Drive]:     https://drive.google.com
