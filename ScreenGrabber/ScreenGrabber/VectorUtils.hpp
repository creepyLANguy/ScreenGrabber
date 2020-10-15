#pragma once

template <class T>
void AppendToVector(const vector<T>& src, vector<T>& dest)
{
  dest.reserve(dest.size() + src.size());

  for (auto v : src)
  {
    dest.emplace_back(v);
  }
}


template <class T>
void OverwriteVector(const vector<T>& src, vector<T>& dest)
{
  dest.clear();
  AppendToVector(src, dest);
}
