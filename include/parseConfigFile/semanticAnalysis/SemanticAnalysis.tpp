#include "SemanticAnalysis.hpp"
#include <sstream>
#include <iostream>

template <typename  T>
void SemanticAnalysis::saveMultipleDirectiveValue(std::vector<T> &type) noexcept {
  std::stringstream stream;
  T convert;
  for (auto it = (_tokenLine.begin() + 1); it != _tokenLine.end(); it++) {
    stream.clear();
    stream.str((*it)->_tokenStr);
    stream >> convert;
    type.push_back(convert);
    //std::cout << "Added: " << (*it)->_tokenStr << std::endl;
 }
}
