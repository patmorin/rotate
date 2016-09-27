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

/*
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
*/

// This sequence of shuffle-primes is approximately S(i+1) ~= (5/4)S(i)
std::uint64_t sprimes[] = {2U, 4U, 10U, 12U, 18U, 28U, 36U, 52U, 66U, 82U, 106U, 138U, 172U, 226U, 292U, 372U, 466U, 586U, 756U, 946U, 1186U, 1482U, 1860U, 2332U, 2938U, 3676U, 4602U, 5778U, 7228U, 9058U, 11436U, 14340U, 17938U, 22468U, 28098U, 35140U, 43932U, 54916U, 68658U, 85828U, 107308U, 134212U, 167770U, 209716U, 262146U, 327706U, 409690U, 512146U, 640218U, 800332U, 1000428U, 1250628U, 1563292U, 1954156U, 2442778U, 3053482U, 3816876U, 4771162U, 5963980U, 7455028U, 9318810U, 11648530U, 14560690U, 18200932U, 22751236U, 28439140U, 35548938U, 44436276U, 55545388U, 69431826U, 86789788U, 108487242U, 135609090U, 169511428U, 211889322U, 264861658U, 331077100U, 413846482U, 517308106U, 646635330U, 808294258U, 1010367970U, 1262959986U, 1578699988U, 1973375050U, 2466718866U, 3083398690U, 3854248372U, 4817810466U, 6022263108U, 7527829092U, 9409786522U, 11762233276U, 14702791612U, 18378489532U, 22973111932U, 28716389956U, 35895487492U, 44869359396U, 56086699476U, 70108374388U, 87635468002U, 109544335002U, 136930418812U, 171163023556U, 213953779452U, 267442224346U, 334302780450U, 417878475612U, 522348094572U, 652935118228U, 816168897820U, 1020211122276U, 1275263902948U, 1594079878788U, 1992599848500U, 2490749810650U, 3113437263322U, 3891796579258U, 4864745724300U, 6080932155498U, 7601165194372U, 9501456493002U, 11876820616386U, 14846025770482U, 18557532213106U, 23196915266386U, 28996144083042U, 36245180103828U, 45306475129818U, 56633093912500U, 70791367390642U, 88489209238348U, 110611511547946U, 138264389435092U, 172830486793948U, 216038108492532U, 270047635616052U, 337559544520420U, 421949430650556U, 527436788313202U, 659295985391668U, 824119981739602U, 1030149977174508U, 1287687471468442U, 1609609339335562U, 2012011674169492U, 2515014592712002U, 3143768240890018U, 3929710301112762U, 4912137876391386U, 6140172345489250U, 7675215431861580U, 9594019289827018U, 11992524112283908U, 14990655140354892U, 18738318925443646U, 23422898656804578U, 29278623321005742U, 36598279151257260U, 45747848939071596U, 57184811173839588U, 71481013967299522U, 89351267459124412U, 111689084323905546U, 139611355404881950U, 174514194256102542U, 218142742820128186U, 272678428525160238U, 340848035656450306U, 426060044570562886U, 532575055713203610U, 665718819641504526U, 832148524551880686U, 1040185655689850860U, 1300232069612313576U, 1625290087015391988U, 2031612608769240016U, 2539515760961550178U, 3174394701201937726U, 3967993376502422170U, 4959991720628027716U, 6199989650785034706U, 7749987063481293456U, 9687483829351616830U, 12109354786689521098U, 15136693483361901396U};

template<typename Data, typename Index>
void prime_inshuffle(Data *a, Index n) {
	while (n > 1) {
		// Compute appropriate value of m
		int i = 0;
		while (sprimes[i+1] <= n) i++;
		Index m = sprimes[i];

		// Move correct m elements to front of the array
		std::rotate(a+m/2, a+n/2, a+n/2+m/2);

		// Now use Jain's trick to shuffle a[0,...,m-1];
		Index cur = 0;
		Data t = a[cur];
		do {
			Index nxt = inshuffle_perm3(cur, m);
			Data t2 = a[nxt];
			a[nxt] = t;
			t = t2;
			cur = nxt;
		} while (cur != 0);

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
		Data t[2];
		int flipflop = 0;
		t[flipflop] = a[cur];
		do {
			Index nxt = outshuffle_perm3(cur, m);
			t[!flipflop] = a[nxt];
			a[nxt] = t[flipflop];
			flipflop = !flipflop;
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

	// Follow the cycle through a[0,...,m-1];
	Index cur = 0;
	Data t[2];
	int flipflop = 0;
	t[flipflop] = a[cur];
	do {
		Index nxt = outshuffle_perm3(cur, m);
		t[!flipflop] = a[nxt];
		a[nxt] = t[flipflop];
		flipflop = !flipflop;
		cur = nxt;
	} while (cur != 0);

	// Recurse on a[m,...n-1]
	prime_outshuffle(a+m, n-m);

	// Regroup odds and evens
	std::rotate(a+m/2, a+m, a+m+(n-m)/2);
}

// This and blocked_outshuffle are the stars of the show
template<unsigned R, typename Data, typename Index>
void prime_outshuffle_pf(Data *a, Index n) {
	if (n <= 1) return;

	// Compute appropriate value of m
	int i = 0;
	while (sprimes[i+1] <= n) i++;
	Index m = sprimes[i];

	Index cur = 0;
	Index pf = cur;
	for (unsigned i = 0; i < R; i++)
		__builtin_prefetch(a+(pf = outshuffle_perm3(pf, m)), 1);
	Data t[2];
	int flipflop = 0;
	t[flipflop] = a[cur];
	do {
		__builtin_prefetch(a+(pf = outshuffle_perm3(pf, m)), 1);
		Index nxt = outshuffle_perm3(cur, m);
		t[!flipflop] = a[nxt];
		a[nxt] = t[flipflop];
		flipflop = !flipflop;
		cur = nxt;
	} while (cur != 0);

	// Recurse on a[m,...n-1]
	prime_outshuffle(a+m, n-m);

	// Regroup odds and evens
	std::rotate(a+m/2, a+m, a+m+(n-m)/2);
}

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

// Implements a reversed outshuffle on 32 values
template<typename Data>
void rev_outshuffle32(Data *a) {
	const int cycles[6][5] = {
		{1,16,8,4,2},
		{3,17,24,12,6},
		{5,18,9,20,10},
		{7,19,25,28,14},
		{11,21,26,13,22},
		{15,23,27,29,30}
	};
	for (int i = 0; i < 6; i++) {
		Data t1 = a[cycles[i][4]];
		for (int j = 0; j < 5; j++) {
			Data t2 = a[cycles[i][j]];
			a[cycles[i][j]] = t1;
			t1 = t2;
		}
	}
}

// This is the star of the show
template<bool prefetch=false, typename Data, typename Index>
void blocked_outshuffle(Data *a, Index n) {
	Index r = n % 32;
	Index m = n - r;

	prime_outshuffle(a+m, r);

	for(Index i = 0; i < m/32; i++) {
		rev_outshuffle32(a+32*i);
	}
	struct block { Data dummy[16];	};
	if (prefetch) {
		prime_outshuffle_pf<20>((block*)a, m/16);
	} else {
		prime_outshuffle((block*)a, m/16);
	}
	// Regroup odds and evens
	std::rotate(a+m/2, a+m, a+m+r/2);
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

	std::cout << "Refilling...";
	std::cout.flush();
	std::iota(a, a+n, 0);
	std::cout << "done" << std::endl;

	std::cout << "Permuting using blocked_outshuffle()...";
	std::cout.flush();
	print_array(a, n);
	start = std::chrono::high_resolution_clock::now();
	blocked_outshuffle(a, n);
	stop =  std::chrono::high_resolution_clock::now();
	elapsed = stop - start;
	std::cout << "done (" << elapsed.count() << "s)" << std::endl;
	print_array(a, n);
	check_outshuffle_output(a, n);

	std::cout << "Refilling...";
	std::cout.flush();
	std::iota(a, a+n, 0);
	std::cout << "done" << std::endl;

	std::cout << "Permuting using blocked_outshuffle_pf()...";
	std::cout.flush();
	print_array(a, n);
	start = std::chrono::high_resolution_clock::now();
	blocked_outshuffle<true>(a, n);
	stop =  std::chrono::high_resolution_clock::now();
	elapsed = stop - start;
	std::cout << "done (" << elapsed.count() << "s)" << std::endl;
	print_array(a, n);
	check_outshuffle_output(a, n);

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

}
