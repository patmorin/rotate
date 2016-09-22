#include<chrono>
#include<iostream>
#include<iomanip>
#include<sstream>
#include<numeric>
#include<algorithm>

#include<cstdlib>


void usage(char *progname) {
	std::cerr << "Usage: " << progname << " <n> <r>" << std::endl;
	exit(-1);
}

template<typename T, typename I>
void reverse(T *a, I n) {
	for (I i = 0; i < n/2; i++) {
		std::swap(a[i], a[n-i-1]);
	}
}

template<typename T, typename I>
void rotate_std(T *a, I n, I r) {
    std::rotate(a, a+n-r, a+n);
}

template<typename T, typename I>
void rotate_stdrev(T *a, I n, I r) {
	std::reverse(a, a+n);
    std::reverse(a, a+r);
	std::reverse(a+r, a+n-r);
}

template<typename T, typename I>
void rotate_rev(T *a, I n, I r) {
	reverse(a, n);
    reverse(a, r);
	reverse(a+r, n-r);
}

// Compute the gcd of a and b, requires a >= b
template<typename I>
I gcd(I a, I b) {
    I r = a % b;
    if (r == 0) return b;
    return gcd(b, r);
}

template<typename T, typename I>
void rotate_gcd(T *a, I n, I r) {
    I g = gcd(n, r);
    I m = n / g;
    for (I k = 0; k < g; k++) {
        I i = k;
        T t = a[i];
        for (I j = 0; j < m; j++) {
            I inxt = (i+r);
						inxt = inxt >= n ? inxt-n : inxt; // avoid mod in favour of cmov
            T t2 = a[inxt];
            a[inxt] = t;
            t = t2;
            i = inxt;
        }
    }
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
void speed_test(T *a, I n, I r, void(*rot)(T*,I,I)) {
    auto start = std::chrono::high_resolution_clock::now();
    rot(a, n, r);
    auto stop =  std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    std::cout << elapsed.count() << std::endl;
}

template<typename T, typename I>
void outshuffle(T *a, I n) {

}



typedef std::uint64_t Index;
typedef std::uint32_t Data;

typedef void(*Rotater)(Data*,Index,Index);

int main(int argc, char *argv[]) {
    Index n, r;

    if (argc != 3) usage(argv[0]);
    std::istringstream is_n(argv[1]);
    is_n >> n;
    std::istringstream is_r(argv[2]);
    is_r >> r;

    std::cout << "n = " << n << ", r = " << r << std::endl;

		auto *a = new Data[n];

		Rotater fns[] =
		{ rotate_std<Data,Index>, rotate_stdrev<Data,Index>,
			rotate_rev<Data,Index>, rotate_gcd<Data,Index> };
		std::string fn_names[] =
		{ "rotate_std", "rotate_stdrev", "rotate_rev", "rotate_gcd" };

		if (n <= 50) {
			for (int i = 0; i < 4; i++) {
				std::iota(a, a+n, 1);
				print_array(a, n);
				rotate_std(a, n, r);
				print_array(a, n);
				std::cout << std::endl;
			}
		} else {
			for (int i = 0; i < 4; i++) {
		    std::iota(a, a+n, 1);

				std::cout << std::setw(20) << fn_names[i] << " : ";
		    speed_test(a, n, r, fns[i]);
			}
		}
    return 0;
}
