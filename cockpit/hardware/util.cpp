#include "util.hpp"
#include "ioCards/IOCards.h"

using namespace std;

Util::Util(void)
{
}


Util::~Util(void)
{
}


int Util::low_pass(const int previous_output, const int input)
{
	const auto beta = 0.25;
	const auto filtered  = static_cast<int>(static_cast<double>(previous_output) - (beta * (previous_output - input)));
	//const auto remainder = filtered % 10;
	//const auto rounded = round(remainder / 10.0) * 10;
	const int rounded_input = ((filtered / 10) * 10) + round((filtered % 10) / 10.0) * 10;
	return rounded_input;
//	return static_cast<int>(static_cast<double>(previous_output) - (beta * (previous_output - rounded_input)));
}

vector<string> &Util::split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> Util::split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}




