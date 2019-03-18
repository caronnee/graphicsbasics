#include "OctTree.h"


OctTree::OctTree()
{
  _leaves = NULL;
}

OctTree::~OctTree()
{
  if (_leaves)
    delete[] _leaves;
  _leaves = NULL;
  _geometries.clear();
}

void OctTree::Add(Geometry * geom)
{
  OctTree * cont = _leaves? FindLeaf( geom->Position() ): this;
  cont->Append( geom );
}

void OctTree::Append(Geometry * geom)
{
  SimpleAdd(geom);
  if ( _geometries.size() >= 16 )
    Split();
}

void OctTree::Sort()
{

}
void OctTree::Split()
{
  _leaves = new OctTree[8];
  Sort();
  for ( int i =0; i < _geometries.size(); i++)
    _leaves[i/2].SimpleAdd(_geometries[i]);
  _geometries.clear();
}

void OctTree::SimpleAdd(Geometry * geom)
{
  _min.AA(geom->Position());
  _max.BB(geom->Position());
  _geometries.push_back(geom);
}

OctTree * OctTree::FindLeaf(const Vector4d & position) const
{
  //looking for x
  for ( int i =0; i < 8;i++)
  {

  }
  return NULL;
}
