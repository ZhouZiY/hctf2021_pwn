#pragma once

#ifndef  XML_HPP
#define XML_HPP
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <memory>
#include <map>

class XML_NODE;

enum CHARACTACTERS :char
{
	BLANK = ' ',
	LT = '<',
	GT = '>',
	EQUAL = '=',
	QUOT = '\"',
	SLASH = '/',
	NEWLINE='\n',
};
class XML
{
public:
	void parseXml(const std::string& fileName);
	std::shared_ptr<std::vector<std::shared_ptr<XML_NODE>>> node;

protected:
	typedef std::pair<std::string, std::string>      value_type;
	std::vector<std::string::value_type> v;
	bool parseNode(std::vector<std::string::value_type>::iterator& current);
	bool parseElement(std::vector<std::string::value_type>::iterator& current);
	
};


class XML_NODE
{
public:
	std::string nodeName;
	
	void parse(std::vector<std::string::value_type>::iterator& current);
	std::map<std::string, std::string> attribute;
	std::shared_ptr <std::string> data;
	std::shared_ptr<std::vector<std::shared_ptr<XML_NODE>>> node;	
private:
	bool parseName(std::vector<std::string::value_type>::iterator& current);
	void name(std::vector<std::string::value_type>::iterator being, std::vector<std::string::value_type>::iterator end);
	void parseNodeAttribute(std::vector<std::string::value_type>::iterator& current);
	void paeseNodeContents(std::vector<std::string::value_type>::iterator& current);

};


template<typename I>
inline I iterFind(I c, char s = ' ')
{
	auto blank = c;
	
	while (true)
	{
		if (*blank != '\0')
		{
			if (*blank != s)
				++blank;
			else
				break;
		}
		else
			return c;
	}
	return blank;
}
template<typename I>
inline void skip(I& c)
{
	while ((*c == ' ' || *c == '\n') && *c != 0)
		++c;
}
#endif // ! XML_HPP




#ifdef _DEBUG
#include<iostream>
void inline log(std::vector<std::string::value_type>::iterator& current,std::string info)
{
	/*using namespace std;
	cout << "position:\n            ";
	for (size_t i = 0; i < 20&& *current!='\0'; ++i)
	{
		cout << *(current+i );
	}
	cout << "\ninfo:  " << info << endl;;
	*/
}
#else
void inline log(std::vector<std::string::value_type>::iterator& current, std::string info)
{

}
#endif // _DEBUG
