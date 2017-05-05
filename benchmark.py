#!/usr/bin/env python2.7

import os
from subprocess import Popen, PIPE

numberItems = 1
ALPHA = [0.5, 0.75, 0.9, 1.0, 2.0, 4.0, 8.0, 16.0]

print '''| NITEMS    | ALHPA    | TIME     | SPACE     |
| --------- | -------- | -------- | --------- |'''
       
def display(x):
  output = 'shuf -i1-{} -n {} | ./measure ./freq -l {}'.format(numberItems, numberItems, x)
  o = Popen(output, stdout=PIPE, stderr=PIPE, shell=True)
  stdout,stderr = o.communicate()
  print '''| {}         | {}      | {} | {}  |'''.format(numberItems, x, stderr.split()[0], stderr.split()[2])
  
while numberItems <= 10000000:
  for x in ALPHA:
    display(x)

  numberItems = numberItems * 10
