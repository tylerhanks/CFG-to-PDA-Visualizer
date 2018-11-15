import tkinter as tk

import re
import sys

LAMBDA = '\u03BB'

def getTransitions(file):
    fo = open(file, 'r')
    transitions = []
    lines = []

    for line in fo:
        line = line.strip()
        line = line.replace('#', LAMBDA)
        line_vec = re.split(', | |\t', line)
        lines.append(line_vec)

        transition = line_vec[1] + ', ' + line_vec[2][:-1] + ' : ' + line_vec[5][:-1]
        transitions.append(transition)

    fo.close()
    return (transitions, lines)

class PDAWindow:
    
    def __init__(self, file):
        self.count = 1
        self.offset = 100
        self.GRAPH_HEIGHT = 400
        self.FONT = ("Purisa", 16)
        self.root = tk.Tk()
        self.canvas = tk.Canvas(self.root, width = 1000, height = 1000, bg = 'grey')
        self.transitions = getTransitions(file)
        self.canvas.bind('<Button-1>', self.handleEvent)
        self.root.wm_title("PDA Visualizer")

    def drawNode(self, x, y, name):
        self.canvas.create_oval(x-40,y-40,x+40,y+40, fill = 'white')
        self.canvas.create_text(x,y, text = name, font = self.FONT)

    def drawArrow(self, x1, y1, x2, y2):
        self.canvas.create_line(x1+40, y1, x2-40, y2)
        self.canvas.create_polygon(x2-40,y2,x2-60,y2+20,x2-60,y2-20)

    def handleEvent(self, event):
        if self.count < len(self.transitions[0])-1:
            self.canvas.create_text(500, self.GRAPH_HEIGHT - self.offset, text = self.transitions[0][self.count], font = self.FONT)
            production = self.transitions[1][self.count][-3] + ' ' + self.transitions[1][self.count][-2] + ' ' + self.transitions[1][self.count][-1]
            self.canvas.create_text(200, self.GRAPH_HEIGHT+100+self.offset, text = production, font = self.FONT, anchor = 'sw')
            transition = self.transitions[1][self.count][0] + ', ' + self.transitions[1][self.count][1] + ', ' + self.transitions[1][self.count][2] + ' ' + self.transitions[1][self.count][3] + ' ' + self.transitions[1][self.count][4] + ', ' + self.transitions[1][self.count][5]
            self.canvas.create_text(700, self.GRAPH_HEIGHT+100+self.offset, text = transition, font = self.FONT, anchor = 'sw')
            self.count += 1
            self.offset += 25
        elif self.count == len(self.transitions[0])-1:
            self.canvas.create_text(700, self.GRAPH_HEIGHT+100+self.offset, text = "d(q0, \u03BB, z) = (q1, Sz)", font = self.FONT, anchor = 'sw')
            self.canvas.create_text(700, self.GRAPH_HEIGHT+100+self.offset+25, text = "d(q1, \u03BB, z) = (q2, \u03BB)", font = self.FONT, anchor = 'sw')
        else:
            pass

    

def main():
    if len(sys.argv) != 2:
        print("Usage: python visualizer.py [path to a file containing a GNF grammar]")
        return -1
    else:

        window = PDAWindow(sys.argv[1])
        window.canvas.grid()
        window.canvas.create_oval(500-30, window.GRAPH_HEIGHT+40, 500+30, window.GRAPH_HEIGHT-80, fill = 'grey')
        window.drawNode(200, window.GRAPH_HEIGHT, 'q0')
        window.drawNode(500, window.GRAPH_HEIGHT, 'q1')
        window.drawNode(800, window.GRAPH_HEIGHT, 'q3')
        window.drawArrow(200, window.GRAPH_HEIGHT, 500, window.GRAPH_HEIGHT)
        window.drawArrow(500, window.GRAPH_HEIGHT, 800, window.GRAPH_HEIGHT)
        window.canvas.create_text(200, window.GRAPH_HEIGHT+150, text = "Input Grammar:", font = ("Purisa", 20))
        window.canvas.create_text(800, window.GRAPH_HEIGHT+150, text = "Transition Rule:", font = ("Purisa", 20))

        window.canvas.create_text(350, window.GRAPH_HEIGHT-20, text = window.transitions[0][0], font = window.FONT)
        window.canvas.create_text(650, window.GRAPH_HEIGHT-20, text = window.transitions[0][-1], font = window.FONT)

        window.root.mainloop()

if __name__ == "__main__":
    main()

