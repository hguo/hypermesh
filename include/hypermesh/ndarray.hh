#ifndef _HYPERMESH_ARRAY_HH
#define _HYPERMESH_ARRAY_HH

#include <vector>
#include <array>

namespace hypermesh {

template <int N, typename T, typename I=size_t>
struct ndarray {
  ndarray() {dims.fill(0); s.fill(0);}
  ndarray(const std::array<I, N> &dims) {resize(dims);}

  const T* data() const {return p.data();}
  T* data() {return p.data();}

  void resize(const std::array<I, N> &dims_) {
    dims = dims_;
    for (I i = 0; i < N; i ++)
      if (i == 0) s[i] = 1;
      else s[i] = s[i-1]*dims[i-1];
    p.resize(s[N-1]*dims[N-1]);
  }

  template <typename T1, typename I1>
  void resize(const ndarray<N, T1, I1>& a) {
    resize(a.dims);
  }

  I index(const std::array<I, N>& idx) const {
    I i(idx[0]);
    for (I j = 1; j < N; j ++)
      i += idx[j] * s[j];
    return i;
  }

  I& at(const std::array<I, N>& idx) {return p[index(idx)];}
  const I& at(const std::array<I, N>& idx) const {return p[index(idx)];}

  T& at(I i0) {return p[i0];}
  T& at(I i0, I i1) {return p[i0+i1*s[1]];}
  T& at(I i0, I i1, I i2) {return p[i0+i1*s[1]+i2*s[2]];}
  T& at(I i0, I i1, I i2, I i3) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]];}
  T& at(I i0, I i1, I i2, I i3, I i4) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]];}
  T& at(I i0, I i1, I i2, I i3, I i4, I i5) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]];}
  T& at(I i0, I i1, I i2, I i3, I i4, I i5, I i6) {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]+i6*s[6]];}
  
  const T& at(I i0) const {return p[i0];}
  const T& at(I i0, I i1) const {return p[i0+i1*s[1]];}
  const T& at(I i0, I i1, I i2) const {return p[i0+i1*s[1]+i2*s[2]];}
  const T& at(I i0, I i1, I i2, I i3) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]];}
  const T& at(I i0, I i1, I i2, I i3, I i4) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]];}
  const T& at(I i0, I i1, I i2, I i3, I i4, I i5) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]];}
  const T& at(I i0, I i1, I i2, I i3, I i4, I i5, I i6) const {return p[i0+i1*s[1]+i2*s[2]+i3*s[3]+i4*s[4]+i5*s[5]+i6*s[6]];}

public:
  std::array<I, N> dims, s;
  std::vector<T> p;
};

}

#endif
