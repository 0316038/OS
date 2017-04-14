#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
#include <list>

using namespace std;

int main()
{
	cout << "FIFO---" << endl;
	cout << "size\tmiss\thit\t\tpage fault ratio" << endl;
	for(int frame=64;frame<=512;frame*=2){
		fstream ifile;
		ifile.open("trace.txt",ios::in);
		if(!ifile){
			cout << "Cannot open file!" << endl;
		}

		list <string> list_LRU;
		list <string>::iterator it;
		map <string,list<string>::iterator> map_LRU;
		map <string,list<string>::iterator>::iterator iter;
		string op_code,address,pattern;
		int hit=0,miss=0;
		double ratio=0;
		while(ifile >> op_code){
			ifile >> address;
			pattern="";
			for(int i=0;i<5;i++){
				pattern+=address[i];				//取address的前5個bit
			}
			iter = map_LRU.find(pattern);
			if(iter != map_LRU.end()){				//hit
				hit++;
			}
			else if(iter == map_LRU.end()){			//miss
				if(list_LRU.size() == frame){		//frame is full
					string temp = list_LRU.front();	//存取list的第一個元素(first in)
					list_LRU.pop_front();			//刪除list的第一個元素(first in)
					map_LRU.erase(temp);			//把first in元素從map中刪除
				}
				list_LRU.push_back(pattern);		//將pattern插入list的尾巴
				it = list_LRU.end();
				it--;								//it指向新加入的pattern
				map_LRU[pattern] = it;				//用array方式插入一個元素到map中
				//map_LRU.insert(pair<string,list<string>::iterator>(pattern,it));	//用insert函數插入pair
				miss++;
			}
		}
		ratio = (double)miss/(miss+hit);
		cout << frame << "\t" << miss << "\t" << hit << "\t" << ratio << endl;
		ifile.close();
	}




	cout << "LRU---" << endl;
	cout << "size\tmiss\thit\t\tpage fault ratio" << endl;
	for(int frame=64;frame<=512;frame*=2){
		fstream ifile;
		ifile.open("trace.txt",ios::in);
		if(!ifile){
			cout << "Cannot open file!" << endl;
		}

		list <string> list_LRU;
		list <string>::iterator it;
		map <string,list<string>::iterator> map_LRU;
		map <string,list<string>::iterator>::iterator iter;
		string op_code,address,pattern;
		int hit=0,miss=0;
		double ratio=0;
		while(ifile >> op_code){
			ifile >> address;
			pattern="";
			for(int i=0;i<5;i++){
				pattern+=address[i];				//取address的前5個bit
			}
			iter = map_LRU.find(pattern);
			if(iter != map_LRU.end()){				//hit
				list_LRU.erase(iter->second);
				list_LRU.push_back(pattern);		//把hit pattern移到list的尾巴(MRU)
				it = list_LRU.end();
				it--;							//it指向hit pattern
				map_LRU[pattern] = it;
				hit++;
			}
			else if(iter == map_LRU.end()){			//miss
				if(list_LRU.size() == frame){		//frame is full
					string temp = list_LRU.front();	//存取list的第一個元素(LRU)
					list_LRU.pop_front();			//刪除list的第一個元素(LRU)
					map_LRU.erase(temp);			//把LRU從map中刪除
				}
				list_LRU.push_back(pattern);		//將pattern插入list的尾巴
				it = list_LRU.end();
				it--;								//it指向新加入的pattern
				map_LRU[pattern] = it;				//用array方式插入一個元素到map中
				//map_LRU.insert(pair<string,list<string>::iterator>(pattern,it));	//用insert函數插入pair
				miss++;
			}
		}
		ratio = (double)miss/(miss+hit);
		cout << frame << "\t" << miss << "\t" << hit << "\t" << ratio << endl;
		ifile.close();
	}
}
