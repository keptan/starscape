#include <memory>
#include <unordered_set>

class GrammarNode
{
	std::unordered_set< std::shared_ptr<GrammarNode>> 
	children;

	std::unordered_set< std::shared_ptr<GrammarNode>> 
	parents; 

	std::unordered_set< std::string>
	words;

	public:
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

	void insertRelation (const std::string& key, const std::string& word)
	{
		const auto parent = getNode(key);
		const auto child =  getNode(word);

		parent->insertChild(child);
		child->insertParent(parent);
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
		const auto& children = node->getChildren();
		const auto& parents  = node->getParents();

		for(const auto& c : children)
		{
			std::cout << c->res << std::endl;
		}


		for(const auto& p : parents)
		{
			std::cout << p->res << std::endl;
		}
	}
};


