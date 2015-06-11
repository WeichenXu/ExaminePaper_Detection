// Box board 
// Author: Weichen Xu
// Date: 2015/6/11
#define MAX_IMAGE_SIZE 500
#define MAX_BOX_SIZE 20
struct boxBoard{
	int min_x,max_x,min_y,max_y;
	int gap_x,gap_y;
	int box_x,box_y;
	int X[MAX_IMAGE_SIZE],Y[MAX_IMAGE_SIZE];
	int Position_X[MAX_BOX_SIZE],Position_Y[MAX_BOX_SIZE];
	void init(){
		int i = 0,j=0;
		while(i<MAX_IMAGE_SIZE){
			X[i] = 0;
			Y[i] = 0;
			i++;
		}
		while (j<MAX_BOX_SIZE)
		{
			Position_X[j] = 0;
			Position_Y[j] = 0;
			j++;
		}
	}
};