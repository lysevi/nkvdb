#pragma once

namespace utils {
template <class ItT, class T, class _Pr, class _Dt>
ItT find_begin(const ItT begin, const ItT end, const T &val, _Pr pred,
               _Dt delta) {
  ItT low = begin;
  ItT high = end - 1;

  while (pred(*low, val) < 0 && pred(*high, val) > 0) {
    ItT mid = low + (delta(val, *low) * (high - low)) / delta(*high, *low);

    int cmp_res = pred(*mid, val);
    if (cmp_res < 0) {
      low = mid + 1;
    } else {
      if (cmp_res > 0) {
        high = mid - 1;
      } else {
        return mid;
      }
    }
  }

  if (pred(val, *low) == 0) {
    return low;
  } else {
    if (pred(val, *high) == 0) {
      return high;
    } else {
      return low;
    }
  }
  /*if (*low >= val) {
  return low;
  } else {
  if (*high == val) {
  return high;
  }
  }*/
}
}