import subprocess
import numpy.random as random
import time

f = open("my_ans_do", "w")
dtype = "adtTest.dlist"
# array bst dlist
n = 0

def myNew():
  global n
  q = random.random_integers(100)
  n += q
  print('adta -R {}'.format(q), file=f)

def myDel():
  global n
  if n > 10:
    q = random.random_integers(10)
  else:
    q = random.random_integers(n)
  r = random.random()
  n -= q
  if r > 0.5:
    print('adtd -R {}'.format(q), file=f)
  elif 0.5 >= r > 0.25:
    print('adtd -F {}'.format(q), file=f)
  elif 0.25 >= r > 0.01:
    print('adtd -B {}'.format(q), file=f)
  else:
    print('adtd -All', file=f)
    n = 0

def myop():
  for i in range(3): # if dlist smaller to 4
    myNew()
  myDel()
  print("adtp", file=f)
  print("adtp -R", file=f)
  print("adts", file=f)
  print("adtp", file=f)
  print("adtp -R", file=f)

def myops():
  global n, an
  n = 0
  for i in range(10):
    myop()
  print("atdr {}" + str(random.random_integers(100) + 1), file=f)

for i in range(10):
  myops()

print("Q -F", file=f)
f.close()

p = subprocess.Popen(["./" + dtype, "-f", "my_ans_do"], stdout=open("my_ans_test","w"), stderr=subprocess.STDOUT)
p.wait()
p = subprocess.Popen(["ref/" + dtype, "-f", "my_ans_do"], stdout=open("my_ans","w"), stderr=subprocess.STDOUT)
p.wait()

with open("my_ans_test") as f1:
  with open("my_ans") as f2:
    f1 = f1.read()
    f2 = f2.read()
    if f1 == f2:
      print("OK")
    else:
      print("vimdiff my_ans_test my_ans")
