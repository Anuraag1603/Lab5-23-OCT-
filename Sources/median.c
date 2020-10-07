/*
 * median.c
 *
 *  Created on: 15 Oct 2015
 *      Author: 11671284
 */

#include "median.h"

uint8_t Median_Filter3(const uint8_t n1, const uint8_t n2, const uint8_t n3)
{
if ((n1 >= n2 && n1 <= n3) || (n1 <= n2 && n1 >= n3))
  {
  	return n1;
  }
  else if ((n2 >= n1 && n2 <= n3) || (n2 <= n1 && n2 >= n3))
  {
  	return n2;
  }
  /* if not n1 or n2 then n3 */
  return n3;
}
