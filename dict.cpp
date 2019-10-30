#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <optional>
#include  <random>
#include  <iterator>

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

struct NiaveWordChains
{
	std::unordered_map<std::string, std::vector<std::string>> 
	map; 

	void insert (const std::string& key, const std::string& word)
	{
		const auto it = map.find(key); 
		if(it == map.end())
		{
			map[key] = {word};
			return;
		}

		map[key].push_back(word);
	}

	std::optional<std::string> getWord (const std::string& key)
	{
		const auto it = map.find(key);
		if(it == map.end()) return std::nullopt; 

		const auto& v = map[key];

		return *select_randomly(v.begin(), v.end());
	}

};



void buildDict (const char* file)
{
	std::ifstream in; 
	in.open(file); 

	NiaveWordChains chains;

	std::string line; 
	while (std::getline(in, line))
	{
		std::string previousWord;
		std::string word;
		for(const auto a : line)
		{
			if(a == ' ' || a == '\n')
			{
				if(word.length() && previousWord.length())
				{
					chains.insert(previousWord, word); 
					previousWord = word;
					word = "";
					continue;
				}

				if(word.length())
				{
					previousWord = word; 
					word = "";
					continue;
				}
			}

			word += a; 
		}
	}

	std::string start = "hello";
	std::cout << start << ' ';
	while (true)
	{
		const auto find = chains.getWord(start);
		if(!find) break; 

		std::cout << *find << ' ' ; 
		start = *find; 
	}
	std::cout << '\n';
}


int main (int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage:" << argv[0] << " text file" << std::endl;
		return -1;
	}

	buildDict (argv[1]);
}

