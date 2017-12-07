// vector::begin/end
#include <iostream>
#include <vector>
#include <algorithm>

int main ()
{
  std::vector<int> myvector;
  std::vector<int>::iterator it;
  for (int i=1; i<=5; i++) myvector.push_back(i);

  it = std::find(myvector.begin(), myvector.end(), 3);
  
  if(it != myvector.end())
	myvector.erase(it);

  std::cout << "myvector contains:";
  for (std::vector<int>::iterator it = myvector.begin() ; it != myvector.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

  return 0;
}

