#!/usr/bin/env python2.7

import os
import sys
import subprocess

def getTime(str):
  data = []
  data = str.split()
  time = data[0]
  return time[:-4]

def getSpace(str):
  data = []
  data = str.split()
  space = data[2]
  return space[:-4]

ALPHAS = [0.5, 0.75, 0.9, 1.0, 2.0, 4.0, 8.0, 16.0]
NITEMS = [1,10,100,1000,10000,100000,1000000,10000000]
time = []
space = []
alpha = []
items = []

for i in NITEMS:
  for lf in ALPHAS:
    command = "shuf -i1-%d -n %d | ./measure ./freq -l %d > /dev/null" % (i,i,lf)
    ps = subprocess.Popen(command,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    output = ps.communicate()[0]
    time.append(getTime(output))
    alpha.append(str(lf))
    space.append(getSpace(output))
    items.append(str(i))

print '-----------------------------------------------------'
print '| NITEMS     | SORT       | TIME       | SPACE      |'
print '|------------|------------|------------|------------|'
data = zip(items,alpha,time,space)
for rows in data:
  for row in rows:
    print '| '+row.ljust(10),
  print '|'

print '-----------------------------------------------------'
