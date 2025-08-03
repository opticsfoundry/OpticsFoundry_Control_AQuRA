import os
import time
import re
import matplotlib.pyplot as plt
import numpy as np

# Configuration
WATCH_DIR = "."  # <-- change to your target directory
POLL_INTERVAL = 0.1  # seconds
FILENAME_PATTERN = re.compile(r"input\d{4}\.dat")

# Track last modification time
last_mod_time = 0
last_filename = ""

# Setup plot
plt.ion()
fig, ax = plt.subplots()
line, = ax.plot([], [])

def get_latest_file():
    """Return the most recently modified matching file (name, mtime)"""
    latest_file = None
    latest_time = 0
    for f in os.listdir(WATCH_DIR):
        if FILENAME_PATTERN.fullmatch(f):
            full_path = os.path.join(WATCH_DIR, f)
            try:
                mtime = os.path.getmtime(full_path)
                if mtime > latest_time:
                    latest_time = mtime
                    latest_file = f
            except FileNotFoundError:
                continue  # File may have been deleted
    return latest_file, latest_time

def read_file(filepath):
    """Reads the file, skipping the first 3 lines"""
    try:
        with open(filepath, "r") as f:
            lines = f.readlines()[3:]
            data = np.loadtxt(lines)
            return data
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return None

def update_plot(data, title=""):
    x, y = data[:, 0], data[:, 1]
    line.set_xdata(x)
    line.set_ydata(y)
    ax.set_title(title)
    ax.relim()
    ax.autoscale_view()
    plt.draw()
    plt.pause(0.01)

# Initial display
filename, mod_time = get_latest_file()
if filename:
    full_path = os.path.join(WATCH_DIR, filename)
    print(f"Initial display: {filename}")
    data = read_file(full_path)
    if data is not None and data.shape[1] == 2:
        update_plot(data, title=filename)
    last_mod_time = mod_time
    last_filename = filename

# Monitor loop
print(f"Watching directory: {WATCH_DIR}")
while True:
    time.sleep(POLL_INTERVAL)
    filename, mod_time = get_latest_file()
    if filename and mod_time > last_mod_time:
        full_path = os.path.join(WATCH_DIR, filename)
        print(f"New file detected: {filename}")
        data = read_file(full_path)
        if data is not None and data.shape[1] == 2:
            update_plot(data, title=filename)
            last_mod_time = mod_time
            last_filename = filename
