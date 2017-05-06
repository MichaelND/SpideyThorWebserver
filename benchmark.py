#!/usr/bin/env python2.7

import os
import sys
from subprocess import Popen, PIPE

PROCESSES = 10
REQUESTS = 100

TIMES = []
THROUGHPUT = 0
FILES = [8000, 800000, 8000000000] #of bits for 1KB, 1MB, and 1GB

def usage(exit_code=0):
    print '''Usage: {} [-t THROUGHPUT] [link to server:PORT]
    -t flag for throughput mode'''.format(os.path.basename(sys.argv[0]))
    sys.exit(exit_code)

#check invalid number arguments
if ((len(sys.argv) < 2) or (len(sys.argv) > 3)):
  usage(1)

#parse command line
args = sys.argv[1:]
while len(args):
  if (args[0].startswith('-') and len(args[0]) > 1):
    arg = args.pop(0)
    if arg == '-t':
      THROUGHPUT = 1
    elif arg == '-h':
      usage(0)
  elif (args[0].startswith('http')):
    link = args[0];
    arg = args.pop(0)

#based on output of command line, generate list of average latencies or throughputs of spidey using thor
i = 0
if THROUGHPUT == 0:
  LINKS = [ link, link + '/text/hackers.txt', link + '/scripts/cowsay.sh' ]
  for link in LINKS:
    print "Processing: " + link
    output = './thor.py -r {} -p {} {}'.format(REQUESTS, PROCESSES, link)
    o = Popen(output, stdout=PIPE, stderr=PIPE, shell=True)
    stdout,stderr = o.communicate()
    TIMES.append(stdout.split()[-1]) #this appends the last item of the list which is stdout to TIMES

  print "Average Latencies of Spidey"
  print "[Directory Listings, Static Files, CGI SCRIPTS]"
  print TIMES
else:
  LINKS = [ link + '/text/1K.txt', link + '/text/1M.txt', link + '/text/1G.txt' ]
  for link in LINKS:
    print "Processing: " + link
    output = './thor.py -r {} -p {} {}'.format(REQUESTS, PROCESSES, link)
    o = Popen(output, stdout=PIPE, stderr=PIPE, shell=True)
    stdout,stderr = o.communicate()
    lst = stdout.split()[-1]
    lst = float(lst)
    TIMES.append(lst / FILES[i]) #this appends the last item of the list which is stdout to TIMES
    i = i + 1

  print "Average Throughputs of Spidey"
  print "[1KB, 1MB, 1GB]"
  print TIMES




