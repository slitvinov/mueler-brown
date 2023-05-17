#include <math.h>
#include <stdio.h>

static double E(double, double);
static void F(double, double, double *, double *);
static const double xlo = -2, xhi = 2, ylo = -1, yhi = 3;
int main(int argc, char **argv) {
  int i, j, n = 200;
  double h, x, y, eng, u, v;

  h = (xhi - xlo) / n;
  for (j = 0; j < n; j++) {
    y = ylo + h / 2 + h * j;
    for (i = 0; i < n; i++) {
      x = xlo + h / 2 + h * i;
      eng = E(x, y);
      F(x, y, &u, &v);
      fwrite(&eng, sizeof(eng), 1, stdout);
      fwrite(&u, sizeof(u), 1, stdout);
      fwrite(&v, sizeof(v), 1, stdout);
    }
  }
}

static const double A[] = {-200, -100, -170, 15}, a[] = {-1, -1, -6.5, 0.7},
                    b[] = {0, 0, 11, 0.6}, c[] = {-10, -10, -6.5, 0.7},
                    x0[] = {1, 0, -0.5, -1}, y0[] = {0, 0.5, 1.5, 1.0};
static double E(double x, double y) {
  int i;
  double dx, dy, s;
  s = 0;
  for (i = 0; i < sizeof A / sizeof *A; i++) {
    dx = x - x0[i];
    dy = y - y0[i];
    s += A[i] * exp(a[i] * dx * dx + b[i] * dx * dy + c[i] * dy * dy);
  }
  return s;
}
static void F(double x, double y, double *u, double *v) {
  int i;
  double dx, dy, s;
  *u = 0;
  *v = 0;
  for (i = 0; i < sizeof A / sizeof *A; i++) {
    dx = x - x0[i];
    dy = y - y0[i];
    s = A[i] * exp(a[i] * dx * dx + b[i] * dx * dy + c[i] * dy * dy);
    *u += (2 * a[i] * dx + b[i] * dy) * s;
    *v += (b[i] * dx + 2 * c[i] * dy) * s;
  }
}
