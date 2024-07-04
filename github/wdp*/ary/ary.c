#include "ary.h"
#include <stdio.h>
#include <math.h>

bool czy_zero(double x) {
  return fabs(x) < 1e-10;
}

wartosc minus_zero(wartosc w) {
  if (czy_zero(w.min)) w.min = 0.0;
  if (czy_zero(w.max)) w.max = 0.0;
  return w;
}

wartosc wartosc_dokladnosc(double x, double p) {
  wartosc result;
  result.min = x - (x * p / 100);
  result.max = x + (x * p / 100);
  if (result.max <= result.min) {
    double pomoc = result.min;
    result.min = result.max;
    result.max = pomoc;
  }
  result.dop = 0;
  return result;
}

wartosc wartosc_od_do(double x, double y) {
  wartosc result;
  result.min = x;
  result.max = y;
  result.dop = 0;
  return result;
}

wartosc wartosc_dokladna(double x) {
  wartosc result;
  result.min = x;
  result.max = x;
  result.dop = 0;
  if (czy_zero(x)) {
    result.min = 0;
    result.max = 0;
  }
  return result;
}

bool in_wartosc(wartosc w, double x) {
  if (w.dop == 0) return (w.min <= x && w.max >= x);
  else return (x >= w.max || x <= w.min);
}

double min_wartosc(wartosc w) {
  if (w.dop == 0) return w.min;
  else return - HUGE_VAL;
}

double max_wartosc(wartosc w) {
  if (w.dop == 0) return w.max;
  else return HUGE_VAL;
}

double sr_wartosc(wartosc w) {
  if (isnan(w.min)) return NAN;
  else if (w.dop == 0) {
    if (isinf(w.min) && w.min < 0 && isinf(w.max) && w.max > 0) return NAN;
    if (isinf(w.min) && w.min < 0) return - HUGE_VAL;
    if (isinf(w.max) && w.max > 0) return HUGE_VAL;

    return (w.max + w.min) / 2;
  } else return NAN;
}

wartosc plus(wartosc a, wartosc b) {
  wartosc result;
  if (isnan(a.min) || isnan(b.min)) result = wartosc_od_do(NAN, NAN);
  if (a.dop == 0 && b.dop == 0) result = wartosc_od_do(a.min + b.min, a.max + b.max);
  else if ((a.dop == 1 && b.dop == 0) || (b.dop == 1 && a.dop == 0)) {
    result.min = a.min + b.max;
    result.max = a.max + b.min;
    if (czy_zero(result.min - result.max)) result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);

    else result.dop = 1;
  } else result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
  if (!isnan(result.min)) result = minus_zero(result);
  return result;
}

wartosc minus(wartosc a, wartosc b) {
  wartosc pomoc;
  pomoc.max = - b.min;
  pomoc.min = - b.max;
  pomoc.dop = b.dop;
  wartosc result = plus(a, pomoc);
  if (!isnan(result.min)) result = minus_zero(result);
  return result;
}

wartosc razy(wartosc a, wartosc b) {
  wartosc result;
  if (isnan(a.min) || isnan(b.min)) result = wartosc_od_do(NAN, NAN);
  else if ((czy_zero(b.min) && czy_zero(b.max) && b.dop == 0) 
  || (czy_zero(a.min) && czy_zero(a.max) && a.dop == 0)) result = wartosc_dokladna(0);
  else if (a.dop == 0 && b.dop == 0) {
    double wyniki[4] = {
      a.min * b.min,
      a.min * b.max,
      a.max * b.min,
      a.max * b.max
    };
    result.min = fmin(fmin(wyniki[0], wyniki[1]), fmin(wyniki[2], wyniki[3]));
    result.max = fmax(fmax(wyniki[0], wyniki[1]), fmax(wyniki[2], wyniki[3]));
    result.dop = 0;
  } else if (a.dop == 1 && b.dop == 0) {
    if (b.min >= 0) {
      result.min = fmax(a.min * b.min, a.min * b.max);
      result.max = fmin(a.max * b.min, a.max * b.max);
      result.dop = 1;
      if (result.min >= result.max || czy_zero(result.min - result.max)) result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    } else if (b.max <= 0) {
      result.min = fmax(a.max * b.max, a.max * b.min);
      result.max = fmin(a.min * b.max, a.min * b.min);
      result.dop = 1;
      if (result.min >= result.max || czy_zero(result.min - result.max)) result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    } else {
      result.min = fmax(a.max * b.min, a.min * b.max);
      result.max = fmin(a.max * b.max, a.min * b.min);
      result.dop = 1;
      if (result.min >= result.max || czy_zero(result.min - result.max)) result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    }
  } else if (a.dop == 0 && b.dop == 1) result = razy(b, a);
  else if (a.dop == 1 && b.dop == 1) {
    double g1 = a.min * b.min, g2 = a.max * b.max;
    double d1 = a.min * b.max, d2 = a.max * b.min;
    result.min = fmax(d1, d2);
    result.max = fmin(g1, g2);
    result.dop = 1;
    if (result.min >= result.max || czy_zero(result.min - result.max)) result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
  }
  if (!isnan(result.min)) result = minus_zero(result);
  return result;
}

wartosc podzielic(wartosc a, wartosc b) {
  wartosc result;
  if (isnan(a.min) || isnan(b.min)) result = wartosc_od_do(NAN, NAN);
  else if (czy_zero(b.max) && czy_zero(b.min) && b.dop == 0) result = wartosc_od_do(NAN, NAN);
  else if (a.dop == 0 && b.dop == 0) {
    if (a.max > 0 && b.min <= 0 && a.min < 0 && b.max >= 0) result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    else if (isinf(b.max) && isinf(b.min) && b.min < 0 && b.max > 0) result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    else if (czy_zero(b.min))
    {
      if (a.max < 0) result = wartosc_od_do(-HUGE_VAL, a.max / b.max);
      else if (a.min > 0) result = wartosc_od_do(a.min / b.max, HUGE_VAL);
      else wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    } else if (czy_zero(b.max))
    {
      if (a.max < 0) result = wartosc_od_do(a.max / b.min, HUGE_VAL);
      else if (a.min > 0) result = wartosc_od_do(-HUGE_VAL, a.min / b.min);
      else wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    } else if (b.min > 0 || b.max < 0) {
      double wyniki[4] = {
        a.min / b.min,
        a.min / b.max,
        a.max / b.max,
        a.max / b.min
      };
      result.min = fmin(fmin(wyniki[1], wyniki[2]), fmin(wyniki[3], wyniki[0]));
      result.max = fmax(fmax(wyniki[1], wyniki[2]), fmax(wyniki[3], wyniki[0]));
      result.dop = 0;
    } else {
      double wyniki[4] = {
        a.min / b.min,
        a.min / b.max,
        a.max / b.min,
        a.max / b.max
      };
      double wyniki2[4] = {
        a.min / b.min,
        a.min / b.max,
        a.max / b.min,
        a.max / b.max
      };
      for (int i = 0; i < 4; i++) {
        if (wyniki[i] > 0) wyniki[i] = -HUGE_VAL;
        if (wyniki2[i] < 0) wyniki2[i] = HUGE_VAL;
      }
      result.min = fmax(fmax(wyniki[0], wyniki[1]), fmax(wyniki[2], wyniki[3]));
      result.max = fmin(fmin(wyniki2[0], wyniki2[1]), fmin(wyniki2[2], wyniki2[3]));
      result.dop = 1;
      if ((czy_zero(result.max - result.min)) ||
        (czy_zero(result.max) && czy_zero(result.min)) ||
        (czy_zero(result.max) && czy_zero(result.min))) result.dop = 0;
    }
  } else if (a.dop == 1 && b.dop == 0) {
    result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    result.dop = 1;
    if (b.min < 0) {
      if (result.min < a.max / b.min) result.min = a.max / b.min;
      if (result.max > a.min / b.min) result.max = a.min / b.min;
    } else {
      if (result.min < a.min / b.min) result.min = a.min / b.min;
      if (result.max > a.max / b.min) result.max = a.max / b.min;
    }

    if (b.max < 0) {
      if (result.min < a.max / b.max) result.min = a.max / b.max;
      if (result.max > a.min / b.max) result.max = a.min / b.max;
    } else {
      if (result.min < a.min / b.max) result.min = a.min / b.max;
      if (result.max > a.max / b.max) result.max = a.max / b.max;
    }
    if (result.min >= result.max) result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
  } else if (a.dop == 0 && b.dop == 1) {
    if (czy_zero(b.min)) {
      if (a.min > 0) {
        result.min = -HUGE_VAL;
        double wyniki[4] = {
          a.min / b.min,
          a.max / b.max,
          a.min / b.max,
          a.max / b.min
        };
        for (int i = 0; i < 4; i++)
          if (isinf(wyniki[i]) && wyniki[i] > 0) wyniki[i] = -HUGE_VAL;
        result.max = fmax(fmax(wyniki[2], wyniki[3]), fmax(wyniki[0], wyniki[1]));
        result.dop = 0;
      } else if (a.max < 0) {
        result.max = HUGE_VAL;
        double wyniki[4] = {
          a.min / b.min,
          a.max / b.max,
          a.min / b.max,
          a.max / b.min
        };
        for (int i = 0; i < 4; i++)
          if (isinf(wyniki[i]) && wyniki[i] < 0) wyniki[i] = HUGE_VAL;
        result.min = fmin(fmin(wyniki[2], wyniki[3]), fmin(wyniki[0], wyniki[1]));
      } else result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
      result.dop = 0;
    } else if (czy_zero(b.max)) {
      if (a.min > 0) {
        result.max = HUGE_VAL;
        double wyniki[4] = {
          a.min / b.min,
          a.max / b.max,
          a.min / b.max,
          a.max / b.min
        };
        for (int i = 0; i < 4; i++)
          if (isinf(wyniki[i]) && wyniki[i] < 0) wyniki[i] = HUGE_VAL;
        result.min = fmin(fmin(wyniki[2], wyniki[3]), fmin(wyniki[0], wyniki[1]));
        result.dop = 0;
      } else if (a.max < 0) {
        result.min = -HUGE_VAL;
        double wyniki[4] = {
          a.min / b.min,
          a.max / b.max,
          a.min / b.max,
          a.max / b.min
        };
        for (int i = 0; i < 4; i++)
          if (isinf(wyniki[i]) && wyniki[i] > 0) wyniki[i] = -HUGE_VAL;
        result.max = fmax(fmax(wyniki[2], wyniki[3]), fmax(wyniki[0], wyniki[1]));
        result.dop = 0;
      } else result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    } else if (in_wartosc(b, 0)) {
      if (a.min > 0) {
        result.dop = 1;
        if (b.min > 0) {
          result.min = a.max / b.max;
          result.max = a.min / b.min;
        } else {
          result.min = a.min / b.max;
          result.max = a.max / b.min;
        }
      } else if (a.max < 0) {
        result.dop = 1;
        if (b.min > 0) {
          result.max = a.max / b.max;
          result.min = a.min / b.min;
        } else {
          result.min = a.min / b.max;
          result.max = a.max / b.min;
        }
      } else result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
    } else {
      double wyniki[4] = {
        a.min / b.min,
        a.max / b.max,
        a.min / b.max,
        a.max / b.min
      };
      result.min = fmin(fmin(wyniki[2], wyniki[3]), fmin(wyniki[0], wyniki[1]));
      result.max = fmax(fmax(wyniki[2], wyniki[3]), fmax(wyniki[0], wyniki[1]));
      result.dop = 0;
    }
  } else if (a.dop == 1 && b.dop == 1) result = wartosc_od_do(-HUGE_VAL, HUGE_VAL);
  if (!isnan(result.min)) result = minus_zero(result);
  return result;
}