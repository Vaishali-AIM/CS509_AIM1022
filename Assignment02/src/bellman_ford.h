#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <stdbool.h>
#include "csr.h"

void bellman_ford(const CSRGraph *g, int source, long long *dist,
                   bool *has_negative_cycle);

#endif