#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

int main()
{
    set<int>a;
    set<int>b;
    set<int>c;
    set<int>d;
    set<int>e;

    a.insert(1);
    a.insert(2);
    a.insert(3);
    a.insert(4);
    a.insert(5);

    b.insert(3);
    b.insert(4);
    b.insert(5);

    set_union(a.begin(), a.end(), b.begin(), b.end(), inserter(c, c.begin()));
    set<int>::iterator s;
    for(s=c.begin(); s!=c.end(); s++)
    {
        cout <<(*s)<<",";
    }
    cout << endl;

    set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(d, d.begin()));
    for(s=d.begin(); s!=d.end(); s++)
    {
        cout <<(*s) <<",";
    }
    cout << endl;

    set_difference(a.begin(), a.end(), b.begin(), b.end(), inserter(e, e.begin()));
    for(s=e.begin(); s!=e.end(); s++)
    {
        cout <<(*s)<<",";
    }
    cout <<endl;

    return 0;
}   
