import tkinter as tk
from tkinter import Menu
import os

def read_colors():
    with open(f'{home_path}/.cache/wal/colors', 'r') as f:
        return [line.strip() for line in f.readlines()]

home_path = os.environ.get('HOME')

colors = read_colors()

# Create main window but don't show it
root = tk.Tk()
root.withdraw()

# Create the context menu
menu = Menu(root, tearoff=0, bg=colors[0], fg=colors[8])

menu.add_command(label="Option 1")
menu.add_command(label="Option 2")
menu.add_command(label="Option 3")
menu.add_separator()
menu.add_command(label="Exit", command=root.quit)

# Get mouse position
x = root.winfo_pointerx()
y = root.winfo_pointery()

# Display the menu at the mouse position
menu.post(x, y)

root.mainloop()


# print('Right click detected')
