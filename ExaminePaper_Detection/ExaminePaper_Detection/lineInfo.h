// header for self defined structs
// Author: Weichen Xu
// Date: 2015/6/11
#ifndef BOXBOARD_LINEINFO
#define BOXBOARD_LINEINFO
struct lineInfo{
	int index;
	float length;
	bool operator <(lineInfo &a){
		return (this->length < a.length);
	}
};
struct boxLine{
	int start;
	int count;
	bool operator< (const boxLine &a){
		return (this->start < a.start);
	}
};
#endif