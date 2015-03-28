#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "aux.h"

int search(node * self, int k);
int join_succi(node * self, int service);
int join(node * self, int x);
int show(node * self);
int leave(node * self);
void exit_app(node * self);

#endif

