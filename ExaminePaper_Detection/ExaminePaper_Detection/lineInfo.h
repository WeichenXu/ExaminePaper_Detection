// header for line info 
// Author: Weichen Xu
// Date: 2015/6/11
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