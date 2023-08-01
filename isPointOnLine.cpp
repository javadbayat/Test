#define between(x, y, z) ((x < y) ? (x <= z) && (z <= y) : (y <= z) && (z <= x))

bool line(int x0, int y0, int x1, int y1, int x, int y)
{
  return (between(x0, x1, x) && between(y0, y1, y)) ? ((x1 - x0) * (y - y0) == (y1 - y0) * (x - x0)) : false;
}
