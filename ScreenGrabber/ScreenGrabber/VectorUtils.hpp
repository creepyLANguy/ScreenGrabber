#ifndef VECTORUTILS_H
#define VECTORUTILS_H

template <class T>
void CopyToVector(vector<T>& src, vector<T>& dest)
{
  for (auto v : src)
  {
    dest.push_back(v);
  }
}


template <class T>
void OverwriteVector(vector<T>& src, vector<T>& dest)
{
  dest.clear();
  for (auto v : src)
  {
    dest.push_back(v);
  }
}

#endif
