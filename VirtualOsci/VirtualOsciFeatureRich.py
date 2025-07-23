import sys
import os
import json
import numpy as np
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QPushButton, QVBoxLayout,
    QHBoxLayout, QWidget, QSizePolicy, QFileDialog, QMenuBar, QMenu, QAction
)
from PyQt6.QtCore import QTimer
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from read_data import read_data

class OscilloscopeGUI(QMainWindow):
    def __init__(self, sequence_file=None, output_file=None):
        super().__init__()

        self.setWindowTitle("That's what's going on in Control")
        self.setGeometry(100, 100, 1200, 800)

        self.sequence_file = sequence_file
        self.output_file = output_file
        self.excludevector = set()
        self.separationfactor = 1.5

        self.last_seq_mtime = None
        self.last_out_mtime = None

        self.init_ui()

        if not (self.sequence_file and self.output_file):
            self.open_files()
        else:
            self.load_and_plot()

        # Timer for auto-refresh
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.check_file_updates)
        self.timer.start(2000)  # check every 2 seconds

    def init_ui(self):
        # Menu bar
        menubar = self.menuBar()
        file_menu = menubar.addMenu("File")

        open_action = QAction("Open .dat files", self)
        open_action.triggered.connect(self.open_files)
        file_menu.addAction(open_action)

        export_action = QAction("Export plot as PNG", self)
        export_action.triggered.connect(self.export_plot_png)
        file_menu.addAction(export_action)

        export_pdf_action = QAction("Export plot as PDF", self)
        export_pdf_action.triggered.connect(self.export_plot_pdf)
        file_menu.addAction(export_pdf_action)

        save_excl_action = QAction("Save Channel Exclusions", self)
        save_excl_action.triggered.connect(self.save_exclusions)
        file_menu.addAction(save_excl_action)

        load_excl_action = QAction("Load Channel Exclusions", self)
        load_excl_action.triggered.connect(self.load_exclusions)
        file_menu.addAction(load_excl_action)

        # Main widget and layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        main_layout = QHBoxLayout()
        central_widget.setLayout(main_layout)

        # Plot area
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.ax_main = self.figure.add_subplot(121)
        self.ax_labels = self.figure.add_subplot(122)
        self.ax_labels.set_xticks([])
        self.ax_labels.set_yticks([])
        self.canvas.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)

        # Connect event handler
        self.canvas.mpl_connect('button_press_event', self.on_click)
        self.click_mode = None
        self.pending_clicks = []

        # Control buttons
        controls_layout = QVBoxLayout()
        btn_zoom_in = QPushButton("Zoom in time")
        btn_zoom_out = QPushButton("Zoom out in time")
        btn_exclude = QPushButton("Exclude Channels")
        btn_show_some = QPushButton("Show certain Channels")
        btn_show_all = QPushButton("Show all Channels")

        btn_zoom_in.clicked.connect(self.zoom_in_time)
        btn_zoom_out.clicked.connect(self.zoom_out_time)
        btn_exclude.clicked.connect(self.exclude_channels)
        btn_show_some.clicked.connect(self.show_some_channels)
        btn_show_all.clicked.connect(self.show_all_channels)

        for btn in [btn_zoom_in, btn_zoom_out, btn_exclude, btn_show_some, btn_show_all]:
            controls_layout.addWidget(btn)

        main_layout.addLayout(controls_layout)
        main_layout.addWidget(self.canvas)

    def open_files(self):
        seq_path, _ = QFileDialog.getOpenFileName(self, "Select SequenceListAfterExecutionComputerReadable.dat")
        out_path, _ = QFileDialog.getOpenFileName(self, "Select OutputListComputerReadable.dat")
        if seq_path and out_path:
            self.sequence_file = seq_path
            self.output_file = out_path
            self.load_and_plot()

    def load_and_plot(self):
        self.ch, self.ch0, self.namech = read_data(self.sequence_file, self.output_file)
        self.starttime = np.min(self.ch[:, 0])
        self.endtime = np.max(self.ch[:, 0])
        if not hasattr(self, 'mintime') or not hasattr(self, 'maxtime'):
            self.mintime = self.starttime
            self.maxtime = self.endtime
        self.last_seq_mtime = os.path.getmtime(self.sequence_file)
        self.last_out_mtime = os.path.getmtime(self.output_file)
        self.plot_data()

    def check_file_updates(self):
        if not (self.sequence_file and self.output_file):
            return
        try:
            current_seq_mtime = os.path.getmtime(self.sequence_file)
            current_out_mtime = os.path.getmtime(self.output_file)
            if (current_seq_mtime != self.last_seq_mtime or
                current_out_mtime != self.last_out_mtime):
                print("Files changed. Reloading data...")
                self.load_and_plot()
        except Exception as e:
            print("File check error:", e)

    def export_plot_png(self):
        filename, _ = QFileDialog.getSaveFileName(self, "Save plot as PNG", filter="PNG Files (*.png)")
        if filename:
            self.figure.savefig(filename, format='png')

    def export_plot_pdf(self):
        filename, _ = QFileDialog.getSaveFileName(self, "Save plot as PDF", filter="PDF Files (*.pdf)")
        if filename:
            self.figure.savefig(filename, format='pdf')

    def save_exclusions(self):
        filename, _ = QFileDialog.getSaveFileName(self, "Save exclusions", filter="JSON Files (*.json)")
        if filename:
            with open(filename, 'w') as f:
                json.dump(sorted(list(self.excludevector)), f)

    def load_exclusions(self):
        filename, _ = QFileDialog.getOpenFileName(self, "Load exclusions", filter="JSON Files (*.json)")
        if filename:
            with open(filename, 'r') as f:
                self.excludevector = set(json.load(f))
            self.plot_data()

def zoom_in_time(self):
        self.click_mode = 'zoom'
        self.pending_clicks = []
        print("Click twice to select zoom range")

def zoom_out_time(self):
    self.mintime = self.starttime
    self.maxtime = self.endtime
    self.plot_data()

def exclude_channels(self):
    self.click_mode = 'exclude'
    print("Click on channels to exclude, right-click to finish")

def show_some_channels(self):
    self.click_mode = 'include'
    self.excludevector = set(range(100))
    print("Click on channels to show, right-click to finish")

def show_all_channels(self):
    self.excludevector.clear()
    self.plot_data()

def on_click(self, event):
    if self.click_mode == 'zoom' and event.inaxes == self.ax_main:
        self.pending_clicks.append(event.xdata)
        if len(self.pending_clicks) == 2:
            self.mintime, self.maxtime = sorted(self.pending_clicks)
            self.pending_clicks = []
            self.click_mode = None
            self.plot_data()

    elif self.click_mode in ('exclude', 'include') and event.inaxes == self.ax_labels:
        if event.button == 3:
            self.click_mode = None
            self.plot_data()
            return
        y = event.ydata
        if y is not None:
            ch_index = int(y // self.separationfactor)
            if self.click_mode == 'exclude':
                self.excludevector.add(ch_index)
            elif self.click_mode == 'include':
                self.excludevector.discard(ch_index)
            self.plot_data()

def plot_data(self):
    self.ax_main.clear()
    self.ax_labels.clear()
    self.ax_labels.set_xticks([])
    self.ax_labels.set_yticks([])

    colors = ['r', 'b', 'k', 'g', 'm', 'c', 'y']
    prev_chid = None
    plot_t, plot_v = [], []
    plotii = 0
    actualplotii = 0
    ch0_dict = {int(row[2]): row for row in self.ch0}

    for i, row in enumerate(self.ch):
        t, typecode, chid, *_rest = row
        if typecode == -999 or int(chid) == 2000777:
            continue

        chid = int(chid)

        # New channel block
        if chid != prev_chid:
            if plot_t and (plotii not in self.excludevector):
                plot_t.append(self.ch[:, 0].max())
                plot_v.append(plot_v[-1])

                maxv, minv = max(plot_v), min(plot_v)
                if prev_chid >= 2_000_000 and (maxv - minv) != 0:
                    plot_v = [(v - minv) / (maxv - minv) for v in plot_v]

                if actualplotii % 2 == 1:
                    self.ax_main.fill_between([self.mintime, self.maxtime],
                                                actualplotii * self.separationfactor,
                                                (actualplotii + 1) * self.separationfactor,
                                                color='0.9')

                color = colors[prev_chid % len(colors)]
                shifted_v = [v + actualplotii * self.separationfactor for v in plot_v]
                self.ax_main.plot(plot_t, shifted_v, color=color)

                if prev_chid in ch0_dict:
                    label = self.namech[self.ch0.tolist().index(ch0_dict[prev_chid])]
                    label_text = f"ch={prev_chid} {minv:.2f}-{maxv:.2f} {label}"
                    if actualplotii % 2 == 1:
                        self.ax_labels.fill_between([0, 1],
                                                    actualplotii * self.separationfactor,
                                                    (actualplotii + 1) * self.separationfactor,
                                                    color='0.9')
                    self.ax_labels.text(0.01, (actualplotii + 0.2) * self.separationfactor,
                                        label_text, fontsize=7, verticalalignment='bottom')

            if not (chid >= 2_010_000 and (chid - chid % 20) == (prev_chid - prev_chid % 20)):
                plotii += 1
                if plotii not in self.excludevector:
                    actualplotii += 1

            plot_t = [-1]
            plot_v = [ch0_dict.get(chid, [0]*8)[6] if chid in ch0_dict else 0]
            prev_chid = chid

        plot_t.extend([row[0], row[0]])
        val = max(-42, row[7])
        plot_v.extend([plot_v[-1], val])

        if typecode == 4:
            plot_t.append(row[0] + row[10])
            plot_v.append(row[8])

    self.ax_main.set_xlim(self.mintime, self.maxtime)
    self.ax_main.set_yticks([])
    self.ax_labels.set_xlim(0, 1)
    self.ax_labels.set_yticks([])

    self.canvas.draw()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = OscilloscopeGUI()
    win.show()
    sys.exit(app.exec())