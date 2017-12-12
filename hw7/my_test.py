import subprocess
import os
import numpy.random as random
import time

f = open("my_ans_do", "w")
n = 0

def show():
  print("taskquery -heap", file=f)
  print("taskquery -hash", file=f)
  print("taskquery -min", file=f)

def myop():
  global n
  # new
  q = random.random_integers(100)
  n += q
  print("tasknew -r "+ str(q), file=f)
  show()

  # assign
  q = random.random_integers(100)
  print("taskassign -r "+ str(q), file=f)
  show()

  # remove
  q = random.random_integers(100)
  n -= q
  print("taskremove -r "+ str(q), file=f)
  show()

def myops():
  global n
  n = 0
  for j in range(50):
    print("taskinit " + str(random.random_integers(1000, 1000000)), file=f)
    for i in range(50):
      myop()

# main
myops()
print("Q -F", file=f)
f.close()

p0 = subprocess.Popen(["./taskMgr" , "-f", "my_ans_do"], stdout=open("my_ans_test","w"), stderr=subprocess.STDOUT)
p1 = subprocess.Popen(["ref/taskMgr-ref", "-f", "my_ans_do"], stdout=open("my_ans","w"), stderr=subprocess.STDOUT)
p0.wait()
p1.wait()

with open("my_ans_test") as f1:
  with open("my_ans") as f2:
    f1 = f1.read()
    f2 = f2.read()
    if f1 == f2:
      print("OK")
    else:
      print("vimdiff my_ans_test my_ans")
