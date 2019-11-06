#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <optional>
#include <unordered_set>
#include "grammarFragment.h"
#include "utility.h"

//niave markov chains
//just remembers adjacency 
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

void buildDict (const char* file, GrammarGraph& graph)
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

	std::string start = graph.randomWord("$opening");
	std::cout << start << ' ';
	int i = 0;
	while (true)
	{
		auto find = chains.getWord(start);
		start = *find; 

		if((*find)[0] == '$') find =  graph.randomWord(*find);
		std::cout << *find << ' ' ; 
	}
	std::cout << '\n';
}

//build graph from a file
GrammarGraph buildGraph (const char* file)
{

	GrammarGraph graph; 
	std::ifstream in;
	in.open(file);

	std::string line;
	while (std::getline(in, line))
	{
		if(line[0] == '$')
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

		if(line[0] != '$')
		{
			std::string key = getKey(line);
			std::string word;
			bool stripped = false;
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
					std::string t = "$" + key;
					graph.insertWord(t, word);
					word = "";
					continue;
				}

				word += a;
			}
				if(word.length())
				{
					std::string t = "$" + key;
					graph.insertWord(t, word);
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

	auto graph = buildGraph(argv[2]);
	std::cout << graph.recursiveBuild("$opening $middle $closing") << std::endl;
//graph.dumpFile("$verb","/tmp/verbs.txt");
//	graph.dumpFile("$noun","/tmp/nouns.txt");
//	graph.dumpFile("$adjective","/tmp/adjectives.txt");
//	graph.dumpFile("$adverb","/tmp/adverbs.txt");
	//buildDict (argv[1], graph);
}
