#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

extern int  port;

void	quit(int sig_num);
void	signal_catching();

#endif