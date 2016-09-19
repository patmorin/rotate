from __future__ import print_function
import sys

def f(i, n):
    if i % 2 == 0:
        return n//2+i//2
    else:
        return i//2

if __name__ == "__main__":
    n = int(sys.argv[1])
    marked = set()
    for i in range(n):
        if i not in marked:
          marked.add(i)
          print("({}".format(i), end="")
          j = f(i, n)
          while j != i:
              print(",{}".format(j), end="")
              marked.add(j)
              j = f(j, n)
          print(")", end="")
    print()
