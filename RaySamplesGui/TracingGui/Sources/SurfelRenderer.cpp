#include "SurfelRenderer.h"

SurfelRenderer::SurfelRenderer( const RenderContext & ctx ) : DirectMCRenderer(ctx.scene), Renderer(ctx)
{
  _renderCtx = ctx;
}

void SurfelRenderer::Init(Image * image)
{
  _image = image;
  _image->SetSize(coarseSize,coarseSize);
}

void SurfelRenderer::test()
{
  // create how the world is seen from this perspective. Take surfels as reference
  Vector4d ret(0,0,0,0);
  Vector4d origin = _renderCtx.scene->GetCamera()->Position();
  Vector4d cameraDir = _renderCtx.scene->GetCamera()->Direction();
  for ( int i =0; i < _surfels.size(); i ++)
  {
    //find the one with the nearest distance in the correct direction

    Vector4d toPoint = _surfels[i].position - origin;
    // check if the position id in the direction of the 
    float cosFactor = toPoint.Dot(_surfels[i].normal);
    if ( (toPoint.Dot(cameraDir) < 0 ) || (cosFactor < 0) )
      continue;
    Vector4d raster = _renderCtx.scene->GetCamera()->WorldToRaster(_surfels[i].position);
    // visible by raster
    if ( (raster.X() < 0) || (raster.Y() < 0) || (raster.X() >= _image->W()) || (raster.Y() >= _image->H()) )
      continue;
    // find the distance
    float len = toPoint.Size2();
    HDRComponent & comp = _image->GetComponent(raster.X(),raster.Y());

    // check if the distance is less
    if ( (comp[3] != 0) && (comp[3] < len) )
      continue;

    // TODO what if the disk is very very close/big? covers more rasters?
    float projectedSolidAngle = _surfels[i].area * cosFactor / (len * len);
    comp = _surfels[i].color * projectedSolidAngle;
    comp[3] = len;
  }
}

Vector4d SurfelRenderer::RenderPixel(const int &x, const int &y)
{
  // TODO change toShowSurfels
  if ( true )
  {
    Vector4d ret(0,0,0,0);
    Vector4d pos (x,y,0,1);
    float mindist = 1e36;
    Vector4d cameraPos = _scene->GetCamera()->Position();
    for ( int i =0; i < _surfels.size(); i++ )
    {
      float dist = (cameraPos - _surfels[i].position).Size2();
      // if surfel position matches the one that is X,Y ( at let +- ) and is nearest, show it
      Vector4d raster = _scene->GetCamera()->WorldToRaster( _surfels[i].position);
      Vector4d diff= raster - pos;
      if ( (fabs(raster[0])) < 0.5 && (fabs(raster[1]) < 0.5) && dist < mindist)
      {
        mindist = dist;
        ret = _surfels[i].color;
      }

    }
  }
  return Vector4d(0,0,0,0);
}

const int SurfelRenderer::coarseSize = 15;

void SurfelRenderer::Bake()
{
  _renderCtx.iterations = 1;
  _surfels.clear();
  // fill surfels
  Geometry * geom;
  for ( int i =0; geom = _renderCtx.scene->Model(i); i++)
  {
    geom->GenerateSurfels(_surfels,20);
  }

  // todo will not work for glossy
  Ray dummyRay;
  Intersection sec;
  // bake direct illumination for each surfel
  for ( int i =0; i < _surfels.size(); i++)
  {
    sec.model = _surfels[i].parent;
    sec.nrm = _surfels[i].normal;
    sec.t = 0;
    sec.worldPosition = _surfels[i].position;
    _surfels[i].color = SampleLight(dummyRay,sec);
  }
}

const Vector4d SurfelRenderer::SampleGlobalIllumination(const Ray & ray, const Intersection & isec)
{
  // coarse step- world as seen front the intersection point
  CameraContext ctx;
  Matrix4d xyz;
  xyz.CreateFromZ(isec.nrm);
  ctx.axis[0] = xyz.GetRow(2);
  ctx.axis[1] = xyz.GetRow(1);
  ctx.fov = 90;
  ctx.position = isec.worldPosition;
  // some wall resolution
  ctx.resolution[0] = ctx.resolution[1] = SurfelRenderer::coarseSize;

  Camera * CreateCamera( CameraContext & ctx);
  Camera * camera = CreateCamera(ctx);
  camera = _renderCtx.scene->SwitchCamera(camera);
  Image image;
  //Renderer * render = new SurfelRenderer(_surfels,_renderCtx);
  //render->Init(&image);
  //render->Bake();
  camera = _renderCtx.scene->SwitchCamera(camera);
  //  render->Render();

  delete camera;
  // image is the depth buffer
  // calculate from the depth buffer final image
  Vector4d ret(0,0,0,0);
  for ( int i =0; i < image.W(); i++)
  {
    for ( int j =0; j < image.H(); j++)
    {
      // convert to the position that we can recognize
      Vector4d color = image.GetComponent(i,j);
      // TODO this will only work for diffuse
      Vector4d brdf = isec.model->GetMaterial()->EvalBrdf(Vector4d(0,0,0,0),Vector4d(0,0,0,0),Vector4d(0,0,0,0));
      ret += color.MultiplyPerElement(brdf);
    }
  }
  return ret;
}