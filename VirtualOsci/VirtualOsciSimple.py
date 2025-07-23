import sys
import numpy as np
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QPushButton, QVBoxLayout,
    QHBoxLayout, QWidget, QSizePolicy
)
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from read_data import read_data

class OscilloscopeGUI(QMainWindow):
    def __init__(self, sequence_file, output_file):
        super().__init__()

        self.setWindowTitle("That's what's going on in Control")
        self.setGeometry(100, 100, 1200, 800)

        self.ch, self.ch0, self.namech = read_data(sequence_file, output_file)
        self.starttime = np.min(self.ch[:, 0])
        self.endtime = np.max(self.ch[:, 0])
        self.mintime = self.starttime
        self.maxtime = self.endtime
        self.excludevector = set()
        self.separationfactor = 1.5

        self.init_ui()
        self.plot_data()

    def init_ui(self):
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

        # Layouts
        main_layout.addLayout(controls_layout)
        main_layout.addWidget(self.canvas)

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
    win = OscilloscopeGUI(
        "SequenceListAfterExecutionComputerReadable.dat",
        "OutputListComputerReadable.dat"
    )
    win.show()
    sys.exit(app.exec())