#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <optional>
#include  <random>
#include  <iterator>
#include <unordered_set>
#include "grammarFragment.h"

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
			if(a == ' ')
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
		if(word.length() && previousWord.length())
		{
			chains.insert(previousWord, word); 
			previousWord = word;
			word = "";
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

GrammarGraph buildGraph (const char* file)
{

	GrammarGraph graph; 
	std::ifstream in;
	in.open(file);

	std::string line;
	while (std::getline(in, line))
	{
		if(line[0] == '!')
		{
			std::string previousWord;
			std::string word;

			for(const auto a : line)
			{
				if(a == '-')
				{
					if(word.length() && previousWord.length())
					{
						graph.insertRelation(previousWord, word);
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
			if(word.length() && previousWord.length())
			{
				graph.insertRelation(previousWord, word);
				previousWord = word;
				word = "";
			}

		}

		const auto getKey = [](const auto& s)
		{
			std::string key; 
			for(const auto c : s)
			{
				if(c == ':') break;
				key += c; 
			}
			return key;
		};

		if(line[0] != '!')
		{
			std::string key = getKey(line);
			std::string word;
			bool stripped;
			for(const auto a : line)
			{
				if(a == ':') 
				{
					stripped = true;
					continue;
				}

				if(!stripped) continue;

				if(word.length() && a == ',')
				{
					graph.insertWord(key, word);
					continue;
				}

				word += a;
			}
				if(word.length())
				{
					graph.insertWord(key, word);
					continue;
				}
		}
	}

	return graph;
}
					



int main (int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage:" << argv[0] << " text file" << ' ' << "dict file" << std::endl;
		return -1;
	}

	const auto graph = buildGraph(argv[2]);
	buildDict (argv[1]);
}

