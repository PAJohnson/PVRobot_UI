import curses
import serial

#use curses for persistent keypress output
#use serial to arduinos

stdscr = curses.initscr()
curses.cbreak()
curses.noecho()
stdscr.keypad(1)
stdscr.timeout(200)

stdscr.addstr(0,10,"Hit q to quit")
stdscr.refresh()

key = ''

ser = []

try:
	ser.append(serial.Serial('COM4', 9600, timeout=1))
	print("Connected 1")
except:
        print("Not Connected")
        pass
try:
	ser.append(serial.Serial('COM5', 9600, timeout=1))
	print("Connected 2")
except:
        print("Not Connected")
        pass
try:
	ser.append(serial.Serial('COM9', 9600, timeout=1))
	print("Connected 3")
except:
        print("Not Connected")
        pass
try:
        ser.append(serial.Serial('COM10', 9600, timeout=1))
        print("Connected 4")
except:
        print("Not Connected")
        pass

def forward(ser):
	for i in range(len(ser)):
		ser[i].write(b'F')
	
def backward(ser):
	for i in range(len(ser)):
		ser[i].write(b'B')
		
def left(ser):
	for i in range(len(ser)):
		ser[i].write(b'L')
		
def right(ser):
	for i in range(len(ser)):
		ser[i].write(b'R')
		
def stop(ser):
	for i in range(len(ser)):
		ser[i].write(b'S')

while key != ord('q'):
	key = stdscr.getch()
	stdscr.refresh()
	stdscr.addstr(2, 20, "     ")
	stdscr.addstr(4, 20, "     ")
	stdscr.addstr(3, 10, "     ")
	stdscr.addstr(3, 30, "     ")
	if key == curses.KEY_UP: 
		stdscr.addstr(2, 20, "Up")
		stdscr.addstr(3, 20, "    ")
		forward(ser)
	elif key == curses.KEY_DOWN: 
		stdscr.addstr(4, 20, "Down")
		stdscr.addstr(3, 20, "    ")
		backward(ser)
	elif key == curses.KEY_LEFT:
		stdscr.addstr(3, 10, "Left")
		stdscr.addstr(3, 20, "    ")
		left(ser)
	elif key == curses.KEY_RIGHT:
		stdscr.addstr(3, 30, "Right")
		stdscr.addstr(3, 20, "    ")
		right(ser)
	elif key == ord(' ') or key == -1:
		#stop
		stdscr.addstr(3, 20, "STOP")
		stop(ser)

curses.endwin()
