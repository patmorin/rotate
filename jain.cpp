#include<chrono>
#include<iostream>
#include<iomanip>
#include<sstream>
#include<numeric>
#include<algorithm>

#include<cstdlib>

void usage(char *progname) {
	std::cerr << "Usage: " << progname << " <n>" << std::endl;
	exit(-1);
}

template<typename T, typename I>
void print_array(T *a, I n) {
    if (n < 50) {
        for (I i = 0; i < n-1; i++) {
            std::cout << a[i] << ",";
        }
        std::cout << a[n-1] << std::endl;
    }
}

template<typename T, typename I>
void speed_test(T *a, I n, I r, I(*rot)(T*,I,I)) {
    auto start = std::chrono::high_resolution_clock::now();
    rot(a, n, r);
    auto stop =  std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    std::cout << elapsed.count() << std::endl;
}

// Precondition: n is even
template<typename I>
inline I inshuffle_perm1(I i, I n) {
	return (2*(i+1) % (n+1))-1;
}

template<typename I>
inline I inshuffle_perm2(I i, I n) {
	if (i < n/2) {
		return 2*i+1;
	} else {
		return 2*i-n;
	}
}

template<typename I>
inline I inshuffle_perm3(I i, I n) {
	I out = 2*i + 1;
	I fix = (i < n/2) ? 0 : n+1;
	return out-fix;
}

// Precondition: n is even
template<typename I>
inline I outshuffle_perm1(I i, I n) {
	I inverse = (n+2)/2; // multiplicative inverse of 2
	return (inverse*(i+1) % (n+1))-1;
}

template<typename I>
inline I outshuffle_perm2(I i, I n) {
	if (i%2 == 0) {
		return n/2+i/2;
	} else {
		return i/2;
	}
}

// This is the fastest implementation in preliminary tests
template<typename I>
inline I outshuffle_perm3(I i, I n) {
	I out = i/2;
	I fix = (i & 1) ? 0 : n/2;
	return out+fix;
}

template<typename Data, typename Index>
void check_inshuffle_output(Data *a, Index n) {
	for (Index i = 0; i < n; i++) {
		if (a[i] != outshuffle_perm3(i, n)) {
			std::cerr << "Mistake in output!!!" << std::endl;
			exit(-1);
		}
	}
}

/*
 * This kind of implementation is probably needed for larger data types
template<typename Data, typename Index>
void jain_inshuffle(Data *a, Index n) {
	while (n > 1) {
		// Compute appropriate value of m
		Index m = 1;
		while (3*m-1 <= n) m *= 3;
		m -= 1;

		// Move correct m elements to front of the array
		std::reverse(a+m/2, a+n/2+m/2);
		std::reverse(a+m/2, a+m);
		std::reverse(a+m, a+n/2+m/2);

		// Now use Jain's trick to shuffle a[0,...,m-1];
		for (Index g = 1; g < m; g *= 3) {
			Index cur = g-1;
			Data t[2] = { a[cur], 0 };
			int flg = 0;
			do {
				Index nxt = inshuffle_perm3(cur, m);
				t[!flg] = a[nxt];
				a[nxt] = t[flg];
				flg = !flg;
				cur = nxt;
			} while (cur != g-1);
		}

		// Recurse on a[m,...n-1]
		a += m;
		n -= m;
	}
}
 */

template<typename Data, typename Index>
void jain_inshuffle(Data *a, Index n) {
	while (n > 1) {
		// Compute appropriate value of m
		Index m = 1;
		while (3*m-1 <= n) m *= 3;
		m -= 1;

		// Move correct m elements to front of the array
		std::rotate(a+m/2, a+n/2, a+n/2+m/2);

		// Now use Jain's trick to shuffle a[0,...,m-1];
		for (Index g = 1; g < m; g *= 3) {
			Index cur = g-1;
			Data t = a[cur];
			do {
				Index nxt = inshuffle_perm3(cur, m);
				Data t2 = a[nxt];
				a[nxt] = t;
				t = t2;
				cur = nxt;
			} while (cur != g-1);
		}

		// Recurse on a[m,...n-1]
		a += m;
		n -= m;
	}
}

// This version tries to use prefetching, but it doesn't help much. Memory
// bandwidth is the bottleneck.
template<typename Data, typename Index>
void jain_inshuffle_pf(Data *a, Index n) {
	while (n > 1) {
		// Compute appropriate value of m
		Index m = 1;
		while (3*m-1 <= n) m *= 3;
		m -= 1;

		// Move correct m elements to front of the array
		std::rotate(a+m/2, a+n/2, a+n/2+m/2);

		// Now use Jain's trick to shuffle a[0,...,m-1];
		for (Index g = 1; g < m; g *= 3) {
			Index cur = g-1;
			Data t = a[cur];

			const Index readahead = 35;
			Index buf[readahead];
			buf[0] = cur;
			for (Index i = 1; i < readahead; i++) {
				buf[i] = inshuffle_perm3(buf[i-1], m);
				__builtin_prefetch(a+buf[i]);
			}

			Index bicur = 0;
			do {
				Index biprv = bicur == 0 ? readahead-1 : bicur-1;
				Index binxt = bicur+1 == readahead ? 0 : bicur+1;

				buf[bicur] = inshuffle_perm3(buf[biprv], m);
				__builtin_prefetch(a+buf[bicur], 1);

				Data t2 = a[buf[binxt]];
				a[buf[binxt]] = t;
				t = t2;
				bicur = binxt;
			} while (buf[bicur] != g-1);
		}

		// Recurse on a[m,...n-1]
		a += m;
		n -= m;
	}
}

int main(int argc, char *argv[]) {
	std::uint64_t n;
    if (argc != 2) usage(argv[0]);
	{
	    std::istringstream is_n(argv[1]);
	    is_n >> n;
	}

	std::cout << "Building and filling...";
	std::cout.flush();
	auto *a = new std::uint32_t[n];

	std::iota(a, a+n, 0);
	std::cout << "done" << std::endl;

	std::cout << "Permuting using jain_inshuffle()...";
	std::cout.flush();
	print_array(a, n);
	auto start = std::chrono::high_resolution_clock::now();
	jain_inshuffle(a, n);
	auto stop =  std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = stop - start;
	std::cout << "done (" << elapsed.count() << "s)" << std::endl;
	print_array(a, n);
	check_inshuffle_output(a, n);

	std::cout << "Refilling...";
	std::cout.flush();
	std::iota(a, a+n, 0);
	std::cout << "done" << std::endl;

	std::cout << "Permuting using jain_inshuffle_pf()...";
	std::cout.flush();
	print_array(a, n);
	start = std::chrono::high_resolution_clock::now();
	jain_inshuffle_pf(a, n);
	stop =  std::chrono::high_resolution_clock::now();
	elapsed = stop - start;
	std::cout << "done (" << elapsed.count() << "s)" << std::endl;
	print_array(a, n);
	check_inshuffle_output(a, n);

}
