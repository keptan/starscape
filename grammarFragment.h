#include <cctype>
#include <memory>
#include <unordered_set>
#include "utility.h"

//graph node
//contains an arbitrary number of parents, children, and words
//acts as a category 
//for example 'verb' has the child node 'attacks', 
//but also contains an assortment of verbs directly
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

	//choses either from current words or from this nodes children
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

	//selects a random word, from this node or from its children
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

//builds and manages an acyclical directed graph of grammar nodes 
class GrammarGraph
{
	std::unordered_map<std::string, std::shared_ptr<GrammarNode>> 
	graph; 


	//creates a node if it doesn't exist, or retrieves an existing node 
	std::shared_ptr<GrammarNode> getNode (const std::string& k) 
	{
		const auto it = graph.find(k);
		if(it != graph.end()) return it->second;

		const auto ptr = std::make_shared<GrammarNode>(k);
		graph[k] = ptr;
		return ptr;
	}

	//same as above but returns a nullptr instead of silently creating a node
	std::shared_ptr<GrammarNode> node (const std::string& k)
	{
		const auto it = graph.find(k);
		if(it == graph.end()) return nullptr; 

		return it->second;
	}

	public:

	//searches parents of a node 
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

	//depth first recursive search, uses helper function
	std::optional<std::string> cat (const std::string& word)
	{
		const auto node = getNode("$word");
		return cat(node, word);
	}

	//searches res and children for a string 
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

	//inserts a relation
	//such as nouns -> people -> girls
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

	//inserts a word into a node
	//for example "hello" inserted into "greetings"
	void insertWord (const std::string& key, const std::string& word)
	{
		const auto node = getNode(key);
		node->insertWord(word);
	}

	//prints the contents of a node 
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

	//finds a random word from a category
	std::string randomWord (const std::string& key)
	{
		std::string s = key[0] == '$' ? key : "$" + key;
		const auto n = node(s);
		if(!n) 
		{ 
			std::cerr << "No nodes for " << key << std::endl;
			throw; 
		}
		if(!n->size())
		{
			std::cerr << "node has no members! " << key << std::endl;
			throw;
		}
		return n->getRandom();
	}

	//recursively builds a string from a category
	//for example $character said $greeting
	//would be expanded into 'ben said hello'
	//or similiar 
	std::string recursiveBuild (const std::string& string)
	{
		std::string out; 

		std::string word; 
		bool send = false;
		for(const auto c : string)
		{
			if(word.length() == 0 && c == '$') send = true; 
			if(word.length() && c == ' ') 
			{
				out += send ? recursiveBuild(randomWord(word)) : word; 
				out += ' ';
				send = false; 
				word = "";
				continue; 
			}
			if(c != ' ')  word += c; 
		}

		out += send ? recursiveBuild(randomWord(word)) : word; 

		return out;
	}

	//dumps a file into a category
	//can dump a file of verbs into the verb category for example 
	void dumpFile (const std::string& key, const std::string& filename)
	{
		std::ifstream in;
		in.open(filename);
		std::string line; 
		while(std::getline(in, line)) insertWord(key, line);
	}

};


