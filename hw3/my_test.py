import subprocess

nowR = 7
nowC = 6
f = open("my_do", "w")

def showNochange():
  show_list = ["DBAVerage", "DBCount", "DBMAx", "DBMIn", "DBPrint -c", "DBRead", "DBSUm"]
  for i in range(-1, nowC + 1):
    for j in show_list:
      print(j + ' ' + str(i), file=f)

  for i in range(-1, nowR + 1):
      print("DBPrint -r " + str(i), file=f)

  for i in range(-1, nowR + 1):
    for j in range(-1, nowC + 1):
      print("DBPrint " + str(i) + " " + str(j), file=f)
  print("DBPrint -t", file=f)


print("dbp -t ", file=f)
print("dbr tests/test3.csv", file=f)
print("dbr tests/test1.csv", file=f)
print("dbr tests/test1.csv", file=f)
print("dbr tests/test2.csv -R", file=f)
showNochange()
print("DBAPpend -r - 123 ", file=f)
print("DBAPpend -r 1 2 3 4 5 6 7 8 9 10 ", file=f)
nowR += 2
showNochange()
print("DBAPpend -c - 123 ", file=f)
print("DBAPpend -c 1 2 3 4 5 6 7 8 9 10 ", file=f)
nowC += 2
showNochange()

nowR -= 1
print("DBDelete -c -1", file=f)
print("DBDelete -c 0 ", file=f)
showNochange()
nowR -= 1
print("DBDelete -r -1", file=f)
print("DBDelete -r 0 ", file=f)
showNochange()
print("DBSort 0 1 2 3 -1 ", file=f)
print("DBSort 3 2 1 5", file=f)
showNochange()
print("DBSort 1 2 3 4", file=f)
showNochange()

print("Q -F", file=f)
f.close()

subprocess.Popen(["./mydb", "-f", "my_do"], stdout=open("my_ans_test","w"), stderr=subprocess.STDOUT)
subprocess.Popen(["ref/mydb-linux", "-f", "my_do"], stdout=open("my_ans","w"), stderr=subprocess.STDOUT)

"""
not test:
DOfile
"""
