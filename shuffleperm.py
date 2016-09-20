from __future__ import print_function
import sys

def f(i, n):
    if i % 2 == 0:
        return n//2+i//2
    else:
        return i//2

def print_cycles(cycles):
    print("(" + ")*(".join([",".join([str(x) for x in c]) for c in cycles]) +")")

def print_bin_cycles(cycles):
    print("(" + ")*(".join([",".join(["{0:05b}".format(x) for x in c]) for c in cycles]) +")")

if __name__ == "__main__":
    n = int(sys.argv[1])
    marked = set()
    cycles = []
    for i in range(n):
        if i not in marked:
          marked.add(i)
          c = [i]
          j = f(i, n)
          while j != i:
              c.append(j)
              marked.add(j)
              j = f(j, n)
          cycles.append(c)

    print_cycles(cycles)
    print_bin_cycles(cycles)
