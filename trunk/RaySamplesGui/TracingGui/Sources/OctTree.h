#ifndef __OCTTREE__
#define __OCTTREE__

#include "Geometry.h"
#include "Vector.h"

struct OctLeaf 
{
  // 
  Vector4d _range;
  Geometry * _geometry;
  /// oct tree constructor
  OctLeaf( Geometry * geom ) : _geometry(geom) {  }
};

#include <vector>
class OctTree
{
  Vector4d _min;
  Vector4d _max;
  OctTree * _leaves;
  std::vector<Geometry *> _geometries;

  void SimpleAdd(Geometry * geom);

  void Split();

  void Append( Geometry * geom);
  OctTree * FindLeaf(const Vector4d & position)const;
public:
  // constructor
  OctTree();
  ~OctTree();

  void Add( Geometry * geom );

  Geometry * Find( const Ray & ray)
  {
    return NULL;
  }
  void Sort();
};

#endif