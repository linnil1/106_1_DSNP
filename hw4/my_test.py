import subprocess
import numpy.random as random
import time

f = open("my_do", "w")
n = 0
an = 0

def myNew():
  global n, an
  q = random.random_integers(10)
  if random.random() > 0.2:
    an += q
    print('mtn {} -A {}'.format(q, random.random_integers(100)), file=f)
  else:
    n += q
    print('mtn {}'.format(q), file=f)

def myDel():
  global n, an
  r = random.random()
  q = random.random_integers(10)
  if r > 0.3:
    print('mtd -I {} -A'.format(random.random_integers(an)), file=f)
    an -= q
  elif r < 0.1:
    print('mtn -I {}'.format(random.random_integers(q)), file=f)
    n -= q
  else:
    if random.random() > 0.5:
      q = random.random_integers(int( n/50)+1)
      n -= q
      print('mtd -R {}'   .format(q), file=f)
    else:
      q = random.random_integers(int(an/50)+1)
      an -= q
      print('mtd -R {} -A'.format(q), file=f)


def myop():
  for i in range(5):
    myNew()
  myDel()
  print("mtp", file=f)

def myops():
  global n, an
  n = 0
  an = 0
  for i in range(100):
    myop()
  print("mtr " + str(random.random_integers(1000,10000)), file=f)

for i in range(100):
  myops()

print("Q -F", file=f)
f.close()

p = subprocess.Popen(["./memTest", "-f", "my_do"], stdout=open("my_ans_test","w"), stderr=subprocess.STDOUT)
p.wait()
p = subprocess.Popen(["ref/memTest-linux", "-f", "my_do"], stdout=open("my_ans","w"), stderr=subprocess.STDOUT)
p.wait()

with open("my_ans_test") as f1:
  with open("my_ans") as f2:
    f1 = f1.read()
    f2 = f2.read()
    if f1 == f2:
      print("OK")
    else:
      print("vimdiff my_ans_test my_ans")
