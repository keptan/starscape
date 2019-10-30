#include <memory>
#include <unordered_set>
#include "utility.h"

struct GrammarNode
{
	std::unordered_set< std::shared_ptr<GrammarNode>> 
	children;

	std::unordered_set< std::shared_ptr<GrammarNode>> 
	parents; 

	std::unordered_set< std::string>
	words;

	using SharedNode = std::shared_ptr<GrammarNode>;
	const std::string res;

	GrammarNode (const std::string& s)
		: res(s)
	{}

	bool operator == (const GrammarNode& n) const
	{
		return (res == n.res);
	}

	void insertChild (const SharedNode n)
	{
		children.insert(n);
	}

	void insertParent (const SharedNode n)
	{
		parents.insert(n);
	}

	void insertWord (const std::string& w)
	{
		words.insert(w);
	}

	const std::unordered_set<std::shared_ptr<GrammarNode>>& getChildren (void) const 
	{
		return children;
	}

	const std::unordered_set<std::shared_ptr<GrammarNode>>& getParents (void) const 
	{
		return parents;
	}

	int size (void) const
	{
		int s = words.size(); 
		for(const auto c : children)
		{
			s += c->size();
		}
		return s;
	}

	std::string operator[] (const int i) const
	{
		if(i < words.size()) return *select_randomly(words.begin(), words.end());
		int acc = words.size() -1;

		for(const auto c : children)
		{
			acc += (c->size());
			const auto& g = *c;
			if(acc >= i) return g[acc - i];
		}
		throw("acc didnt acc");
	}

	std::string
	getRandom (void)
	{
		const int choice = randInt(0, size() -1);
		return (*this)[choice];
	}

};

namespace std 
{
	template<>
		struct hash<GrammarNode>
		{
			size_t operator () (const GrammarNode &n) const 
			{
				return hash<std::string>()(n.res);
			}
		};
}

class GrammarGraph
{
	std::unordered_map<std::string, std::shared_ptr<GrammarNode>> 
	graph; 


	std::shared_ptr<GrammarNode> getNode (const std::string& k) 
	{
		const auto it = graph.find(k);
		if(it != graph.end()) return it->second;

		const auto ptr = std::make_shared<GrammarNode>(k);
		graph[k] = ptr;
		return ptr;
	}

	public:

	bool upSearch (const std::shared_ptr<GrammarNode> node, const std::string& key)
	{
		if(node->res == key) return true;
		if(node->parents.size() == 0) return false; 
		for(const auto p : node->parents)
		{
			if(upSearch(p, key)) return true;
		}
		return false; 
	}

	std::optional<std::string> cat (const std::string& word)
	{
		const auto node = getNode("$word");
		return cat(node, word);
	}

	std::optional<std::string> cat (std::shared_ptr<GrammarNode> node, const std::string& w)
	{
		const auto it = node->words.find(w); 
		if(it != node->words.end()) return node->res; 

		for(const auto c : node->children)
		{
			const auto search = cat(c, w);
			if(search) return search;
		}
		return std::nullopt;
	}



	void insertRelation (const std::string& key, const std::string& word)
	{
		const auto parent = getNode(key);
		const auto child =  getNode(word);

		if(upSearch(parent, word))
		{
			std::cout << "avoided cycle" << std::endl;
			return;
		}

		child->insertParent(parent);
		parent->insertChild(child);
		return;
	}

	void insertWord (const std::string& key, const std::string& word)
	{
		const auto node = getNode(key);
		node->insertWord(word);
	}

	void test (const std::string& str)
	{
		const auto node = getNode(str);

		for(const auto& c : node->children)
		{
			std::cout << c->res << std::endl;
		}


		for(const auto& p : node->parents)
		{
			std::cout << p->res << std::endl;
		}
	}

	std::string randomWord (const std::string& key)
	{
		std::string s = key[0] == '$' ? key : "$" + key;
		const auto node = getNode(s);
		return node->getRandom();
	}

	void dumpFile (const std::string& key, const std::string& filename)
	{
		std::ifstream in;
		in.open(filename);
		std::string line; 
		while(std::getline(in, line)) insertWord(key, line);
	}

};


