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

    static int xDir = 0; //red
    static int yDir = 1; //green

    const int pixelSize = 2;
    Line dispLine;

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

    private WriteableBitmap GetMap(ref uint[] pixels, int w, int h)
    {
      WriteableBitmap mmap = new WriteableBitmap(w, h, 96, 96, PixelFormats.Bgr32, null);
      Int32Rect rect = new Int32Rect(0,0,w,h);
      mmap.WritePixels(rect, pixels, w * 4, 0);
      return mmap;
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

      Checkers.Source = GetMap(ref checkerArray,width,height);

      BitmapImage src = new BitmapImage();
      src.BeginInit();
      src.UriSource = new Uri("../../dispmap.jpg", UriKind.Relative);
      src.CacheOption = BitmapCacheOption.OnLoad;
      src.EndInit();

      //HeightMap.Source = src;
      dispArray = new uint[size];
      try
      {
        src.CopyPixels( dispArray, width * 4, 0);
      }
      catch ( Exception ) 
      {
        System.Diagnostics.Debug.Assert(false);
      }
      HeightMap.Source = GetMap(ref dispArray, width, height);
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

    Point GetDisplacementVector( int i, int j )
    {
      Point ret = new Point();

      double indexB = GetIndex(i, j);
      uint cX = (dispArray[(int)indexB] >> (8 * xDir)) & 0xff;
      System.Diagnostics.Debug.Assert(cX < 256);
      ret.X = ((double)cX -128)/128;
      
      uint cY = (dispArray[(int)indexB] >> (8 * yDir)) & 0xff;
      System.Diagnostics.Debug.Assert(cY < 256);
      ret.Y = ((double)cY - 128)/128;

      return ret;
    }
    private Point ApplyFilterStep( int i, int j)
    {
      int xmult = 15;
      int ymult = 50;

      Point col = GetDisplacementVector(i, j);

      double xDisp = xmult * col.X;
      double yDisp = ymult * col.Y;

      // the new pixel
      int newCoordX = (int)MakeValid(i + xDisp, width);
      int newCoordY = (int)MakeValid(j + yDisp, height);

      return new Point(newCoordX, newCoordY);
    }

    private void ApplyFilter(object sender, RoutedEventArgs e)
    {
      WriteableBitmap bitmap = new WriteableBitmap(width * pixelSize, height * pixelSize, 96, 96, PixelFormats.Bgr32, null);
      int size = width * height * pixelSize;
      uint[] destArray = new uint[size];
      uint defcolor = GetColor(255, 0, 0);

      for (int i = 0; i < size; i++)
        destArray[i] = defcolor;
      for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++)
        {
          Point p = ApplyFilterStep(i,j);
          Int32Rect rect = new Int32Rect(i * pixelSize, j * pixelSize, pixelSize, pixelSize);
          int shiftedIndex = GetIndex((int)p.X, (int)p.Y);
          for (int pi = 0; pi < pixelSize * 4; pi++)
            destArray[pi] = checkerArray[shiftedIndex];
          bitmap.WritePixels(rect, destArray, pixelSize * 4, 0);
        }
      Result.Height = height * pixelSize;
      Result.Width = width * pixelSize;
      Result.Source = bitmap;
    }

    private void CheckOrigin(object sender, MouseButtonEventArgs e)
    {
      //get index
      System.Windows.IInputElement send = sender as IInputElement;
      //make opposite dot int he displ. map
      WriteableBitmap bitmap = HeightMap.Source as WriteableBitmap;
      if (bitmap == null)
      {
        System.Diagnostics.Debug.Assert(false);
        return;
      }
      // reset checks array and displ array
      int index = 0;

      try
      {
        Point p = e.GetPosition(send);
        p = new Point(p.X / pixelSize, p.Y / pixelSize);
        Point newIndex = ApplyFilterStep((int)p.X, (int)p.Y);

        Checkers.Source = GetMap(ref checkerArray, width, height);
        HeightMap.Source = GetMap(ref dispArray, width, height);
        int colorsSize = pixelSize * 4;
        uint[] colors = new uint[colorsSize];
        for (int i = 0; i < colorsSize; i++)
          colors[i] = GetColor(255, 0, 0);
        // make vector in the checkers image ( one special color )
        WriteableBitmap checkersSource = GetMap(ref checkerArray, width, height);
        Int32Rect rectSource = new Int32Rect((int)p.X, (int)p.Y, 1, 1);
        checkersSource.WritePixels(rectSource, colors, 4, 0);
        Checkers.Source = checkersSource;

        if (dispLine == null)
        {
          dispLine = new Line();
          dispLine.StrokeThickness = 4;
          Color color = new Color();
          color.A = 0xFF;
          color.R = 0xDA;
          color.G = 0x79;
          color.B = 0x6F;
          dispLine.Stroke = new SolidColorBrush(color);

          CheckerCanvas.Children.Add(dispLine);
        }

        dispLine.X1 = p.X;
        dispLine.X2 = newIndex.X;
        dispLine.Y1 = p.Y;
        dispLine.Y2 = newIndex.Y;

        WriteableBitmap heightMapSource = GetMap(ref dispArray, width, height);
        heightMapSource.WritePixels(rectSource, colors, 4, 0);
        HeightMap.Source = heightMapSource;

        Int32Rect rectRes = new Int32Rect((int)p.X * pixelSize, (int)p.Y * pixelSize, pixelSize, pixelSize);
        WriteableBitmap destSource = Result.Source as WriteableBitmap;
        int size = (int)ShowColor.Width * (int)ShowColor.Height;
        uint[] pixels = new uint[size];
        int pixelSize2 = pixelSize * pixelSize;
        for (; index < size; index += pixelSize2)
          destSource.CopyPixels(rectRes, pixels, pixelSize * 4, index);
        {
          uint cCol = 0;
          Point p2 = ApplyFilterStep(120, 5);
          Int32Rect rect = new Int32Rect((int)p.X * pixelSize, (int)p.Y * pixelSize, pixelSize, pixelSize);
          int shiftedIndex = GetIndex((int)p2.X, (int)p2.Y);
          cCol = checkerArray[shiftedIndex];
        }
        ShowColor.Source = GetMap(ref pixels, (int)ShowColor.Width, (int)ShowColor.Height);
        destSource.WritePixels(rectRes, colors, pixelSize * 4, 0);
        Point col = GetDisplacementVector((int)p.X, (int)p.Y);
        Info.Content = string.Format("xDir -> {0} , yDir -> {1}. Index: {2}, {3}", col.X, col.Y, (int)p.X, (int)p.Y);
      }
      catch (Exception ex)
      {
        Info.Content = ex.ToString();
      }

    }
  }
}
