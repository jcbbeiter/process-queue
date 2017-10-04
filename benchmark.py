#!/usr/bin/env python2.7

import os
import sys
import subprocess
import stat
import time

def run_workload(policy, filename, cores):
    print "  Benchmarking " + policy.upper() + ", " + str(cores) + " cores, workload '" + filename[18:-3] + "'"
    # start server
    bit_bucket = open(os.devnull,'w')
    args = ["./pq","-p",policy, "-n", str(cores)]
    server = subprocess.Popen(args,stdout=bit_bucket)

    # make a temp copy of script, without watch command
    temp_file = "./.benchmark_temp_script"
    with open("./workloads/" + filename, 'r') as r:
       with open(temp_file, 'w') as w: 
            for line in r:
                if not 'watch' in line:
                    w.write(line)

    # make temp job executable and run it
    st = os.stat(temp_file)
    os.chmod(temp_file, st.st_mode | stat.S_IEXEC)
    print "    adding jobs..."
    subprocess.call(temp_file)

    # remove temp job file
    os.remove(temp_file)

    # repeatedly run ./pq status until no more processes are running
    print_counter = 0 
    done = False
    while not done:
        status = subprocess.Popen(["./pq","status"],stdout=subprocess.PIPE)
        out, err = status.communicate()
        lines = out.split('\n')
        header = lines[0].split()
        if header[2] == '0,' and header[5] == '0,':
           turnaround_time = float(header[11][:-1]) # trim comma from end of this field
           response_time = float(header[14])
           done = True
        elif print_counter % 20 == 0:
            print "    jobs running..."
            print_counter = 0
        time.sleep(1)
        print_counter = print_counter + 1

    # stop server
    server.send_signal(2)

    print "    complete: turnaround = " + str(turnaround_time) + ", response = " + str(response_time)

    return [turnaround_time,response_time]

if len(sys.argv) != 2:
    print "usage: ./benchmark.py [Number of cores]"
    exit(0)

cores = int(sys.argv[1])

if not (cores > 0 and cores < 32):
    print "Error: must specify an amount of cores between 1 and 31 (inclusive)"
    exit(1)

if not os.path.exists("./benchmarks/"):
    os.makedirs("./benchmarks")
    print "making benchmarks directory"

if os.path.exists("/tmp/pq.socket_jbeiter"):
    os.remove("/tmp/pq.socket_jbeiter")
    print "removing old socket before running..."

print "Cleaning and building before benchmark..."
subprocess.call(["make","clean"])
subprocess.call("make")

print "Running benchmarks with " + str(cores) + " cores..."
for policy in {"fifo", "rdrn", "mlfq"}:
    out_filename = "./benchmarks/benchmark-" + policy.upper() + "-" + str(cores)
    with open(out_filename, 'w') as out_file:
        print "Starting benchmark for policy: " + policy.upper()
        out_file.write("Benchmark policy: " + policy.upper() + ", " + str(cores) + " cores\n")
        out_file.write("WORKLOAD".center(15) + " | ")
        out_file.write("TURNAROUND".center(10) + " | ")
        out_file.write("RESPONSE\n")
        out_file.write("----------------x------------x---------\n")
        for filename in os.listdir("./workloads"):
            [turnaround, response] = run_workload(policy, filename, cores)
            out_file.write(filename[18:-3].rjust(15) + " | ")
            out_file.write(str(turnaround).center(11) + "| ")
            out_file.write(str(response) + "\n")
print "Benchmark complete!"
