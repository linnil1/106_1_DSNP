import subprocess
import os
import numpy.random as random
import time

def myops(fname, f, name):
  print("cirr -r {}".format(fname),  file=f)
  print("usage", file=f)
  print("cirsim -r", file=f)
  print("usage", file=f)
  print("cirfraig", file=f)
  print("usage", file=f)
  print("cirsweep", file=f)
  print("cirw -o " + name, file=f)
  print("Q -F", file=f)
  f.close()

# main
dir_name = "tests.fraig/"
files = [ dir_name + name for name in os.listdir(dir_name) if name.endswith(".aag") ]
random.shuffle(files)
# files[0] = "tests.fraig/sim09.aag"

print(files[0] + " :")
myops(files[0], open("my_ans_do" , "w"), "my_ans")
p0 = subprocess.Popen(["./fraig"  , "-f", "my_ans_do"], stdout=open("my_ans_back_test","w"), stderr=subprocess.STDOUT)
p0.wait()
print("RUN OK")
myops(files[0], open("my_ans_do", "w"), "my_ans_test")
p1 = subprocess.Popen(["ref/fraig", "-f", "my_ans_do"], stdout=open("my_ans_back","w"), stderr=subprocess.STDOUT)
p1.wait()
print("RUN OK")

def showTime(s):
  for l in s.split('\n'):
    if 'second' in l:
      print(l)
    elif 'Bytes' in l:
      print(l)

print("MY")
showTime(open("my_ans_back_test").read())
print('-' * 40)
print("REF")
showTime(open("my_ans_back").read())
print('-' * 40)

p = subprocess.Popen(["src/sat/checker/satTest", "my_ans", "my_ans_test"])
p.wait()
