// clang++ main.cpp -O3 -o Truth
// md5sum Truth f7a17c8399fb7df438cebf54aa2423c4
#include "xml.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>

void init()
{
	setbuf(stdin, 0);
	setbuf(stdout, 0);
}

void XML::parseXml(const std::string& temp)
{
	if (isParsed) 
	{
		std::cout << "Can not change the file!" << std::endl;
		return;
	}

	isParsed = true;
	v = {temp.begin(), temp.end()};
	v.push_back(0);
	auto node = v.begin();
	parseNode(node);
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


void XML_NODE::parseNodeContents(std::vector<std::string::value_type>::iterator& current)
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
					std::cout << "Unmatch!" << std::endl;
					exit(-1);
				}
				current = gt + 1;
				return;
			}
			else
			{
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
			backup = (char*)malloc(0x50);
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
	this->parseName(current);
	this->parseNodeAttribute(current);

	if (*current == CHARACTACTERS::SLASH && *(current + 1) == CHARACTACTERS::GT)
	{
		current += 2;
		return;
	}

	this->parseNodeContents(current);
}

char* XML_NODE::isInsertable(int x)
{
	if (x > 0x50 || x < 0)
	{
		return nullptr;
	}
	return backup;
}

std::shared_ptr<XML_NODE> pnode(std::shared_ptr<XML_NODE>x, std::string s, std::string name="")
{
	if (x->nodeName == name) return x;

	bool isShowOrFind = true;
	if (name != "") isShowOrFind = false;

	if (isShowOrFind)
	{
		std::cout << s << x->nodeName << "  ";
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

	return nullptr;
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
		if (insertPlace[0] != CHARACTACTERS::UNREMOVABLE)
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

void XML_NODE::meme(char* content)
{
	std::cout << "Useless" << content << std::endl;
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
	init();
	using namespace std;
	int choice = 0;
	XML xmlfile;
	string nodeName, content, xmlContent;
	cout << "Welcome to D^3CTF 2021!" << endl;
	while (1)
	{
		menu();
		cin >> choice;
		switch (choice)
		{
		case 1:
			char temp;
			cout << "Please input file's content" << endl;
			while (read(STDIN_FILENO, &temp, 1) && temp != '\xff')
			{
				xmlContent.push_back(temp);
			}
			xmlfile.parseXml(xmlContent);
			break;
		
		case 2:
			cout << "Please input the node name which you want to edit" << endl;
			cin >> nodeName >> content;
			xmlfile.editXML(nodeName, content);
			break;

		case 3:
			pnode(*xmlfile.node->begin(), "");
			break;

		case 4:
			cout << "MEME" << endl;
			cin >> nodeName;
			if (auto temp = pnode(*xmlfile.node->begin(), "", nodeName)) 
				temp->meme(temp->backup);
			break;

		default:
			break;
		}
	}
}