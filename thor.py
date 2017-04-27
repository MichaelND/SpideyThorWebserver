#!/usr/bin/env python2.7

import multiprocessing
import os
import requests
import sys
import time

# Globals

PROCESSES = 1
REQUESTS  = 1
VERBOSE   = False
URL       = None

# Functions

def usage(status=0):
    print '''Usage: {} [-p PROCESSES -r REQUESTS -v] URL
    -h              Display help message
    -v              Display verbose output

    -p  PROCESSES   Number of processes to utilize (1)
    -r  REQUESTS    Number of requests per process (1)
    '''.format(os.path.basename(sys.argv[0]))
    sys.exit(status)

def do_request(pid):
    totaltime = 0
    timeTaken = 0
    for i in range(0, int(REQUESTS)):
        t=0
        t = time.time()
        requests.get(URL)
        timeTaken = time.time()-t
        totaltime = totaltime + timeTaken
        print ("Process: " + str(pid - 1) + ", Request: " + str(i) + ", Elapsed Time: " + str(round(timeTaken, 2)))


    print ("Process: " + str(pid - 1) + ", AVERAGE   , Elapsed Time: " + str(round((totaltime / int(REQUESTS)), 2)))

    return (totaltime / int(REQUESTS))

# Main execution

if __name__ == '__main__':
    # Parse command line arguments
    args = sys.argv[1:]
    while len(args) and args[0].startswith('-') and len(args[0]) > 1:
        arg = args.pop(0)
        if arg == '-h':
            usage(0)
        elif arg == '-v':
            VERBOSE = True
        elif arg == '-p':
            PROCESSES = args.pop(0)
        elif arg == '-r':
            REQUESTS = args.pop(0)
        else:
            usage(1)

    URL = args.pop(0)

    # Create pool of workers and perform requests

    totalavg = 0
    if PROCESSES == 1:
        totalavg = do_request(1)
        print ("TOTAL AVERAGE ELAPSED TIME: " + str(round(totalavg, 2)))

    else:
        pool = multiprocessing.Pool(int(PROCESSES))
        results = pool.imap(do_request, range(1, int(PROCESSES) + 1))

        for i in results:
            totalavg = totalavg + i

        print ("TOTAL AVERAGE ELAPSED TIME: " + str(round((totalavg / int(PROCESSES)),2)))

# vim: set sts=4 sw=4 ts=8 expandtab ft=python:
