#ifndef _HYPERMESH_REGULAR_SIMPLEX_MESH_HH
#define _HYPERMESH_REGULAR_SIMPLEX_MESH_HH

#include <iostream>
#include <vector>
#include <tuple>
#include <set>
#include <algorithm>
#include <numeric>
#include <cassert>

namespace hypermesh {

struct regular_simplex_mesh;

struct regular_simplex_mesh_element {
  friend class regular_simplex_mesh;

  regular_simplex_mesh_element(const regular_simplex_mesh &m, int d); 
  regular_simplex_mesh_element(const regular_simplex_mesh &m, int d, 
      const std::vector<int>& corner, int type); 

  bool operator!=(const regular_simplex_mesh_element& e) const {return !(*this == e);}
  bool operator<(const regular_simplex_mesh_element& e) const;
  bool operator==(const regular_simplex_mesh_element& e) const;
  regular_simplex_mesh_element& operator++();
  friend std::ostream& operator<<(std::ostream& os, const regular_simplex_mesh_element&);

  std::vector<std::vector<int> > vertices() const;

  void increase_corner(int d=0);
  bool valid() const;

  std::vector<regular_simplex_mesh_element> sides() const;
  std::vector<regular_simplex_mesh_element> side_of() const;

  const regular_simplex_mesh &m;
  std::vector<int> corner;
  int dim, type;
};

struct regular_simplex_mesh {
  friend class regular_simplex_mesh_element;
  typedef regular_simplex_mesh_element iterator;

  regular_simplex_mesh(int n) : nd_(n) {
    for (int i = 0; i < n; i ++) {
      lb_.push_back(0);
      ub_.push_back(0);
    }
    initialize_subdivision();
  }
  int nd() const {return nd_;}
  int ntypes(int d) const {return ntypes_.at(d);}
  int n(int d) const; // number of elements

  std::vector<std::vector<int>> unit_simplex(int d, int t) const {return unit_simplices[d][t];}

  void set_lb(const std::vector<int>& lb);
  void set_ub(const std::vector<int>& ub);
  int lb(int d) const {return lb_[d];}
  int ub(int d) const {return ub_[d];}
  const std::vector<int>& lb() const {return lb_;}
  const std::vector<int>& ub() const {return ub_;}

  iterator element_begin(int d);
  iterator element_end(int d);

private: // initialization functions
  void initialize_subdivision();

  // Generate spatial subdivision for the unit n-cube
  // input: dimensionality n
  // output: a vector (n!) of n-simplices; each simplex is a vector of vertices; 
  std::vector<std::vector<std::vector<int>>> subdivide_unit_cube(int n); 

  // Reduce a simplex in the unit cube.  The reduction means to re-encode the simplex with 
  // the corner coordinate (offset) and the node coordinates of the reduced simplex.  
  // For example in a 2-cube, simplex (01,11) can be represented by the cube at (0,1) and 
  // with vertices (00,10)
  // input: a k-dimensional simplex inside the unit cube
  // output: 1) simplex: the sample simplex that is encoded by the 
  //            original cube or another cube. 
  //         2) offset: the offset is all-zero if the simplex can be 
  //            encoded by the original cube, otherwise is the coordinate 
  //            of the cube that uniquely encodes the simplex
  std::tuple<std::vector<std::vector<int>>, std::vector<int>> reduce_unit_simplex(const std::vector<std::vector<int>> &simplex) const;

  // Enumerate all types reduced k-simplices in the n-cube
  std::vector<std::vector<std::vector<int>>> enumerate_unit_simplices(int n, int k);

  // Enumerate all sides of the specific type of k-simplex
  std::vector<std::tuple<int, std::vector<int>>> enumerate_unit_simplex_sides(int k, int type);
 
  // Enumerate all element that contains the specific type of k-simplex
  std::vector<std::tuple<int, std::vector<int>>> enumerate_unit_simplex_side_of(int k, int type);

  // bool is_simplex_identical(const std::vector<std::string>&, const std::vector<std::string>&) const;

private:
  const int nd_;
  std::vector<int> lb_, ub_; // lower and upper bounds of each dimension
  std::vector<int> ntypes_; // number of types for k-simplex

  // list of k-simplices types; each simplex contains k vertices
  std::vector<std::vector<std::vector<std::vector<int>>>> unit_simplices;

  // (dim,type) --> vector of (type,offset)
  std::vector<std::vector<std::vector<std::tuple<int, std::vector<int>>>>> unit_simplex_sides;
  std::vector<std::vector<std::vector<std::tuple<int, std::vector<int>>>>> unit_simplex_side_of;
};



//////////////////////////////////
regular_simplex_mesh_element::regular_simplex_mesh_element(const regular_simplex_mesh &m_, int d_) 
  : m(m_), dim(d_), type(0) 
{
  corner.resize(m.nd());
}

regular_simplex_mesh_element::regular_simplex_mesh_element(
    const regular_simplex_mesh &m_, int d_, 
    const std::vector<int>& corner_, int type_)
  : m(m_), corner(corner_), dim(d_), type(type_)
{
}

bool regular_simplex_mesh_element::operator<(const regular_simplex_mesh_element& e) const
{
  if (corner < e.corner) return true;
  else if (corner == e.corner) return type < e.type;
  else return false;
}

bool regular_simplex_mesh_element::operator==(const regular_simplex_mesh_element& e) const
{
  return dim == e.dim && type == e.type && corner == e.corner;
}

regular_simplex_mesh_element& regular_simplex_mesh_element::operator++() 
{
  if (type + 1 >= m.ntypes(dim)) {
    if (corner == m.ub()) 
      type ++; // the invalid type together with the ub encodes the end of elements
    else {
      type = 0;
      increase_corner();
    }
  } else type ++;
  return *this;
}
  
void regular_simplex_mesh_element::increase_corner(int d)
{
  if (corner[d] + 1 > m.ub(d)) {
    corner[d] = 0;
    increase_corner(d + 1);
  }
  else corner[d] ++;
}
  
bool regular_simplex_mesh_element::valid() const {
  if (type < 0 || type >= m.ntypes(dim)) return false;
  else {
    auto my_vertices = vertices();
    for (int i = 0; i < my_vertices.size(); i ++)
      for (int j = 0; j < my_vertices[i].size(); j ++)
        if (my_vertices[i][j] < m.lb(j) || my_vertices[i][j] > m.ub(j))
          return false;
    return true;
#if 0
    for (int i = 0; i < m.nd(); i ++) 
      if (corner[i] < m.lb(i) || corner[i] > m.ub(i))
        return false;
#endif
  }
}

std::vector<std::vector<int> > regular_simplex_mesh_element::vertices() const
{
  std::vector<std::vector<int>> vertices;

  const auto unit_vertices = m.unit_simplex(dim, type);
  vertices.resize(unit_vertices.size());
  for (size_t i = 0; i < unit_vertices.size(); i ++) {
    for (int j = 0; j < m.nd(); j ++) {
      vertices[i].push_back(corner[j] + unit_vertices[i][j]);
    }
  }

  return vertices;
}
  
std::ostream& operator<<(std::ostream& os, const regular_simplex_mesh_element& e)
{
  os << "dim=" << e.dim << ",cornor={";
  for (size_t i = 0; i < e.m.nd(); i ++)
    if (i < e.corner.size()-1) os << e.corner[i] << ",";
    else os << e.corner[i];
  os << "},type=" << e.type << "/{";

  const auto s_vertices = e.m.unit_simplex(e.dim, e.type);
  for (size_t i = 0; i < s_vertices.size(); i ++) {
    for (size_t j = 0; j < s_vertices[i].size(); j ++)
      os << s_vertices[i][j];
    if (i < s_vertices.size()-1) os << ",";
  }
  os << "},";

  const auto vertices = e.vertices();
  os << "vertices={";
  for (size_t i = 0; i < vertices.size(); i ++) {
    os << "{";
    for (size_t j = 0; j < vertices[i].size(); j ++)
      if (j < vertices[i].size()-1) os << vertices[i][j] << ",";
      else os << vertices[i][j] << "}";;
    if (i < vertices.size()-1) os << ",";
    else os << "},";
  }

  os << "valid=" << e.valid();
 
#if 0
  os << ",";
  const auto sides = e.sides();
  os << "#sides=" << sides.size() << std::endl;

  for (auto side : sides)
    os << side;
#endif

#if 0
  os << ",";
  const auto side_of = e.side_of();
  os << "#side_of=" << side_of.size() << std::endl;

  for (auto e : side_of)
    os << e;
#endif
  
  return os;
}

std::vector<regular_simplex_mesh_element> regular_simplex_mesh_element::sides() const
{
  const auto &unit_simplex_sides = m.unit_simplex_sides[dim][type];
  std::vector<regular_simplex_mesh_element> sides;

  for (auto s : unit_simplex_sides) {
    regular_simplex_mesh_element side(m, dim-1, corner, std::get<0>(s));
    const auto &offset = std::get<1>(s);
    for (int i = 0; i < m.nd(); i ++) 
      side.corner[i] += offset[i];
    sides.push_back(side);
  }

  return sides;
}

std::vector<regular_simplex_mesh_element> regular_simplex_mesh_element::side_of() const
{
  const auto &unit_simplex_side_of = m.unit_simplex_side_of[dim][type];
  std::vector<regular_simplex_mesh_element> side_of;

  for (auto s : unit_simplex_side_of) {
    regular_simplex_mesh_element e(m, dim+1, corner, std::get<0>(s));
    const auto &offset = std::get<1>(s);
    for (int i = 0; i < m.nd(); i ++)
      e.corner[i] += offset[i];
    side_of.push_back(e);
  }

  return side_of;
}

std::vector<std::vector<std::vector<int>>> regular_simplex_mesh::subdivide_unit_cube(int n)
{
  std::vector<std::vector<std::vector<int>>> results;
  if (n == 1) {
    std::vector<std::vector<int>> simplex; 
    std::vector<int> v0, v1;
    v0.push_back(0);
    v1.push_back(1);
    simplex.push_back(v0);
    simplex.push_back(v1);
    results.push_back(simplex);
  } else {
    const auto results0 = subdivide_unit_cube(n-1);
    for (int i = 0; i < n; i ++) {
      for (int j = 0; j < results0.size(); j ++) {
        std::vector<std::vector<int>> simplex;
        for (int k = 0; k < n; k ++) {
          // std::string node = results0[j][k];
          std::vector<int> vertex = results0[j][k];
          // node.insert(i, "0");
          vertex.insert(vertex.begin()+i, 0);
          simplex.push_back(vertex);
        }
        std::vector<int> all_one_vertex(n);
        for (int k = 0; k < n; k ++)
          all_one_vertex[k] = 1;
        // simplex.push_back(std::string(n, '1'));
        simplex.push_back(all_one_vertex);
        results.push_back(simplex);
      }
    }
  }
  return results;
}

std::tuple<std::vector<std::vector<int>>, std::vector<int>>
regular_simplex_mesh::reduce_unit_simplex(const std::vector<std::vector<int>> &simplex_) const
{
  auto simplex = simplex_;
  const size_t nvertices = simplex.size();
  std::vector<int> offset(nd(), 0);
  // std::string offset(nd(), '0');

  for (size_t i = 0; i < nd(); i ++) {
    // check if every coordinate on the i-th dimension is '1'
    bool all_one = true; 
    for (size_t j = 0; j < nvertices; j ++) {
      if (simplex[j][i] == 0) {
        all_one = false;
        break;
      }
    }

    // if every coordinate on the i-th dimension is '1', add the i-th
    // offset by 1 and set the i-th coordinate to '0'
    if (all_one) {
      offset[i] = 1;
      for (size_t j = 0; j < nvertices; j ++) 
        simplex[j][i] = 0;
    }
  }

  return std::make_tuple(simplex, offset);
}

#if 0
int regular_simplex_mesh::simplex_type(const std::vector<std::string>& vertices) const
{
  // TODO: improve performance by adding a lookup table
  const int dim = vertices.size();
  const auto &my_unit_simplices = unit_simplices[dim];

  for (int i = 0; i < my_unit_simplices.size(); i ++) 
    if (vertices == my_unit_simplices[i]) return i;

  std::cerr << "FATAL: input vertices are:" << std::endl;
  for (auto vertex : vertices)
    std::cerr << vertex << "size=" << vertex.size() << std::endl;

  assert(false); // here's the trouble if the corresponding simplex is not found...  This should not happen
  return 0;
}
#endif

#if 0
bool regular_simplex_mesh::is_simplex_identical(const std::vector<std::string>& l, const std::vector<std::string>& r) const 
{
  if (l.size() != r.size()) return false;
  else {
    auto lhs = l, rhs = r;
    for (size_t i = 0; i < l.size(); i ++)
      if (lhs[i] != rhs[i]) return false;
    return true;
  }
}
#endif

std::vector<std::vector<std::vector<int>>> regular_simplex_mesh::enumerate_unit_simplices(int n, int k)
{
  std::set<std::vector<std::vector<int>>> results;
  if (n == k) {
    auto subdivision = subdivide_unit_cube(n);
    for (auto simplex : subdivision) {
      std::sort(simplex.begin(), simplex.end());
      results.insert(simplex);
    }
  } else if (k < n) {
    auto hyper_simplices = enumerate_unit_simplices(n, k+1);
    for (auto hyper_simplex : hyper_simplices) {
      do {
        std::vector<std::vector<int>> simplex = hyper_simplex;
        simplex.resize(hyper_simplex.size() - 1);

        auto [reduced_simplex, offset] = reduce_unit_simplex(simplex); // TODO: take care of offset
        std::sort(reduced_simplex.begin(), reduced_simplex.end());
        results.insert(reduced_simplex);
      } while (std::next_permutation(hyper_simplex.begin(), hyper_simplex.end()));
    }
  }
  
  std::vector<std::vector<std::vector<int>>> results1;
  for (const auto r : results)
    results1.push_back(r);
  return results1;
}

std::vector<std::tuple<int, std::vector<int>>> regular_simplex_mesh::enumerate_unit_simplex_side_of(int k, int type)
{
  std::vector<std::tuple<int, std::vector<int>>> side_of;
  if (k == nd()) return side_of;

  std::set<std::tuple<int, std::vector<int>>> my_side_of;

  std::vector<int> zero_corner(nd(), 0);
  const auto k_simplex = regular_simplex_mesh_element(*this, k, zero_corner, type);
  const auto k_simplex_vertices = k_simplex.vertices();

  int k_plus_one_simplex_type = 0;
  std::vector<int> corner(nd(), -1);
  while (1) {
    const regular_simplex_mesh_element k_plus_one_simplex(*this, k+1, corner, k_plus_one_simplex_type);
    const auto k_plus_one_simplex_vertices = k_plus_one_simplex.vertices();

    // check if (k+1)-simplex contains the given k-simplex.
    if (std::includes(k_plus_one_simplex_vertices.begin(), k_plus_one_simplex_vertices.end(), 
          k_simplex_vertices.begin(), k_simplex_vertices.end())) 
      my_side_of.insert(std::make_tuple(k_plus_one_simplex_type, corner));

    if (k_plus_one_simplex_type == ntypes(k+1) - 1
        && std::all_of(corner.begin(), corner.end(), [](int i){return i == 1;})) break;
    else {
      if (k_plus_one_simplex_type == ntypes(k+1) - 1) {
        int i = 0;
        corner[i] ++;
        while (corner[i] > 1) {
          corner[i] = -1;
          corner[++i] ++;
        }
        k_plus_one_simplex_type = 0;
      } else k_plus_one_simplex_type ++;
    }
  };

  for (const auto i : my_side_of)
    side_of.push_back(i);

  return side_of;
}

std::vector<std::tuple<int, std::vector<int>>> regular_simplex_mesh::enumerate_unit_simplex_sides(int k, int type)
{
  std::vector<std::tuple<int, std::vector<int>>> sides;
  if (k == 0) return sides;

  std::set<std::tuple<int, std::vector<int>>> mysides;

  auto simplex = unit_simplices[k][type]; 
  const auto k_minums_one_simplices = unit_simplices[k-1]; 
    
  do {
    std::vector<std::vector<int>> side = simplex;
    side.resize(simplex.size() - 1);

    auto [reduced_side, offset] = reduce_unit_simplex(side);
    std::sort(reduced_side.begin(), reduced_side.end());

    int k_minums_one_type; // TODO: replace the linear search
    for (k_minums_one_type = 0; k_minums_one_type < k_minums_one_simplices.size(); k_minums_one_type ++) {
      if (reduced_side == k_minums_one_simplices[k_minums_one_type]) {
        // fprintf(stderr, "adding side, type=%d\n", k_minums_one_type);
        // sides.push_back(std::make_tuple(k_minums_one_type, offset));
        mysides.insert(std::make_tuple(k_minums_one_type, offset));
      }
    }

  } while (std::next_permutation(simplex.begin(), simplex.end())); // TODO: replace the permutation with combination

  for (const auto s : mysides)
    sides.push_back(s);

  // fprintf(stderr, "k=%d, type=%d, #sides=%lu\n", k, type, sides.size());
  // assert(sides.size() > 0);
  return sides;
}

void regular_simplex_mesh::initialize_subdivision()
{
  ntypes_.resize(nd() + 1);
  unit_simplices.resize(nd() + 1);

  for (int k = 0; k <= nd(); k ++) {
    unit_simplices[k] = enumerate_unit_simplices(nd(), k);
    ntypes_[k] = unit_simplices[k].size();
    // fprintf(stderr, "k=%d, count=%d\n", k, ntypes(k));
#if 0
    for (const auto s : unit_simplices[k]) {
      for (const auto v : s)
        std::cerr << v << " ";
      std::cerr << std::endl;
    }
#endif
  }

  unit_simplex_sides.resize(nd()+1);
  for (int dim = 0; dim <= nd(); dim ++) {
    unit_simplex_sides[dim].resize(ntypes(dim));
    for (int type = 0; type < ntypes(dim); type ++) 
      unit_simplex_sides[dim][type] = enumerate_unit_simplex_sides(dim, type);
  }

  unit_simplex_side_of.resize(nd()+1);
  for (int dim = 0; dim <= nd(); dim ++) {
    unit_simplex_side_of[dim].resize(ntypes(dim));
    for (int type = 0; type < ntypes(dim); type ++) 
      unit_simplex_side_of[dim][type] = enumerate_unit_simplex_side_of(dim, type);
  }
  // enumerate_unit_simplex_side_of(0, 0);
}

void regular_simplex_mesh::set_lb(const std::vector<int>& b)
{
  lb_.resize(nd());
  for (int i = 0; i < std::min(lb_.size(), b.size()); i ++)
    lb_[i] = b[i];
}

void regular_simplex_mesh::set_ub(const std::vector<int>& b)
{
  ub_.resize(nd());
  for (int i = 0; i < std::min(ub_.size(), b.size()); i ++)
    ub_[i] = b[i];
}

regular_simplex_mesh::iterator regular_simplex_mesh::element_begin(int d)
{
  regular_simplex_mesh_element e(*this, d);
  e.corner = lb();
  return e;
}

regular_simplex_mesh::iterator regular_simplex_mesh::element_end(int d)
{
  regular_simplex_mesh_element e(*this, d);
  e.corner = ub();
  // e.type = ntypes(d) - 1;
  e.type = ntypes(d); // the invalid type id combines with the ub corner encodes the end.
  return e;
}

}

#endif
