** TODO **

** NOT DOING **

Do some RAII stuff so they can do Ctrl^C without issue
  - Decided not to - they might not really want to resume if they've Ctrl+Cd

** DONE **

All fixed by ending the timer immediately on beginning a break - and starting a new interval as soon as the break finishes.
 - BUG: End of the timer is counted as the end of you work interval! Not when the timer started!!
    Just update the file as soon as we start out break!
 - Change the timers to do calculation based on current time
    Skeptical about their behaviour during sleep...
 - Change the break timer to automatically resume the session

Both of these errors were because we were ignore passed filenames - always looking for time_log.txt...
 - SEGFAULT: Trying to stat from inside the archive directory cause a segfault: ../build/skinner stats 2025-10-09.txt
 - BUG: Stats seems to give the same value every time...?