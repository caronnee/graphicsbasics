#include "SurfelRenderer.h"
#include "Debug.h"

SurfelRenderer::SurfelRenderer( const RenderContext & ctx ) : DirectMCRenderer(ctx.scene), Renderer(ctx)
{
  _renderCtx = ctx;
}

bool GPixel = false;
void SurfelRenderer::GenerateToImage(Camera * camera, Image & image)
{
  // create how the world is seen from this perspective. Take surfels as reference
  Vector4d ret(0,0,0,0);
  Vector4d origin = camera->Position();
  Vector4d cameraDir = camera->Direction();
  cameraDir[3] = 0;
 
  int totalSize = image.W()*image.H();
  Surfel ** tempSurfels;
  if (GPixel)
  {
    tempSurfels= new Surfel *[totalSize];
    memset(tempSurfels,0, sizeof(tempSurfels)*totalSize);
  }

  // check all surfels if they are visible from the point
  for ( int i = 0; i < _surfels.size(); i ++)
  {
    Surfel & s = _surfels[i];
    //find the one with the nearest distance in the correct direction
    Vector4d toPoint = s.position - origin;
    float len = toPoint.Size();
    toPoint /= len;
    // check if the position id in the direction of the 
    float cosNormal = -toPoint.Dot(s.normal);
    float cosFactor = toPoint.Dot(cameraDir);
    //if ( s.normal[0] ==-1)
    //  __debugbreak();
    if ( (cosFactor < 0 ) || (cosNormal <= 0) )
      continue;
    Vector4d raster = camera->WorldToViewport(s.position);
    Vector4d testv = _scene->GetCamera()->WorldToViewport(s.position);
    // visible by raster
    if ( (raster.X() < 0) || (raster.Y() < 0) || (raster.X() >= image.W()) || (raster.Y() >= image.H()) )
      continue;
    // find the distance
    HDRComponent & comp = image.GetComponent(raster.X(),raster.Y());

    if ( GPixel)
    {
      Vector4d testv = _scene->GetCamera()->WorldToViewport(s.position);
      if ( !(   (testv.X() < 0) || 
                (testv.Y() < 0) || 
                (testv.X() >= _image->W()) || 
                (testv.Y() >= _image->H()) 
                ))
        _image->AddColor(testv[0],testv[1],Vector4d(100,0,0,0));
    }

    // check if the distance is less
    if ( (comp[3] != 0) && (comp[3] < len) )
      continue;

    int index = (int)raster.Y()*image.W() + (int)raster.X();
    DoAssert( index < totalSize );
    if (GPixel)
      tempSurfels[index] = &s;

    // TODO what if the disk is very very close/big? covers more rasters?
    float projectedSolidAngle = s.area * cosNormal * cosFactor / (len * len);
    comp.Clear();
    comp.Add(s.color * projectedSolidAngle );
    comp[3] = len;
    
    for ( int I =0; I < DIM; I++)
    {
      DoAssert(comp[I] >=0);
    }
    
  }
  if (GPixel)
  {
    for (  int i = 0; i < totalSize; i++)
    {
      if ( !tempSurfels[i])
        continue;
      Surfel & s= *tempSurfels[i];
      Vector4d testv = _scene->GetCamera()->WorldToViewport(s.position);
      if ( !(   (testv.X() < 0) || 
        (testv.Y() < 0) || 
        (testv.X() >= _image->W()) || 
        (testv.Y() >= _image->H()) 
        ))
        _image->AddColor(testv[0],testv[1],Vector4d(0,100,0,0));
    }
    delete[] tempSurfels;
  }
}

int xxx, yyy;

Vector4d SurfelRenderer::RenderPixel(const int &x, const int &y)
{
  xxx = x;
  yyy = y;
  //   return _image->GetComponent(x,y);
  // TODO change toShowSurfels
  bool surfels = false;
  if ( surfels )
  {
    Vector4d ret(0,0,0,0);
    Vector4d pos (x,y,0,1);
    float mindist = 1e36;
    Vector4d cameraPos = _scene->GetCamera()->Position();
    for ( int i =0; i < _surfels.size(); i++ )
    {
      float dist = (cameraPos - _surfels[i].position).Size2();
      // if surfel position matches the one that is X,Y ( at let +- ) and is nearest, show it
      Vector4d raster = _scene->GetCamera()->WorldToViewport( _surfels[i].position);
      Vector4d diff= raster - pos;
      if ( (fabs(diff[0]) <=1) && (fabs(diff[1]) <= 1) && (dist < mindist) )
      {
        mindist = dist;
        ret = _surfels[i].color;
      }
    }
    return ret;
  }
  {
    int u=x,v=y;
    if ( _renderCtx.mask & COORDS_ONLY )
    {
//      u=_renderCtx.fixed[0],v=_renderCtx.fixed[1];
      GPixel = (u == _renderCtx.fixed[0]) && (v == _renderCtx.fixed[1]);
//      GPixel = (u == 80) && (v == 124);
    }
    //u=80,v=124;
    //if ( x & 1 )
    //  u++;
    Ray ray = _renderCtx.scene->GetCamera()->GetRay(u,v);
    Intersection isection;
    if ( _renderCtx.scene->FindIntersection(ray,isection) )
    {
      Vector4d ret = GlobalIllumination(ray,isection);
      if ( GPixel )
        return Vector4d(100,100,100);
      return ret;
    }
  }
  return Vector4d(0,0,0,0);
}

const int SurfelRenderer::coarseSize = 150;

void SurfelRenderer::Bake()
{
  _renderCtx.iterations = 1;
  _surfels.clear();
  // fill surfels
  Geometry * geom;
  int last =0;
  Ray dummyRay;
  Intersection sec;

  for ( int i =0; geom = _renderCtx.scene->Model(i); i++)
  {
    //if ( geom->Type() != TypeSphere)
    //  continue;
    geom->GenerateSurfels(_surfels,20);
    Vector4d vc;
    while (last < _surfels.size())
    {
      Surfel & s = _surfels[last];
      sec.model = s.parent;
      sec.nrm = s.normal;
      sec.nrm.Normalize();
      sec.t = 0;
      sec.worldPosition = s.position;

      for ( int d =0; d<100; d++)
        s.color += SampleLight(dummyRay,sec);

      s.color /= 100;
#if 0 && _DEBUG
      Vector4d vc = _scene->GetCamera()->WorldToViewport( s.position);
#endif
      last++;      
      if ( false )
      {
        Vector4d v = _renderCtx.scene->GetCamera()->WorldToViewport(s.position);
        //    _image->AddColor(v[0],v[1],Vector4d(100,100,100,1));
        _image->AddColor(v[0],v[1],_surfels[i].color);
      }
      // break;
    }
  }
}


const Vector4d SurfelRenderer::GlobalIllumination(const Ray & ray, const Intersection & isec)
{
  // coarse step- world as seen front the intersection point
  CameraContext ctx;
  Matrix4d xyz;
  xyz.CreateFromZ(isec.nrm);
  ctx.axis[0] = xyz.GetRow(2);
  ctx.axis[1] = xyz.GetRow(1);
  ctx.fov = 120;
  ctx.position = -isec.worldPosition;
  ctx.position[3] = 0;
  // some wall resolution
  ctx.resolution[0] = ctx.resolution[1] = SurfelRenderer::coarseSize;

  Camera * CreateCamera( CameraContext & ctx);
  Camera * camera = CreateCamera(ctx);
  //camera = _renderCtx.scene->SwitchCamera(camera);

  Image image;
  image.SetSize(ctx.resolution[0],ctx.resolution[1]);

  GenerateToImage(camera,image);
  if ( GPixel)
  {
    char buffer [128];
    sprintf(buffer,"camerka%d_%d.hdr\0",xxx,yyy);
    image.SaveHdr(buffer);
    //__debugbreak();
  }
#if _DEBUG
  int a, b;
  float mmm = -1;
#endif
  // image is the depth buffer
  // calculate from the depth buffer final image
  Vector4d ret(0,0,0,0);
  Vector4d brdf = isec.model->GetMaterial()->EvalBrdf(Vector4d(0,0,0,0),Vector4d(0,0,0,0),Vector4d(0,0,0,0));
  for ( int i =0; i < image.W(); i++)
  {
    for ( int j = 0; j < image.H(); j++)
    {
      // convert to the position that we can recognize
      Vector4d color = image.GetComponent(i,j);
      // TODO this will only work for diffuse
#if _DEBUG
      float siz = color.Size2();
      if ( siz > mmm )
      {
        mmm = siz;
        a = i;
        b = j;
      }
#endif
      ret += color.MultiplyPerElement(brdf);
    }
  }
#if _DEBUG
  HDRComponent& cmp = image.GetComponent(a,b);
#endif
  return ret;
}