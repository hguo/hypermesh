#ifndef _HYPERMESH_ARRAY_HH
#define _HYPERMESH_ARRAY_HH

#include <vector>
#include <array>

namespace hypermesh {

template <typename T>
struct ndarray {
  ndarray() {}
  ndarray(const std::vector<size_t> &dims) {reshape(dims);}

  size_t nd() const {return dims.size();}
  size_t dim(size_t i) const {return dims[i];}
  std::vector<size_t> shape() const {return dims;}

  const T* data() const {return p.data();}
  T* data() {return p.data();}

  void reshape(const std::vector<size_t> &dims_) {
    dims = dims_;
    s.resize(dims.size());

    for (size_t i = 0; i < nd(); i ++)
      if (i == 0) s[i] = 1;
      else s[i] = s[i-1]*dims[i-1];

    p.resize(s[nd()-1]*dims[nd()-1]);
  }

  void reshape(const std::vector<size_t> &dims, T val) {
    reshape(dims);
    std::fill(p.begin(), p.end(), val);
  }

  size_t index(const std::vector<size_t>& idx) const {
    size_t i(idx[0]);
    for (size_t j = 1; j < nd(); j ++)
      i += idx[j] * s[j];
    return i;
  }

  T& at(const std::vector<size_t>& idx) {return p[index(idx)];}
  const T& at(const std::vector<size_t>& idx) const {return p[index(idx)];}

  T& at(size_t i0) {return p[i0];}
  T& at(size_t i0, size_t i1) {return p[i0+i1*s[1]];}
  T& at(size_t i0, size_t i1, size_t i2) {return p[i0+i1*s[1]+i2*s[2]];}
  T& at(size_t i0, size_t i1, size_t i2, size_t i3) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]];}
  T& at(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]];}
  T& at(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]];}
  T& at(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, size_t i6) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]+i6*s[6]];}
  
  const T& at(size_t i0) const {return p[i0];}
  const T& at(size_t i0, size_t i1) const {return p[i0+i1*s[1]];}
  const T& at(size_t i0, size_t i1, size_t i2) const {return p[i0+i1*s[1]+i2*s[2]];}
  const T& at(size_t i0, size_t i1, size_t i2, size_t i3) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]];}
  const T& at(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]];}
  const T& at(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]];}
  const T& at(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, size_t i6) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]+i6*s[6]];}
  
  T& operator()(size_t i0) {return p[i0];}
  T& operator()(size_t i0, size_t i1) {return p[i0+i1*s[1]];}
  T& operator()(size_t i0, size_t i1, size_t i2) {return p[i0+i1*s[1]+i2*s[2]];}
  T& operator()(size_t i0, size_t i1, size_t i2, size_t i3) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]];}
  T& operator()(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]];}
  T& operator()(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]];}
  T& operator()(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, size_t i6) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]+i6*s[6]];}
  
  const T& operator()(size_t i0) const {return p[i0];}
  const T& operator()(size_t i0, size_t i1) const {return p[i0+i1*s[1]];}
  const T& operator()(size_t i0, size_t i1, size_t i2) const {return p[i0+i1*s[1]+i2*s[2]];}
  const T& operator()(size_t i0, size_t i1, size_t i2, size_t i3) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]];}
  const T& operator()(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]];}
  const T& operator()(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]];}
  const T& operator()(size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, size_t i6) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]+i6*s[6]];}

  T& operator[](size_t i) {return p[i];}
  const T& operator[](size_t i) const {return p[i];}

private:
  std::vector<size_t> dims, s;
  std::vector<T> p;
};

}

#endif
