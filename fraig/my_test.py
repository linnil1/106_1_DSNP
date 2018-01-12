import subprocess
import os
import numpy.random as random
import time

f = open("my_ans_do", "w")

def myop(n):
  for i in range(50):
    q = random.random_integers(n)
    print("cirg "+ str(q), file=f)
    """
    for j in range(5):
      print("cirg "+ str(q) + " -FanI " + str(random.random_integers(50)), file=f)
    print("cirg "+ str(q) + " -FanI 10000000", file=f)
    print("cirw "+ str(q), file=f)
    for j in range(5):
      print("cirg "+ str(q) + " -FanO " + str(random.random_integers(50)), file=f)
    print("cirg "+ str(q) + " -FanO 10000000", file=f)
    """
def show(dat):
  myop(int(dat[1]) + int(dat[4]))
  """
  print("cirw", file=f)
  print("cirp -PI", file=f)
  print("cirp -PO", file=f)
  """
  print("cirp", file=f)
  print("cirp -net", file=f)
  print("cirp -fl", file=f)

def myops(fname):
  print("cirr -r {}".format(fname),  file=f)
  with open(fname) as fread:
    dat = fread.readline().split()
  show(dat)
  print("cirsweep", file=f)
  show(dat)
  print("ciropt", file=f)
  show(dat)
  print("cirstr", file=f)
  show(dat)
  print("ciropt", file=f)
  show(dat)
  print("cirstr", file=f)
  show(dat)
  print("cirsweep", file=f)

  show(dat)
  if "sim" in fname:
    print("usage", file=f)
    print("cirsim -f " + fname.replace("sim", "pattern.").replace(".aag",""), file=f)
    print("usage", file=f)
  show(dat)

# main
dir_name = "tests.fraig/"
files = [ dir_name + name for name in os.listdir(dir_name) if name.endswith(".aag") ]
random.shuffle(files)
for i in range(2):
#for i in range(min(10, len(files))):
  myops(files[i])
print("Q -F", file=f)
f.close()

p0 = subprocess.Popen(["./fraig" , "-f", "my_ans_do"], stdout=open("my_ans_test","w"), stderr=subprocess.STDOUT)
p0.wait()
p1 = subprocess.Popen(["ref/fraig", "-f", "my_ans_do"], stdout=open("my_ans","w"), stderr=subprocess.STDOUT)
p1.wait()

with open("my_ans_test") as f1:
  with open("my_ans") as f2:
    while True:
      l1 = f1.readline()
      l2 = f2.readline()
      if l1 != l2:
        if 'second' in l1 and 'second' in l2:
          print("Your ans : " + l1)
          print("Ref  ans : " + l2)
        elif 'Bytes' in l1 and 'Bytes' in l2:
          print("Your ans : " + l1)
          print("Ref  ans : " + l2)
        else:
          print("Wrong")
          print("Your ans : " + l1)
          print("Ref  ans : " + l2)
          print("vimdiff my_ans_test my_ans")
          break
      elif l1 == '':
        print("OK")
        break
