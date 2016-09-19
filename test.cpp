#include<chrono>
#include<iostream>
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
    //std::cout << "gcd(" << n << "," << r << ")=" << g << std::endl;
    I m = n / g;
    for (I k = 0; k < g; k++) {
        I i = k;
        T t = a[i];
        for (I j = 0; j < m; j++) {
            I inxt = (i+r)%n;
            T t2 = a[inxt];
            //std::cout << t << "->" << a[inxt] << std::endl;
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

typedef std::uint64_t Index;


int main(int argc, char *argv[]) {
    Index n, r;
    if (argc != 3) usage(argv[0]);
    std::istringstream is_n(argv[1]);
    is_n >> n;
    std::istringstream is_r(argv[2]);
    is_r >> r;

    std::cout << "n = " << n << ", r = " << r << std::endl;
	
	auto *a = new std::uint32_t[n];

    std::iota(a, a+n, 1);
    print_array(a, n);
    rotate_std(a, n, r);
    print_array(a, n);

    std::cout << std::endl;

    std::iota(a, a+n, 1);
    print_array(a, n);
    rotate_stdrev(a, n, r);
    print_array(a, n);

    std::cout << std::endl;


    std::iota(a, a+n, 1);
    print_array(a, n);
    rotate_rev(a, n, r);
    print_array(a, n);

    std::cout << std::endl;

    std::iota(a, a+n, 1);
    print_array(a, n);
    rotate_gcd(a, n, r);
    print_array(a, n);

    std::iota(a, a+n, 1);
    speed_test(a, n, r, rotate_std); 

    std::iota(a, a+n, 1);
    speed_test(a, n, r, rotate_stdrev); 

    std::iota(a, a+n, 1);
    speed_test(a, n, r, rotate_rev); 

    std::iota(a, a+n, 1);
    speed_test(a, n, r, rotate_gcd); 
    
    return 0;
}
