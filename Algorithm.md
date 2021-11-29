# Algorithm

## Line tracing
* If F, go forward
* If L or R sensed, turn until F
* If no sensor high, then two possibilities
  * line is between F and L (or F and R):   
       sway ~10 degrees to find line. (this may be improved by leveraging history, ie PID)
  * line is not between L and R:
        sway progressively more angle(x2 every time) until any sensor detects line.

## Junction type determination  
If two or more sensors are high, determine junction
1. list the sensors which are high
2. go forward 4cm (=2.3 * line width)
3. list sensors which were high while moving forward
4. go forward 6cm
5. sway ~10 degrees to check if line is ahead    
6. Go forward 10cm (??)

now junction type is determined.


## Junction turn
determine direction to go based on strategy.(LEFT_FIRST/RIGHT_FIRST)
- If forward, go forward
- If turn,
1. Turn for 3 ticks
2. Turn until nearest sensor is sensed.
3. delay for a while