"""Code to find a sequence of shuffle primes that form (roughly) a doubling
 sequence"""
from __future__ import print_function
from __future__ import division
import sys
import subprocess

def mod_power(b, x, n):
    if x == 0: return 1
    if x == 1: return b
    halfx = x//2
    t = mod_power(b, halfx, n)
    t = t*t % n
    if x % 2:
        t = t*b % n
    return t

def is_prime(n):
    out = subprocess.check_output(['factor', str(n)])
    return len(out.split()) == 2

def shuffle_prime(n):
    if not is_prime(n+1):
        return False
    out = subprocess.check_output(['factor', str(n)])
    factors = [int(x) for x in out.split()[1:]]
    factors = list(set(factors))
    for f in factors:
        if mod_power(2, n/f, n+1) == 1:
            return False
    return True

if __name__ == "__main__":
    ub = 10
    print ("std::uint64_t sprimes[] = {2,4", end="")
    while ub < 2**64:
        j = ub
        while not shuffle_prime(j):
            j -= 2
        print(",{}".format(j), end="")
        sys.stdout.flush()
        ub = 2*j
    print("};")
