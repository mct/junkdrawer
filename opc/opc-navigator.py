#!/usr/bin/env python3

##
## A curses program for interactively driving Open Pixel Control sculptures,
## to quickly debug which LEDs are working and which may need some attention.
##
##       -- Michael Toren <mct@toren.net>
##          Thu Apr 29 15:01:02 2021 -0700
##

import curses
import argparse
import time
import opc

class Box:
    # ┌────┐  ╭────╮  ╔════╗
    # │    │  │    │  ║    ║
    # │ 10 │  │ 10 │  ║ 30 ║
    # │    │  │    │  ║    ║
    # └────┘  ╰────╯  ╚════╝
    height = 5
    width = 6
    curved = True

    @classmethod
    def draw(cls, stdscr, line, col, number, color, selected=False):
        assert 0 <= number <= 99
        attr = 0

        if selected:
            ul = '╔'
            ur = '╗'
            h  = '═'
            v  = '║'
            ll = '╚'
            lr = '╝'
            #attr = curses.A_REVERSE
            attr = curses.A_BOLD
        elif cls.curved:
            ul = '╭'
            ur = '╮'
            h  = '─'
            v  = '│'
            ll = '╰'
            lr = '╯'
        else:
            ul = '┌'
            ur = '┐'
            h  = '─'
            v  = '│'
            ll = '└'
            lr = '┘'

        # Top row
        stdscr.addstr(line, col, ul + h*4 + ur, attr)
        line += 1

        # Blank
        stdscr.addstr(line, col, v, attr)
        stdscr.addstr(line, col+1, ' '*4, color)
        stdscr.addstr(line, col+5, v, attr)
        line += 1

        # Number
        stdscr.addstr(line, col, v, attr)
        stdscr.addstr(line, col+1, f' {number:2} ', color)
        stdscr.addstr(line, col+5, v, attr)
        line += 1

        # Blank
        stdscr.addstr(line, col, v, attr)
        stdscr.addstr(line, col+1, ' '*4, color)
        stdscr.addstr(line, col+5, v, attr)
        line += 1

        # Bottom row
        stdscr.addstr(line, col, ll + h*4 + lr, attr)

class Screen:
    def __init__(self, stdscr, args, opc_clients):
        self.stdscr = stdscr
        self.opc = opc_clients
        self.kerning = args.kerning
        self.red_pwm = args.red
        self.green_pwm = args.green
        self.blue_pwm = args.blue
        self.num_leds = args.number_leds

        self.assign_color_index = 0
        self.red         = self.assign_color(curses.COLOR_BLACK, curses.COLOR_RED)
        self.green       = self.assign_color(curses.COLOR_BLACK, curses.COLOR_GREEN)
        self.blue        = self.assign_color(curses.COLOR_BLACK, curses.COLOR_BLUE)
        self.white       = self.assign_color(curses.COLOR_BLACK, curses.COLOR_WHITE)
        self.off         = self.assign_color(curses.COLOR_WHITE, curses.COLOR_BLACK)
        self.status_text = self.assign_color(curses.COLOR_WHITE, curses.COLOR_BLUE, curses.A_BOLD)
        self.error_text  = self.assign_color(curses.COLOR_WHITE, curses.COLOR_RED, curses.A_BOLD)

        self.box = Box
        self.box_color = self.off
        self.box_index = 0

        curses.curs_set(False)
        self.set_all_leds(self.off)
        self.resize()

        while True:
            self.paint()
            self.handle_input()

    def assign_color(self, fg, bg, attr=0):
        self.assign_color_index += 1
        curses.init_pair(self.assign_color_index, fg, bg)
        return curses.color_pair(self.assign_color_index) | attr
        
    def resize(self):
        self.lines, self.cols = self.stdscr.getmaxyx()
        self.boxes_per_row = int(self.cols / (self.box.width + self.kerning))

    def set_all_leds(self, color):
        self.box_color = color
        self.leds = [color] * self.num_leds

    def set_led(self, color):
        self.box_color = color
        self.leds = [self.off] * self.num_leds
        self.leds[self.box_index] = self.box_color

    def move_cursor(self, offset):
        if 0 <= self.box_index + offset < len(self.leds):
            self.box_index += offset
            self.set_led(self.box_color)

    def update_opc(self):
        pixels = []
        for i in self.leds:
            if   i == self.off:   pixels.append((0, 0, 0))
            elif i == self.red:   pixels.append((self.red_pwm, 0, 0))
            elif i == self.green: pixels.append((0, self.green_pwm, 0))
            elif i == self.blue:  pixels.append((0, 0, self.blue_pwm))
            elif i == self.white: pixels.append((self.red_pwm, self.green_pwm, self.blue_pwm))
            else:
                raise("Internal error: Unknown color?")

        for client in self.opc:
            if not client.put_pixels(pixels, channel=0):
                raise(f"OPC: {client}: Not connected")

    def quit(self):
        self.stdscr.clear()
        self.stdscr.refresh()
        self.set_led(self.off)
        self.update_opc()
        exit()

    def paint(self):
        self.update_opc()
        self.stdscr.clear()
        y,x = 0,0

        # draw status line
        self.stdscr.addnstr(y,x, ' OPC Navigator ', self.cols-1, self.status_text)
        self.stdscr.chgat(-1, self.status_text)

        hotkeys = ' [q:Quit]  [r:Red]  [g:Green]  [b:Blue]  [w:White]  [o:Off]'
        x = self.cols - 1 - len(hotkeys)
        if x < 0: x = 0
        self.stdscr.addnstr(y,x, hotkeys, self.cols-1-x, self.status_text)

        y += 1
        x = 0

        truncated = False
        num_drawn = 0

        # Draw each box
        for i, color in enumerate(self.leds):
            self.box.draw(self.stdscr, y,x, i, color, selected=(i==self.box_index))
            num_drawn += 1
            x += self.box.width + self.kerning
            if x + self.box.width + self.kerning > self.cols:
                y += self.box.height
                x = 0
            if y + self.box.height > self.lines:
                truncated = True
                break

        if truncated:
            num_left = self.num_leds - num_drawn
            text = f'Warning: Dispay truncated! {num_left} LED{"s" if num_left > 1 else ""} not shown'
            y = self.lines-1
            x = self.cols-1 - len(text)
            self.stdscr.addnstr(y,x, text, self.cols-1-x, self.error_text)

    def handle_input(self):
        c = self.stdscr.getkey()

        if   c == 'KEY_RESIZE': self.resize()
        elif c == 'q': self.quit()

        ##

        elif c == 'KEY_LEFT':  self.move_cursor(-1)
        elif c == 'KEY_RIGHT': self.move_cursor(+1)
        elif c == 'KEY_UP':    self.move_cursor(-self.boxes_per_row)
        elif c == 'KEY_DOWN':  self.move_cursor(+self.boxes_per_row)

        elif c == 'h': self.move_cursor(-1)
        elif c == 'l': self.move_cursor(+1)
        elif c == 'k': self.move_cursor(-self.boxes_per_row)
        elif c == 'j': self.move_cursor(+self.boxes_per_row)

        elif c == '-': self.move_cursor(-1)
        elif c == '+': self.move_cursor(+1)
        elif c == '=': self.move_cursor(+1)

        elif c == '\x06': self.move_cursor(+1)                  # ^F
        elif c == '\x02': self.move_cursor(-1)                  # ^B
        elif c == '\x0E': self.move_cursor(+self.boxes_per_row) # ^N
        elif c == '\x10': self.move_cursor(-self.boxes_per_row) # ^P

        ##

        elif c == 'r': self.set_led(self.red)
        elif c == 'g': self.set_led(self.green)
        elif c == 'b': self.set_led(self.blue)
        elif c == 'w': self.set_led(self.white)
        elif c == 'o': self.set_led(self.off)

        elif c == '1': self.set_led(self.off)
        elif c == '2': self.set_led(self.red)
        elif c == '3': self.set_led(self.green)
        elif c == '4': self.set_led(self.blue)
        elif c == '5': self.set_led(self.white)
        elif c == '6': self.set_led(self.off)
        elif c == '0': self.set_led(self.off)

        elif c == '!': self.set_all_leds(self.off)
        elif c == '@': self.set_all_leds(self.red)
        elif c == '#': self.set_all_leds(self.green)
        elif c == '$': self.set_all_leds(self.blue)
        elif c == '%': self.set_all_leds(self.white)
        elif c == '^': self.set_all_leds(self.off)
        elif c == ')': self.set_all_leds(self.off)

        elif c == 'R': self.set_all_leds(self.red)
        elif c == 'G': self.set_all_leds(self.green)
        elif c == 'B': self.set_all_leds(self.blue)
        elif c == 'W': self.set_all_leds(self.white)
        elif c == 'O': self.set_all_leds(self.off)

        elif c == 'KEY_F(1)': self.set_all_leds(self.off)
        elif c == 'KEY_F(2)': self.set_all_leds(self.red)
        elif c == 'KEY_F(3)': self.set_all_leds(self.green)
        elif c == 'KEY_F(4)': self.set_all_leds(self.blue)
        elif c == 'KEY_F(5)': self.set_all_leds(self.white)
        elif c == 'KEY_F(6)': self.set_all_leds(self.off)
        elif c == 'KEY_F(0)': self.set_all_leds(self.off)

        ##

        elif c == 'KEY_BACKSPACE': self.set_all_leds(self.off)
        elif c == 'KEY_DC':        self.set_all_leds(self.off)
            

def constrain(val, min, max):
    if val < min: val = min
    if val > max: val = max
    return val

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--kerning',     '-k', type=int,  default=1)
    parser.add_argument('--number-leds', '-l', type=int,  default=97)
    parser.add_argument('--red',         '-r', type=int,  default=255)
    parser.add_argument('--green',       '-g', type=int,  default=255)
    parser.add_argument('--blue',        '-b', type=int,  default=255)
    parser.add_argument('--dim',         '-d', type=int,  default=255)
    parser.add_argument('--server',      '-s', type=str,  default='localhost:7890')
    parser.add_argument('--dry-run',     '-n', action='store_true')
    args = parser.parse_args()

    args.dim   = constrain(args.dim,   0, 255)
    args.red   = constrain(args.red,   0, args.dim)
    args.green = constrain(args.green, 0, args.dim)
    args.blue  = constrain(args.blue,  0, args.dim)

    clients = []

    if not args.dry_run:
        # Support a comma-separated list of OPC servers.  Useful if you have,
        # say, a real sculpture you want to drive, along with a simulator that
        # you want to compare the output to.
        for server in args.server.split(','):
            # If no port is specified, add the default OPC port
            if ':' not in server:
                server += ':7890'

            c = opc.Client(server)
            if not c.can_connect():
                print('Could not connect to', server)
                exit(1)
            clients.append(c)

    curses.wrapper(Screen, args, clients)
