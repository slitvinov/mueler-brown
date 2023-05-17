#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static double E(double, double);
static void F(double, double, double *, double *);

int main(int argc, char **argv) {
  long n, i, j;
  int NFlag, DumpVelocity, DtFlag, PeriodFlag, period;
  double h, x, y, eng, u, v, xlo, xhi, ylo, dt, gamma, vx, vy, rx, ry, fx, fy,
      kT, coeff;
  gsl_rng *rnd;
  char *end;
  NFlag = 0;
  DtFlag = 0;
  DumpVelocity = 0;
  PeriodFlag = 0;
  while (*++argv != NULL && argv[0][0] == '-')
    switch (argv[0][1]) {
    case 'h':
      fprintf(stderr, "Usage: mueler-brown [-d] -n <number of steps>\n"
                      "Options:\n"
                      "  -d     Dump V, dVx, and dVy on a grid to stdout as a "
                      "binary file and exit\n");
      exit(1);
    case 'n':
      argv++;
      if (*argv == NULL) {
        fprintf(stderr, "mueler-brown: -n needs an argument\n");
        exit(1);
      }
      n = strtol(*argv, &end, 10);
      if (*end != '\0' || n <= 0) {
        fprintf(stderr, "mueler-brown: '%s' is not a positive integer\n",
                *argv);
        exit(1);
      }
      NFlag = 1;
      break;
    case 't':
      argv++;
      if (*argv == NULL) {
        fprintf(stderr, "mueler-brown: -t needs an argument\n");
        exit(1);
      }
      dt = strtod(*argv, &end);
      if (*end != '\0') {
        fprintf(stderr, "mueler-brown: '%s' is not a number\n", *argv);
        exit(1);
      }
      DtFlag = 1;
      break;
    case 'p':
      argv++;
      if (*argv == NULL) {
        fprintf(stderr, "mueler-brown: -p needs an argument\n");
        exit(1);
      }
      period = strtol(*argv, &end, 10);
      if (*end != '\0' || period <= 0) {
        fprintf(stderr, "mueler-brown: '%s' is not a positive integer\n",
                *argv);
        exit(1);
      }
      PeriodFlag = 1;
      break;
    case 'v':
      DumpVelocity = 1;
      break;
    case 'd':
      xlo = -2;
      xhi = 2;
      ylo = -1;
      n = 200;
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
      fprintf(stderr, "mueler-brown: [origin] spacing: [%.16e %.16e] %.16e \n",
              xlo, ylo, h);
      exit(0);
    default:
      fprintf(stderr, "mueler-brown: unknown option '%s'\n", *argv);
      exit(1);
    }
  if (NFlag == 0) {
    fprintf(stderr, "mueler-brown: -n is not set\n");
    exit(1);
  }
  if (DtFlag == 0) {
    fprintf(stderr, "mueler-brown: -t is not set\n");
    exit(1);
  }
  if (PeriodFlag == 0) {
    fprintf(stderr, "mueler-brown: -p is not set\n");
    exit(1);
  }
  gsl_rng_env_setup();
  rnd = gsl_rng_alloc(gsl_rng_default);
  kT = 15;
  gamma = 1;
  x = 0.5;
  y = 0.0;
  vx = 0;
  vy = 0;
  coeff = sqrt(2 * kT * gamma * dt);
  j = 0;
  for (i = 0;; i++) {
    if (j == 0 || j == period) {
      fwrite(&x, sizeof(x), 1, stdout);
      fwrite(&y, sizeof(y), 1, stdout);
      if (DumpVelocity) {
        fwrite(&vx, sizeof(x), 1, stdout);
        fwrite(&vy, sizeof(y), 1, stdout);
      }
      if (i + period >= n)
        break;
      j = 0;
    }
    j++;

    F(x, y, &fx, &fy);
    fx = fy = 0;
    rx = gsl_ran_gaussian(rnd, 1);
    ry = gsl_ran_gaussian(rnd, 1);

    vx += dt * (-fx - gamma * vx) + coeff * rx;
    vy += dt * (-fy - gamma * vy) + coeff * ry;
    x += dt * vx;
    y += dt * vy;
  }
  gsl_rng_free(rnd);
}

static const double A[] = {-200, -100, -170, 15}, a[] = {-1, -1, -6.5, 0.7},
                    b[] = {0, 0, 11, 0.6}, c[] = {-10, -10, -6.5, 0.7},
                    x0[] = {1, 0, -0.5, -1}, y0[] = {0, 0.5, 1.5, 1.0};
static double E(double x, double y) {
  size_t i;
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
  size_t i;
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
