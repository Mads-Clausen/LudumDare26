#ifndef TIMING_INCLUDED
#define TIMING_INCLUDED

namespace timing
{
    class Timer
    {
        private:
            int startTicks;
            int pausedTicks;

            bool paused;
            bool started;

        public:
            Timer();

            void start();
            void stop();
            void pause();
            void unpause();

            int get_ticks();

            bool is_started();
            bool is_paused();
    };

    void init();
    void update();
    float get_deltatime();
    float get_fps();
}

#endif // TIMING_INCLUDED
