


#include "xml.hpp"
#include <iostream>

void XML::parseXml(const std::string& fileName)
{
	const std::locale& loc = std::locale();

	std::basic_ifstream <std::string::value_type> stream(fileName);
	stream.imbue(loc);

	v = { std::istreambuf_iterator<std::string::value_type>(stream.rdbuf()), std::istreambuf_iterator<std::string::value_type>() };
	v.push_back(0);

	/*
	for (auto& x : v)
		std::cout << x;
	*/

	auto current = v.begin(), end = v.end();

	while (*current && current < end)
	{
		parseNode(current);
	}

}

bool XML::parseNode(std::vector<std::string::value_type>::iterator& current)
{
	while (*current)
	{
		switch (*current)
		{
		case '?':
		{
			if (
				(*(current + 1) == 'x' || *(current + 1) == 'X') &&
				(*(current + 2) == 'm' || (*(current + 2)) == 'M') &&
				(*(current + 3) == 'l' || (*(current + 3)) == 'L')
				)
			{
				current = iterFind(current, CHARACTACTERS::GT);
				++current;
				//current += 3;
				// todo
			}
		}
		break;

		case '!':
			break;
		case CHARACTACTERS::LT:
		case '\n':
			++current;
			break;
		default:
			parseElement(current);
			break;
		}
	}
	return true;

}

void XML_NODE::name(std::vector<std::string::value_type>::iterator begin, std::vector<std::string::value_type>::iterator end)
{
	this->nodeName = { begin,end };
}
bool XML::parseElement(std::vector<std::string::value_type>::iterator& current)
{
	std::shared_ptr<XML_NODE> element{ std::make_shared<XML_NODE>() };
	element->parse(current);
	if (!this->node)
		this->node = std::make_shared<std::vector<std::shared_ptr<XML_NODE>>>();
	this->node->push_back(element);
	return true;
}
void XML_NODE::parseNodeAttribute(std::vector<std::string::value_type>::iterator& current)
{
	while (*current)
	{
		switch (*current)
		{
		case CHARACTACTERS::GT:
			++current;
			return;
			break;
		case CHARACTACTERS::SLASH:
			return;
			break;
		case CHARACTACTERS::BLANK:
			++current;
			break;
		default:
		{
			auto equal = iterFind(current, CHARACTACTERS::EQUAL);

			auto lquot = iterFind(equal, CHARACTACTERS::QUOT);
			auto rquot = iterFind((lquot + 1), CHARACTACTERS::QUOT);
			attribute.insert(std::pair < std::string, std::string>({ current, equal }, { lquot + 1,rquot }));
			current = rquot + 1;
		}
		break;
		}
	}
}
void XML_NODE::paeseNodeContents(std::vector<std::string::value_type>::iterator& current)
{
	while (*current)
	{
		switch (*current)
		{
		case CHARACTACTERS::LT:
		{
			if (*(current + 1) == CHARACTACTERS::SLASH)
			{
				current += 2;
				auto gt = iterFind(current, CHARACTACTERS::GT);
				if (this->nodeName != std::string{ current, gt })
				{
					//error
					_CrtDbgBreak();
				}
				current = gt + 1;
				return;
			}
			else
			{
				log(current, "in paeseNodeContents");
				++current;
				std::shared_ptr<XML_NODE> node(std::make_shared<XML_NODE>());
				node->parse(current);
				if (!this->node)
					this->node = std::make_shared < std::vector < std::shared_ptr<XML_NODE>>>();
				this->node->push_back(node);
			}
			break;
		}
		case CHARACTACTERS::NEWLINE:
		case CHARACTACTERS::BLANK:
			++current;
			break;
		default:
		{
			auto lt = iterFind(current, CHARACTACTERS::LT);
			data = std::make_shared <std::string>(current, lt);
			current = lt;
			break;
		}

		}
	}
}


bool XML_NODE::parseName(std::vector<std::string::value_type>::iterator& current)
{
	auto nameEnd = current;

	while (*nameEnd)
	{
		if (*nameEnd != CHARACTACTERS::BLANK &&
			*nameEnd != CHARACTACTERS::GT)
			++nameEnd;
		else
			break;
	}
	this->name(current, nameEnd);
	current = nameEnd;
	return true;

}
void XML_NODE::parse(std::vector<std::string::value_type>::iterator& current)
{


	log(current, "parse name");
	this->parseName(current);

	log(current, "parseNodeAttribute");
	this->parseNodeAttribute(current);
	if (*current == CHARACTACTERS::SLASH && *(current + 1) == CHARACTACTERS::GT)
	{
		current += 2;
		return;
	}
	log(current, "paeseNodeContents");
	this->paeseNodeContents(current);
}

void pnode(std::shared_ptr<XML_NODE>x, std::string s)
{
	std::cout << s << x->nodeName << "  ";
	for (auto& a : x->attribute)
	{
		std::cout << a.first << "=" << a.second << "  ";
	}
	if (x->data)
		std::cout << *x->data << "  ";
	else if (x->node)
	{
		std::cout << std::endl;
		for (auto& a : *x->node)
		{
			pnode(a, s + "  ");
			std::cout << std::endl;
		}
	}


}
int main()
{
	using namespace std;



	XML x;
	x.parseXml("C:\\code\\hctf2021_pwn\\hctf2021_pwn\\BFS.vcxproj");
	pnode(*x.node->begin(), "");


}