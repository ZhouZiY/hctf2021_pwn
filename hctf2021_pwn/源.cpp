#include "xml.hpp"
#include <iostream>
#include <cstring>

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
					// _CrtDbgBreak();
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

char* XML_NODE::isInsertable(int x)
{
	if (x > 100 || x < 0)
	{
		return nullptr;
	}
	return backup;
}

std::shared_ptr<XML_NODE> pnode(std::shared_ptr<XML_NODE>x, std::string s, std::string name="")
{
	std::cout << s << x->nodeName << "  ";
	bool isShowOrFind = true;
	if (name != "") isShowOrFind = false;
	if (isShowOrFind)
	{
		for (auto& a : x->attribute)
		{
			std::cout << a.first << "=" << a.second << "  ";
		}
		if (x->data)
			std::cout << *x->data << "  ";
	}
	if (x->node)
	{
		if (isShowOrFind) std::cout << std::endl;
		for (auto& a : *x->node)
		{
			std::shared_ptr<XML_NODE> retVal = pnode(a, s + "  ", name);
			if (isShowOrFind) std::cout << std::endl;
			else
			{
				if (retVal != nullptr) return retVal;
			}
		}
	}
	return (x->nodeName == name)?x:nullptr;
}

int XML::getEditStatus(std::string& name, std::string& content)
{
	int retVal = 0;
	int length1 = 0;
	char *insertPlace, *temp;
	std::shared_ptr<XML_NODE> tempNode = pnode(*node->begin(), "", name);
	if (tempNode)
	{
		retVal += 1;
		length1 = tempNode->data->length();
		insertPlace = tempNode->isInsertable(length1);
		if (insertPlace[0] != '\x99')
		{
			retVal += 1;
			return retVal;
		}
	}
	return 0;
}

void XML::editXML(std::string& name, std::string& content)
{
	int status = getEditStatus(name, content);
	if (status >= 1)
	{
		std::shared_ptr<XML_NODE> a = pnode(*node->begin(), "", name);
		if (a && a->nodeName == name)
		{
			if (status == 1)
			{
				*(a->data) = content;
			}
			else
			{
				for (int i = 0; i < a->data->length(); i++)
				{
					a->backup[i] = (*a->data)[i];
				}
				*(a->data) = content;
			}
		}
	}
	else
	{
		std::cout << "No such name" << std::endl;
	}
	return;
}

void menu()
{
	using namespace std;
	cout << "Please input your choice" << endl;
	cout << "1. Parse XML File" << endl;
	cout << "2. Edit existed XML Node" << endl;
	cout << "3. Show XML Information" << endl;
	cout << "Choice: ";
}

int main()
{
	using namespace std;
	int choice = 0;
	XML xmlfile;
	string nodeName, content;
	cout << "Welcome to D^3CTF 2021!" << endl;
	while (1)
	{
		menu();
		cin >> choice;
		switch (choice)
		{
		case 1:
			xmlfile.parseXml("BFS.vcxproj");
			break;
		
		case 2:
			cout << "Please input the node name which you want to edit";
			cin >> nodeName >> content;
			xmlfile.editXML(nodeName, content);
			break;

		case 3:
			pnode(*xmlfile.node->begin(), "");
			break;
		default:
			break;
		}
	}

}