using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

// http://www.emanueleferonato.com/2007/12/03/understanding-flash-displacement-map-filter/

namespace DisplacementFilter
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    static int width = 256;
    static int height = 256;

    uint[] checkerArray;
    uint[] dispArray;

    public MainWindow()
    {
      InitializeComponent();
    }

    uint GetColor(int red, int green, int blue)
    {
      return (uint)( ( red << 16 ) + ( green << 8 ) + ( blue ) );
    }

    private void CreateCheckers(object sender, RoutedEventArgs e)
    {
      const int checkerSize = 64;
      int size = height * width;
      checkerArray = new uint[ size ];

      uint[] color = new uint[2];
      
      color[0] = GetColor( 0, 0, 0 );
      color[1] = GetColor( 255, 255, 255 );

      for ( int i = 0; i < width; i++ )
        for ( int j = 0; j < height; j++ )
        {
          int index = ((j / checkerSize) & 1) == ((i / checkerSize) & 1) ? 0 : 1;
          int iIndex = GetIndex(i,j);
          System.Diagnostics.Debug.Assert(iIndex < size);
          checkerArray[ iIndex ] = color[index];
        }

      WriteableBitmap mmap = new WriteableBitmap(width, height, 96, 96, PixelFormats.Bgr32, null);
      Int32Rect rect = new Int32Rect(0,0,width,height);
      mmap.WritePixels(rect, checkerArray, width * 4, 0);
      Checkers.Source = mmap;

      BitmapImage src = new BitmapImage();
      src.BeginInit();
      src.UriSource = new Uri("../../dispmap.jpg", UriKind.Relative);
      src.CacheOption = BitmapCacheOption.OnLoad;
      src.EndInit();

      //HeightMap.Source = src;
      dispArray = new uint[size];
      mmap = new WriteableBitmap(width, height, 96, 96, PixelFormats.Bgr32, null);
      try
      {
        src.CopyPixels( dispArray, width * 4, 0);
        mmap.WritePixels(rect, dispArray, width * 4, 0);
      }
      catch ( Exception ) 
      {
        System.Diagnostics.Debug.Assert(false);
      }
      HeightMap.Source = mmap;
    }
    double MakeValid(double val, double maxVal)
    {
      if (val < 0)
        return 0;
      if (val >= maxVal)
        return maxVal - 1;
      return val;
    }

    private int GetIndex(int i,int j)
    {
      return i * height + j;
    }

    private Point ApplyFilterStep( int i, int j)
    {
      int xDir = 0; //red
      int yDir = 1; //green

      int xmult = 1;
      int ymult = 0;

      double indexB = GetIndex(i,j);
      uint cX = (dispArray[(int)indexB] >> (8*xDir)) & 0xff;
      System.Diagnostics.Debug.Assert(cX < 256);

      uint cY = (dispArray[(int)indexB] >> (8 * yDir)) & 0xff;
      System.Diagnostics.Debug.Assert(cY < 256);

      float xDisp = xmult * (cX - 128) / 128;
      float yDisp = ymult * (cY - 128) / 128;

      // the new pixel
      int newCoordX = (int)MakeValid(i + xDisp, width);
      int newCoordY = (int)MakeValid(j + yDisp, height);

      return new Point(newCoordX, newCoordY);
      
      //int ret = GetIndex(newCoordX, newCoordY);
      //return ret;
    }

    private void ApplyFilter(object sender, RoutedEventArgs e)
    {
      int pixelSize = 2;
      int pixelSizeHalf = pixelSize / 2;
      WriteableBitmap bitmap = new WriteableBitmap(width * pixelSize, height * pixelSize, 96, 96, PixelFormats.Bgr32, null);
      int size = width * height * pixelSize;      
      uint[] destArray = new uint[size];
      uint defcolor = GetColor(255,0,0);

      for (int i = 0; i < size; i++)
        destArray[i] = defcolor;
      for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++)
        {
          Point p = ApplyFilterStep(i, j);
          Int32Rect rect = new Int32Rect((int)p.X*pixelSize, (int)p.Y*pixelSize, pixelSize, pixelSize);
          int shiftedIndex = GetIndex((int)p.X,(int)p.Y);
          for ( int pi = 0; pi< pixelSize*pixelSize; pi++)
            destArray[pi] = checkerArray[shiftedIndex];
          bitmap.WritePixels(rect, destArray, pixelSize*4, 0);
        }
      Result.Height = height*pixelSize;
      Result.Width = width * pixelSize;
      Result.Source = bitmap;
    }

    private void CheckOrigin(object sender, MouseButtonEventArgs e)
    {
      ////get index
      //System.Windows.IInputElement send = sender as IInputElement;
      //Point p = e.GetPosition(send);
      ////make red dot int he displ. map
      //byte[] resArray = new byte[width * height * 4];
      //try
      //{
      //  HeightMap .SetPixel(0,0,0);   // .CopyPixels(dispArray, width * 4, 0);
      //}
      //catch (Exception exc)
      //{
      //  System.Diagnostics.Debug.Assert(false);
      //}
      //// make vector in the checkers image ( green )
    }
  }
}
