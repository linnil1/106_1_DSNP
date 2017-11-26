import subprocess
import os
import numpy.random as random
import time

f = open("my_ans_do", "w")

def myop(n):
  for i in range(50):
    q = random.random_integers(n)
    print("cirg "+ str(q), file=f)
    for j in range(5):
      print("cirg "+ str(q) + " -FanI " + str(random.random_integers(n)), file=f)
    print("cirg "+ str(q) + " -FanI 10000000", file=f)
    for j in range(5):
      print("cirg "+ str(q) + " -FanO " + str(random.random_integers(n)), file=f)
    print("cirg "+ str(q) + " -FanO 10000000", file=f)

def myops(fname):
  print("cirr -r {}".format(fname),  file=f)
  with open(fname) as fread:
    dat = fread.readline().split()
    myop(int(dat[1]) + int(dat[4]))

  print("cirw", file=f)
  print("cirp", file=f)
  print("cirp -net", file=f)
  print("cirp -PI", file=f)
  print("cirp -PO", file=f)
  print("cirp -fl", file=f)

# main
dir_name = "tests.fraig/"
files = [ dir_name + name for name in os.listdir(dir_name) if name.endswith(".aag") and name != "opt04.aag"]
random.shuffle(files)
for i in range(min(10, len(files))):
  myops(files[i])
print("Q -F", file=f)
f.close()

p0 = subprocess.Popen(["./cirTest" , "-f", "my_ans_do"], stdout=open("my_ans_test","w"), stderr=subprocess.STDOUT)
p1 = subprocess.Popen(["ref/cirTest-ref", "-f", "my_ans_do"], stdout=open("my_ans","w"), stderr=subprocess.STDOUT)
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
