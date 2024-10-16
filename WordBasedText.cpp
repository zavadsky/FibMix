#include "include\WordBasedText.h"

WordBasedText::WordBasedText(string fname,char t)
{
    std::ifstream file(fname);
    buffer << file.rdbuf();
    alpha_num=t;
    word_frequences();
}


bool isalnum0(char c,char prev) {
    return isalnum(c) || c=='-' || (int(c)==39 && isalnum(prev));
}

// Reading a word from alpha-numeric or punctuation stream
string WordBasedText::get_word() {
string sa="",sn="";
char a,c=0,prev;
    if(buffer.eof())
        return "";
    if(alpha_num==2) {
        buffer>>sa;
        return sa;
    }
    if(alpha_num=='#') {
        buffer>>noskipws>>c;
        if(c) {
        sn=c;
        while(c!='#' && !buffer.eof()) {
            buffer>>noskipws>>c;
            sn+=c;
        }
        }
        return sn;
    }
    do {
        prev=c;
        buffer>>noskipws>>c;
        sn+=c==10?'#':c;
    } while(!isalnum0(c,prev) && ! buffer.eof());
    if(buffer.eof())
        return sn;
    buffer.unget();
    sn=sn.substr(0,sn.length()-1);
    do {
        prev=c;
        buffer>>noskipws>>c;
        sa+=c;
    } while(isalnum0(c,prev) && ! buffer.eof());
    if(buffer.eof())
        return sa;
    buffer.unget();
    if(alpha_num) {
        sa=sa.substr(0,sa.length()-1);
        return sa;
    }
    if(sn!="")
        return sn;
    do {
        prev=c;
        buffer>>noskipws>>c;
        sn+=c==10?'#':c;
    } while(!isalnum0(c,prev) && ! buffer.eof());
    if(buffer.eof())
        return sn;
    buffer.unget();
    sn=sn.substr(0,sn.length()-1);
    return sn;
}

//Create different maps and dictionaries
int WordBasedText::word_frequences() {
double pi,sum1=0,sum2=0;
string word;
int i=0,k=0;
    Nwords=0;
    // Create the map <word,frequency> - word_freq
	while ( ! buffer.eof() ) {
		word=get_word();
		if(word_freq.find(word)!=word_freq.end()) {
			word_freq[word]++;
        } else {
			word_freq.insert(make_pair(word,1));
		}
		Nwords++;
    }
	cout<<endl<<"Input file processed. <word,frequency> map buit. Words in the text: "<<Nwords<<endl;
	// Create the multimap <frequency,word> consisting all different words - freq_word
	for(auto it=word_freq.begin();it!=word_freq.end();it++,diff_words++) {
		freq_word.insert(make_pair(it->second,it->first));
		// Calculate Shannon entropy
		pi=(double)it->second;
		entropy-=pi*(long double)log2((long double)pi/Nwords);
	}
    cout<<"<frequency,word> map built "<<sum1<<" "<<sum2<<endl;
	// Create 1) map <word,symbol> which maps words of text to integers according to descending order of their frequencies;
	// 2) vector of Frequencies of all unique words; 3) vector of different Frequencies DiffFreq; 4) map freq_freq<frequency, number of words having this frequency>
int j=-1;
int frq=0;
NFreq=0;
multimap<int,string> :: iterator it1;
	for(it1=freq_word.begin(),i=0;it1!=freq_word.end();i++,it1++) {
        if(it1->first!=frq) {
            if(frq)
                freq_freq.insert(make_pair(frq,i-j));
            frq=it1->first;
            j=i;
            DiffFreq.push_back(frq);
            NFreq++;
        }
		word_symbol.insert(make_pair(it1->second,i)); //insert first element from freq_word map
		Frequencies.push_back(it1->first);
	}
    freq_freq.insert(make_pair(frq,i-j));
	cout<<"<word,symbol> map built. Different words in text: "<<diff_words<<". Entropy H0="<<(int)entropy/8<<" bytes."<<endl;
	return Nwords;
}

#define shortestDigitLen 2
#define longestDigitLen 8
#define longestDigitLen2 3

// Searching the optimal code - Best Code Search
// Algorithm 1 from the paper
void WordBasedText::BCS(uint8_t dSz[digitArSize], uint32_t dN, uint64_t fullN,
	uint64_t incN1,uint64_t incN2, double cLen, double fullSz, uint32_t* PSum, uint32_t maxC)
{
	if(dN>27)
        return;
	if (dN > 10)        // All digits starting from 12th = 2
	{
		if (fullN > maxC)       // Line 1 of Alg. 1
		{
			dSz[dN] = 2;

			if (fullSz < bestFullBitsSz)    // Line 2 of Alg. 1
			{
				bestFullBitsSz = fullSz;
				bestBlocksNum = dN;
				for (uint32_t i = 0; i < digitArSize; i++)
				{
					bestDigitsSz[i] = dSz[i];
				}
			}
			return;                         // Line 3 of Alg. 1
		}

		dSz[dN] = 2;
		double newSz = fullSz + (cLen + 1) * getSum(fullN, fullN + incN1 - 1, PSum, maxC);
		BCS(dSz, dN + 1, fullN + incN1, incN1 + incN2, incN2, cLen + 1, newSz, PSum, maxC);
	}
	else {
        int lD=dN<2?longestDigitLen:longestDigitLen2;
        for(int i=shortestDigitLen;i<=lD;i++) {     // Line 5 of Alg. 1
            dSz[dN] = i;                            // Line 6 of Alg. 1
            // Line 7 of Alg. 1
            double newSz = fullSz + (cLen + log2(i)) * getSum(fullN, fullN + incN1 - 1, PSum, maxC);
            // Line 8 of Alg. 1
            BCS(dSz, dN + 1, fullN + incN1, incN1 + incN2, (i-2)*incN1 + (i-1)*incN2, cLen + log2(i), newSz, PSum, maxC);
		}
	}
}

// Precalculation for searching the optimal Mixed d-ary Fibonacci code
void WordBasedText::precalcBestBlocks()
{
	uint32_t* PSum = new uint32_t[diff_words*5 + 1];
	for(int i=0;i<=diff_words*5;i++)
        PSum[i]=0;

	PSum[0]=Frequencies[0];

	for (uint32_t i = 1; i <= diff_words; i++)
	{
		if(i<diff_words)
            PSum[i] = PSum[i - 1] + Frequencies[i];
        else
            PSum[i]=PSum[i-1];
	}

	bestFullBitsSz = UINT64_MAX;
	uint8_t digitLen[digitArSize]; // Bit lengths of digits of optimal code
	for(int i=0;i<digitArSize;i++)
        digitLen[i]=2;

	BCS(digitLen, 0, 0, 0, 1, 0, 0, PSum, diff_words); // Call the optimal search function

    cout<<endl<<"Optimal Mixed d-ary Fibonacci code: "<<endl;
	for (uint32_t i = 0; i < digitArSize; i++)
        cout<<(int)bestDigitsSz[i]<<" | ";

	delete[] PSum;
}

// Difference between PSum values in line 7 of Alg. 1
uint64_t WordBasedText::getSum(uint32_t l, uint32_t r, uint32_t* pref, uint32_t maxCode)
{
	uint64_t res = 0;
	res += r > maxCode ? pref[maxCode] : pref[r];
	res -= l == 0 ? 0 : pref[l - 1];

	return res;
}

// Difference between PSum for BCMix codes
uint64_t WordBasedText::getSum_bmix(uint32_t l, uint32_t r)
{
	uint64_t res = 0;

	res += r > diff_words ? PSum[diff_words] : PSum[r];
	res -= l == 0 ? 0 : PSum[l - 1];

	return res;
}

// Searching the optimal code for BCMix codes
void WordBasedText::BCS_bmix(uint8_t* dSz, uint32_t dN, uint64_t fullN,
	uint64_t incN, uint64_t cLen, uint64_t fullSz)
{
    if(dN>=24)
        return;
	if (fullN > diff_words) {
			dSz[dN] = 1;
			if (fullSz < bestFullBitsSz)
			{
				bestFullBitsSz = fullSz;
				bestBlocksNum = dN-1;
				for (uint32_t i = 0; i < digitArSize; i++)
				{
					bestDigitsSz[i] = dSz[i];
				}
			}
			return;
    }
    int lowestDigit=2,highestDigit=2;
        if(dN<4)
            highestDigit=4;
        else if(dN<8)
            lowestDigit=1;
        for(int i=lowestDigit;i<=highestDigit;i++) {
            dSz[dN] = i;
            uint64_t newSz = fullSz + (cLen + i) * getSum_bmix(fullN, fullN + incN - 1);
            BCS_bmix(dSz, dN + 1, fullN + incN, incN * ((1 << i)-1), cLen + i, newSz);
		}
}

// Precalculation for searching the optimal BCMix code
void WordBasedText::precalcBestBlocks_bmix()
{
	PSum = new uint32_t[diff_words*5 + 1];
	for(int i=0;i<=diff_words*5;i++)
        PSum[i]=0;

	PSum[0]=Frequencies[0];

	for (uint32_t i = 1; i <= diff_words; i++)
		if(i<diff_words)
            PSum[i] = PSum[i - 1] + Frequencies[i];
        else
            PSum[i] = PSum[i-1];

	bestFullBitsSz = UINT64_MAX;
	uint8_t digitLen[digitArSize]; // Bit lengths of digits of optimal code
	for(int i=0;i<digitArSize;i++)
        bestDigitsSz[i]=2;

	BCS_bmix(digitLen, 0, 0, 1, 0, 0); // Call the optimal search function

	cout<<endl<<"Optimal BCMix code: ";
	for(int i=0;i<16;i++)
        cout<<(int)bestDigitsSz[i]<<"|";
    cout<<endl;

	delete[] PSum;
}


WordBasedText::~WordBasedText()
{
    //dtor
}

