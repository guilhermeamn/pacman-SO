#include "timer.h"

__BEGIN_API

Timer::Timer(const unsigned int period, const Function * handler)
{
    db<Timer>(TRC) << "TRC: Timer(period,handler)\n";

    db<Timer>(INF) << "INF: period = " << period << "\n";

    if ( period <= 0 || !handler){
        std::exit(-1);
    }

    _handler = handler;
    _period = period;

    sigemptyset(&action.sa_mask); //limpa as flags
    action.sa_flags = 0;
    action.sa_handler = signal(SIGALRM,handler);

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = _period;

    if(setitimer(ITIMER_REAL,&timer,nullptr) == -1)
    {
        perror("Error calling setitimer()");
        std::exit(1);
    }
}

void Timer::reset()
{
    db<Timer>(TRC) << "TRC: reset\n";

    if(setitimer(ITIMER_REAL,&timer,nullptr) == -1)
    {
        perror("Error calling setitimer()");
        std::exit(1);
    }
}

Timer::~Timer()
{
    db<Timer>(TRC) << "TRC: ~Timer()\n";
}

__END_API