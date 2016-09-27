from __future__ import print_function
import sys


def height(n):
    """Return the height of an Eytzinger tree of size n"""
    h = 0
    m = 1
    while (m < n):
        h += 1
        m += 2**h
    return h

def eytzinger(i, n):
    """This is the Eytzinger permutation"""

def my_outshuffle(i, n):
    """This is the outshuffle permutation we need for Eytzinger layouts"""
    if i % 2 == 0:
        return n//2+i//2
    else:
        return i//2

def funny_outshuffle(i, n):
    return (my_outshuffle(i, n) + n//2) % n;

def std_inshuffle(i, n):
    return (2*(i+1) % (n+1))-1;

def print_cycles(cycles):
    print("(" + ")*(".join([",".join([str(x) for x in c])
                                      for c in cycles]) +")")

def print_bin_cycles(cycles):
    print("(" + ")*(".join([",".join(["{0:05b}".format(x) for x in c])
                                      for c in cycles]) +")")

def get_cycles(f, n):
    """Get the cycles of the permutation f(0),...,f(n-1)"""
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
    return cycles

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

def stats():
    for n in range(2, 1000):
        cycles = get_cycles(my_outshuffle, n);
        if len(cycles) == 1:
            print("n={} C={}".format(n, len(get_cycles(my_outshuffle, n))))

if __name__ == "__main__":
    #stats()
    #exit(0)
    n = read_cmd_line()
    marked = set()
    cycles = get_cycles(std_inshuffle, n)
    print_cycles(cycles)
    # print_bin_cycles(cycles)
    cycles = get_cycles(my_outshuffle, n)
    print_cycles(cycles)

    cycles = get_cycles(funny_outshuffle, n)
    print_cycles(cycles)
    # print_bin_cycles(cycles)
