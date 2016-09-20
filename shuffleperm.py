from __future__ import print_function
import sys

def f(i, n):
    if i % 2 == 0:
        return n//2+i//2
    else:
        return i//2

def print_cycles(cycles):
    print("(" + ")*(".join([",".join([str(x) for x in c])
                                      for c in cycles]) +")")

def print_bin_cycles(cycles):
    print("(" + ")*(".join([",".join(["{0:05b}".format(x) for x in c])
                                      for c in cycles]) +")")

def read_cmd_line():
    n = -1
    if len(sys.argv) == 2:
        try:
            n = int(sys.argv[1])
        except ValueError:
            sys.stderr.write("Unable to parse argument '{}'\n".format(sys.argv[1]))
    if n <= 0:
        sys.stderr.write("Usage: {} <n>\n".format(sys.argv[0]))
        sys.exit(-1)
    return n

if __name__ == "__main__":
    n = read_cmd_line()
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
