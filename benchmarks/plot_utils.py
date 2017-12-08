import matplotlib.pyplot as plt
import pandas as pd


class PlotUtils:
    @staticmethod
    def line_plot(plots, xlabel, ylabel, save_filename):
        for plot_label, plot_data in plots.items():
            plt.plot(plot_data['x'], plot_data['y'], label=plot_label, marker='x')

        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.legend()
        plt.grid()
        plt.show()
        # plt.savefig(save_filename + '.png')
        plt.clf()

    @staticmethod
    def bar_plot(plot_data, colors, legend, xticklabels, ylabel, title):
        df = pd.DataFrame(plot_data)
        x_labels = df.keys()
        # Setting the positions and width for the bars
        pos = list(range(len(df[x_labels[0]])))
        width = 0.25

        # Plotting the bars
        fig, ax = plt.subplots(figsize=(10, 5))

        # Create a bar with pre_score data,
        # in position pos + some width buffer
        for idx, x_label in enumerate(x_labels):
            plt.bar([p + idx * width for p in pos],
                    df[x_label],
                    width,
                    alpha=0.5,
                    color=colors[idx],
                    label=legend[idx])

        # Set the y axis label
        ax.set_ylabel(ylabel)

        # Set the chart's title
        ax.set_title(title)

        # Set the position of the x ticks
        ax.set_xticks([p + 1.5 * width for p in pos])

        # Set the labels for the x ticks
        ax.set_xticklabels(xticklabels)

        # Setting the x-axis and y-axis limits
        plt.xlim(min(pos) - width, max(pos) + width * 4)

        plt.ylim([0, max([max(df[key]) for key in df.keys()]) * 1.1])

        # Adding the legend and showing the plot
        plt.legend(legend, loc='upper left')
        plt.grid()
        plt.show()
