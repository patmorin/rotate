"""Code to find a sequence of shuffle primes that form (roughly) a doubling
   sequence. This uses the fact that an even integer n is a shuffle-prime
   if and only if n+1 is prime and 2 is a primitive root of n+1.
"""
from __future__ import print_function
from __future__ import division
import sys
import subprocess

def mod_power(b, x, n):
    """Return b^x mod n"""
    if x == 0: return 1
    if x == 1: return b
    halfx = x//2
    t = mod_power(b, halfx, n)
    t = t*t % n
    if x % 2:
        t = t*b % n
    return t

def prime(n):
    """Test if n is prime"""
    out = subprocess.check_output(['factor', str(n)])
    return len(out.split()) == 2


def shuffle_prime(n):
    """Test if n is a shuffle-prime, i.e., if 2 is a primitive root of n+1"""
    if not prime(n+1):
        return False
    out = subprocess.check_output(['factor', str(n)])
    factors = [int(x) for x in out.split()[1:]]
    factors = list(set(factors))
    for f in factors:
        if mod_power(2, n/f, n+1) == 1:
            return False
    return True

if __name__ == "__main__":
    """List some shuffle-primes that form (roughly) a geometric progression."""
    ub = 12
    print ("std::uint64_t sprimes[] = {2U, 4U, 10U", end="")
    while ub < 2**64:
        j = ub
        while not shuffle_prime(j):
            j += 2
        print(", {}U".format(j), end="")
        sys.stdout.flush()
        ub = 5*j//4
        ub += ub % 2
    print("};")
