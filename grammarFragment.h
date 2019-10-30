#include <memory>
#include <unordered_set>

class GrammarNode
{
	std::unordered_set< std::shared_ptr<GrammarNode>> 
	children;

	std::unordered_set< std::shared_ptr<GrammarNode>> 
	parents; 

	public:
	const std::string res;

	bool operator == (const GrammarNode& n) const
	{
		return (res == n.res);
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
