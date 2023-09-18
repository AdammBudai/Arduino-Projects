# Design and implement a Stopwatch based on Arduino UNO with an attached Funshield.
# The stopwatch must measure the time internally in milliseconds and display the measured values on the 7-segment LED display with 0.1s precision (including the decimal dot, rounding down).
# The number should be displayed in the regular decadic format used in the Czech Republic (no leading zeros except one right before the decimal dot).
# The stopwatch is zeroed at the beginning (i.e., displaying only 0.0 aligned right).

# The stopwatch is always in one of three logical states (stopped, running, or lapped).
# In the stopped state, the internal clock is not advanced and the display is showing the last measured value (this state is also the initial state after boot).
# In the running state, the internal clock is measuring passed time and the value on the display is updated continuously.
# In the lapped state, the stopwatch is still running (collecting time), but the display is frozen -- showing the value which was actually when the lapped state was initiated. 
# The transition diagram looks as follows:

# Button 1 performs the start/stop function. It has no bearing in the lapped state.
# Button 2 freezes/unfreezes (laps/un-laps) the time. It has no bearing on the `stopped`` state.
# Button 3 works only in the stopped state and it resets the stopwatch (sets the internal time counter to 0).