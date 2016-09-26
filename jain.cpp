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

template<typename Data, typename Index>
void check_outshuffle_output(Data *a, Index n) {
	for (Index i = 0; i < n; i++) {
		if (a[i] != inshuffle_perm3(i, n)) {
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

 static std::uint64_t sprimes[] =
 	{1,2,4,10,18,36,66,130,226,442,882,1746,3490,6970,13932,27850,55690,111372,
 		222706,445362,890716,1781362,3562716,7125412,14250780,28501546,57003082,
 		114006148,228012258,456024468,912048756,1824097468,3648194922,7296389692,
 		14592779290,29185558546,58371117036,116742233946,233484467860,466968935700,
 		933937871298,1867875742516,3735751484938,7471502969652,14943005939298,
 		29886011878338,59772023756412,119544047512732,239088095025346,
 		478176190050682,956352380101330,1912704760202572,3825409520405140,
 		7650819040810018,15301638081619986,30603276163239906,61206552326479756,
 		122413104652959510,244826209305918996,489652418611837968,979304837223675928,
 		1958609674447351852,3917219348894703586,7834438697789407128,
 		15668877395578814236};

 template<typename Data, typename Index>
 void prime_inshuffle(Data *a, Index n) {
 	while (n > 1) {
 		// Compute appropriate value of m
 		int i = 0;
 		while (sprimes[i+1] <= n) i++;
 		Index m = sprimes[i];
		std::cout << std::endl << "m = " << m << std::endl;
 		// Move correct m elements to front of the array
 		std::rotate(a+m/2, a+n/2, a+n/2+m/2);

 		// Now use Jain's trick to shuffle a[0,...,m-1];
 		Index cur = 1;
 		Data t = a[cur];
 		do {
 			Index nxt = inshuffle_perm3(cur, m);
 			Data t2 = a[nxt];
 			a[nxt] = t;
 			t = t2;
 			cur = nxt;
 		} while (cur != 1);

 		// Recurse on a[m,...n-1]
 		a += m;
 		n -= m;
 	}
 }


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

template<typename Data, typename Index>
void jain_outshuffle(Data *a, Index n) {
	if (n <= 1) return;

	// Compute appropriate value of m
	Index m = 1;
	while (3*m-1 <= n) m *= 3;
	m -= 1;

	// Use Jain's trick to shuffle a[0,...,m-1];
	for (Index g = 1; g < m; g *= 3) {
		Index cur = g-1;
		Data t = a[cur];
		do {
			Index nxt = outshuffle_perm3(cur, m);
			Data t2 = a[nxt];
			a[nxt] = t;
			t = t2;
			cur = nxt;
		} while (cur != g-1);
	}

	// Recurse on a[m,...n-1]
	jain_outshuffle(a+m, n-m);

	// Regroup odds and evens
	std::rotate(a+m/2, a+m, a+m+(n-m)/2);
}

template<typename Data, typename Index>
void prime_outshuffle(Data *a, Index n) {
	if (n <= 1) return;

	// Compute appropriate value of m
	int i = 0;
	while (sprimes[i+1] <= n) i++;
	Index m = sprimes[i];

	// Use Jain's trick to shuffle a[0,...,m-1];
	Index cur = 1;
	Data t = a[cur];
	do {
		Index nxt = outshuffle_perm3(cur, m);
		Data t2 = a[nxt];
		a[nxt] = t;
		t = t2;
		cur = nxt;
	} while (cur != 1);

	// Recurse on a[m,...n-1]
	prime_outshuffle(a+m, n-m);

	// Regroup odds and evens
	std::rotate(a+m/2, a+m, a+m+(n-m)/2);
}


template<typename Data, typename Index>
void shuffle16() {

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

// This version tries to use prefetching, but it doesn't help much. Memory
// bandwidth is the bottleneck.
template<typename Data, typename Index>
void prime_inshuffle_pf(Data *a, Index n) {
	while (n > 1) {
		// Compute appropriate value of m
 		int i = 0;
 		while (sprimes[i+1] <= n) i++;
 		Index m = sprimes[i];

		// Move correct m elements to front of the array
		std::rotate(a+m/2, a+n/2, a+n/2+m/2);

		// Now use Jain's trick to shuffle a[0,...,m-1];
		Index cur = 1;
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
		} while (buf[bicur] != 1);

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

	std::cout << "Permuting using prime_inshuffle()...";
	std::cout.flush();
	print_array(a, n);
	start = std::chrono::high_resolution_clock::now();
	prime_inshuffle(a, n);
	stop =  std::chrono::high_resolution_clock::now();
	elapsed = stop - start;
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

	std::cout << "Refilling...";
	std::cout.flush();
	std::iota(a, a+n, 0);
	std::cout << "done" << std::endl;

	std::cout << "Permuting using prime_inshuffle_pf()...";
	std::cout.flush();
	print_array(a, n);
	start = std::chrono::high_resolution_clock::now();
	prime_inshuffle_pf(a, n);
	stop =  std::chrono::high_resolution_clock::now();
	elapsed = stop - start;
	std::cout << "done (" << elapsed.count() << "s)" << std::endl;
	print_array(a, n);
	check_inshuffle_output(a, n);

	std::cout << "Refilling...";
	std::cout.flush();
	std::iota(a, a+n, 0);
	std::cout << "done" << std::endl;

	std::cout << "Permuting using jain_outshuffle()...";
	std::cout.flush();
	print_array(a, n);
	start = std::chrono::high_resolution_clock::now();
	jain_outshuffle(a, n);
	stop =  std::chrono::high_resolution_clock::now();
	elapsed = stop - start;
	std::cout << "done (" << elapsed.count() << "s)" << std::endl;
	print_array(a, n);
	check_outshuffle_output(a, n);

	std::cout << "Refilling...";
	std::cout.flush();
	std::iota(a, a+n, 0);
	std::cout << "done" << std::endl;

	std::cout << "Permuting using prime_outshuffle()...";
	std::cout.flush();
	print_array(a, n);
	start = std::chrono::high_resolution_clock::now();
	prime_outshuffle(a, n);
	stop =  std::chrono::high_resolution_clock::now();
	elapsed = stop - start;
	std::cout << "done (" << elapsed.count() << "s)" << std::endl;
	print_array(a, n);
	check_outshuffle_output(a, n);

}
