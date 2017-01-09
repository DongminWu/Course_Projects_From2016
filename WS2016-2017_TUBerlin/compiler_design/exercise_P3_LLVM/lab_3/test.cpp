#include <iostream>
#include <algorithm>

#include <map>
#include <string>
#include <vector>

using namespace std;

int main()
{
    std::map<std::string,int> test1;
    test1["hello"]= 1;
    std::cout<<"test1.hello="<<test1["hello"]<<std::endl;
    




    std::map<std::string,std::vector<int>> test2;
    vector<int> temp;
    temp.push_back(1);
    temp.push_back(2);
    test2.insert(make_pair("hello",temp));
    std::cout<<"test2.hello="<<test2["hello"][0]<<std::endl;


    vector<int> int_list;
    vector<int> iint_list;
    int_list.push_back(1);
    int_list.push_back(2);
    int_list.push_back(3);
    int_list.push_back(4);
    iint_list = int_list;
    for (int i = 0; i<int_list.size();i++)
    {
	cout<<int_list[i]<<" "<<endl;
    }
    cout<<"------"<<endl;
    sort(int_list.begin(),int_list.end());
    
    int_list.erase(remove(int_list.begin(),int_list.end(),3),int_list.end());
    for (int i = 0; i<int_list.size();i++)
    {
	cout<<int_list[i]<<" "<<endl;
    }
    cout<<"---i---"<<endl;
    for (int i = 0; i<iint_list.size();i++)
    {
	cout<<iint_list[i]<<" "<<endl;
    }

    vector<string> string_list;
    vector<string> istring_list;
    string_list.push_back("1");
    string_list.push_back("2");
    string_list.push_back("3");
    string_list.push_back("4");
    istring_list = string_list;
    for (int i = 0; i<string_list.size();i++)
    {
	cout<<string_list[i]<<" "<<endl;
    }
    cout<<"------"<<endl;
    sort(string_list.begin(),string_list.end());
    
    string_list.erase(remove(string_list.begin(),string_list.end(),"3"),string_list.end());
    for (int i = 0; i<string_list.size();i++)
    {
	cout<<string_list[i]<<" "<<endl;
    }
    cout<<"---i---"<<endl;
    for (int i = 0; i<istring_list.size();i++)
    {
	cout<<istring_list[i]<<" "<<endl;
    }
    return 0;

}
