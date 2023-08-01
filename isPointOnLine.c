// A function named line, which determines whether or not
// a point with any given coordinates is on a line segment.
// The first two parameters are the coordinates of the first
// end point of the line segment. The next two parameters
// are the coordinates of the second endpoint. The latter
// are the coordinates of the point which is to be checked
// against the line.
#define between(x, y, z) ((x < y) ? (x <= z) && (z <= y) : (y <= z) && (z <= x))

bool line(int x0, int y0, int x1, int y1, int x, int y)
{
  return (between(x0, x1, x) && between(y0, y1, y)) ? ((x1 - x0) * (y - y0) == (y1 - y0) * (x - x0)) : false;
}
