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


typedef std::uint64_t Index;
typedef std::uint32_t Data;

typedef void(*Rotater)(Data*,Index,Index);

int main(int argc, char *argv[]) {
	Index n;
    if (argc != 2) usage(argv[0]);
    std::istringstream is_n(argv[1]);
    is_n >> n;

	while (n > 0) {
		Index k = 0, cum = 1;
		while (3*cum-1 <= n) {
			k++;
			cum *= 3;
		}
		Index m = cum-1;
		std::cout << "n = " << n << ", k = " << k << ", cum = " << cum
			<< ", m = " << m << std::endl;

		n -= m;

		Index sum = 0;
		auto start = std::chrono::high_resolution_clock::now();
		for (Index i = 0; i < m; i++) {
			sum += outshuffle_perm1(i, m);
		}
		auto stop =  std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = stop - start;
		std::cout << "outshuffle1: " << elapsed.count() << " " << sum << std::endl;

		sum = 0;
		start = std::chrono::high_resolution_clock::now();
		for (Index i = 0; i < m; i++) {
			sum += outshuffle_perm2(i, m);
		}
		stop =  std::chrono::high_resolution_clock::now();
		elapsed = stop - start;
		std::cout << "outshuffle2: " << elapsed.count() << " " << sum << std::endl;

		sum = 0;
		start = std::chrono::high_resolution_clock::now();
		for (Index i = 0; i < m; i++) {
			sum += outshuffle_perm3(i, m);
		}
		stop =  std::chrono::high_resolution_clock::now();
		elapsed = stop - start;
		std::cout << "outshuffle3: " << elapsed.count() << " " << sum << std::endl;

		sum = 0;
		start = std::chrono::high_resolution_clock::now();
		for (Index i = 0; i < m; i++) {
			sum += inshuffle_perm1(i, m);
		}
		stop =  std::chrono::high_resolution_clock::now();
		elapsed = stop - start;
		std::cout << "inshuffle1: " << elapsed.count() << " " << sum << std::endl;

		sum = 0;
		start = std::chrono::high_resolution_clock::now();
		for (Index i = 0; i < m; i++) {
			sum += inshuffle_perm2(i, m);
		}
		stop =  std::chrono::high_resolution_clock::now();
		elapsed = stop - start;
		std::cout << "inshuffle2: " << elapsed.count() << " " << sum << std::endl;

		sum = 0;
		start = std::chrono::high_resolution_clock::now();
		for (Index i = 0; i < m; i++) {
			sum += inshuffle_perm3(i, m);
		}
		stop =  std::chrono::high_resolution_clock::now();
		elapsed = stop - start;
		std::cout << "inshuffle3: " << elapsed.count() << " " << sum << std::endl;

		if (m < 50) {
			for (Index i = 0; i < m-1; i++) {
				std::cout << inshuffle_perm1(i, m) << ",";
			}
			std::cout << inshuffle_perm1(m-1, m) << std::endl;

			for (Index i = 0; i < m-1; i++) {
				std::cout << inshuffle_perm2(i, m) << ",";
			}
			std::cout << inshuffle_perm2(m-1, m) << std::endl;

			for (Index i = 0; i < m-1; i++) {
				std::cout << inshuffle_perm3(i, m) << ",";
			}
			std::cout << inshuffle_perm3(m-1, m) << std::endl;
		}
	}
}
