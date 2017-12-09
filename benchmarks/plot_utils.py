import matplotlib.pyplot as plt
import pandas as pd


class PlotUtils:
    @staticmethod
    def line_plot(plots, xlabel, ylabel, title, save_filename=None):
        for plot_label, plot_data in plots.items():
            plt.plot(plot_data['x'], plot_data['y'], label=plot_label, marker='x')

        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.title(title)
        plt.legend()
        plt.grid()
        if save_filename is None:
            plt.show()
        else:
            plt.savefig(save_filename + '.png')
        plt.clf()

    @staticmethod
    def bar_plot(plot_values, plot_labels, colors, ylabel, title, save_filename=None):
        # Setting the positions and width for the bars
        positions = list(range(len(plot_values)))
        width = 0.75

        # Plotting the bars
        fig, ax = plt.subplots(figsize=(10, 5))

        # Create a bar with pre_score data,
        # in position pos + some width buffer
        for idx, value in enumerate(plot_values):
            plt.bar(positions[idx],
                    value,
                    width,
                    alpha=0.5,
                    color=colors[idx],
                    label=plot_labels[idx])

        # Set the y axis label
        ax.set_ylabel(ylabel)

        # Set the chart's title
        ax.set_title(title)

        # Set the position of the x ticks
        ax.set_xticks([p + 1.5 * width for p in positions])

        # Set the labels for the x ticks
        ax.set_xticklabels(plot_labels)

        # Setting the x-axis and y-axis limits
        plt.xlim(min(positions) - width, max(positions) + width * 4)
        plt.ylim([0, max(plot_values) * 1.1])

        # Adding the legend and showing the plot
        plt.legend(plot_labels, loc='best')
        plt.grid()
        if save_filename is None:
            plt.show()
        else:
            plt.savefig(save_filename + '.png')
        plt.clf()
