import os
import subprocess
import timeit

from plot_utils import PlotUtils

textfiles_dir = 'testFiles/'
# textfiles_names = ['code.h', 'shakespeare.txt', 'html.html', 'bin.exe']
textfiles_names = ['code.py', 'code_copy.py']
cpp_compression_source = '../src/compressor/CompressorTest.cpp'

compile_base_command = '%s %s ' % ('g++', '-std=c++11 -O2')


def run(command, print_output=False):
    print(command)
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    process.wait()
    if print_output: print(process.stdout.read())


def get_run_time(function, runs=1):
    timer = timeit.Timer(lambda: function)
    time = timer.timeit(number=runs) / runs
    return time


def plot_compression_size():
    # different kind of files and index file
    compression_algorithms = ["lz77"]

    compile_command = compile_base_command + " " + cpp_compression_source
    run(compile_command, print_output=True)

    plot_data = {'original': []}
    for algorithm in compression_algorithms:
        plot_data[algorithm] = []

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name
        textfile_zip_path = textfile_path + ".myzip"

        original_size = os.path.getsize(textfile_path)
        plot_data['original'] += [original_size]

        for algorithm in compression_algorithms:
            run_command = "./a.out %s %s" % (algorithm, textfile_path)

            if os.path.exists(textfile_zip_path):
                os.remove(textfile_zip_path)

            run(run_command, print_output=True)

            compressed_size = os.path.getsize(textfile_zip_path)
            print(textfile_name,
                  "original_size:" + str(original_size),
                  "compressed_size:" + str(compressed_size),
                  "compression_ratio:" + str(compressed_size / original_size))

            plot_data[algorithm] += [compressed_size]

            # plot_data[textfile_name] += [gzip_compressed_size]

    print(plot_data)

    colors = ['#EE3224', '#F78F1E', '#FFC222']
    # TODO: check uncompressed mode
    legend = ['original'] + compression_algorithms  # + ['gzip']
    PlotUtils.bar_plot(plot_data,
                       colors=colors,
                       legend=legend,
                       xticklabels=textfiles_names,
                       ylabel="size in kb",
                       title="Compression")


def plot_compression_time():
    # different kind of files and index file
    compression_algorithms = ["lz77"]

    compile_command = compile_base_command + " " + cpp_compression_source
    run(compile_command, print_output=True)

    plot_data = {}
    for algorithm in compression_algorithms:
        plot_data[algorithm] = []

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name
        textfile_zip_path = textfile_path + ".myzip"

        for algorithm in compression_algorithms:
            run_command = "./a.out %s %s" % (algorithm, textfile_path)

            if os.path.exists(textfile_zip_path):
                os.remove(textfile_zip_path)

            run_time = get_run_time(run(run_command, print_output=True), runs=10)

            plot_data[algorithm] += [run_time]

            # plot_data[textfile_name] += [gzip_compressed_size]

    print(plot_data)

    colors = ['#EE3224', '#F78F1E', '#FFC222']
    legend = compression_algorithms  # + ['gzip']
    PlotUtils.bar_plot(plot_data,
                       colors=colors,
                       legend=legend,
                       xticklabels=textfiles_names,
                       ylabel="time in seconds",
                       title="Compression time")


def plot_index_time():
    compression_algorithms = ['lz77']
    index_algorithms = ['suffix-tree']

    compile_command = compile_base_command + " " + cpp_compression_source  # TODO
    run(compile_command, print_output=True)

    plot_data = {}
    for compression_algorithm in compression_algorithms:
        for index_algorithm in index_algorithms:
            plot_data[compression_algorithm + '-' + index_algorithm] = []

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name

        for compression_algorithm in compression_algorithms:
            for index_algorithm in index_algorithms:
                # run_command = "./a.out %s %s" % (algorithm, textfile_path)
                run_command = "./a.out %s %s" % ('index', textfile_path)

                run_time = get_run_time(run(run_command, print_output=True), runs=10)
                # run(run_command, print_output=True)

                plot_data[compression_algorithm + '-' + index_algorithm] += [run_time]
                # plot_data[textfile_name] += [gzip_compressed_size]

    print(plot_data)

    colors = ['#EE3224', '#F78F1E', '#FFC222']
    # legend = ['original'] + [compression_algorithm + '-' + index_algorithm
    #                          for compression_algorithm in compression_algorithms
    #                          for index_algorithm in index_algorithms]  # + ['gzip']

    legend = [compression_algorithm + '-' + index_algorithm
              for compression_algorithm in compression_algorithms
              for index_algorithm in index_algorithms]

    PlotUtils.bar_plot(plot_data,
                       colors=colors,
                       legend=legend,
                       xticklabels=textfiles_names,
                       ylabel="time in seconds",
                       title="Index time")


def plot_search_time():
    patterns_filename = 'patterns.txt'
    compression_algorithms = ['lz77']
    index_algorithms = ['suffix-tree']

    pattern_sizes = [5, 10]

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name

        plot_data = {}
        for compression_algorithm in compression_algorithms:
            for index_algorithm in index_algorithms:
                plot_data[compression_algorithm + '-' + index_algorithm] = {}
                plot_data[compression_algorithm + '-' + index_algorithm]['x'] = pattern_sizes
                plot_data[compression_algorithm + '-' + index_algorithm]['y'] = []

        for pattern_size in pattern_sizes:
            patterns_path = '%s-%d' % (textfile_path, pattern_size)

            for compression_algorithm in compression_algorithms:
                for index_algorithm in index_algorithms:
                    # run_command = "./a.out %s %s" % (algorithm, textfile_path)
                    # index
                    index_command = "./a.out %s %s" % ('index', textfile_path)
                    run(index_command, print_output=True)

                    # search
                    search_command = "./a.out %s %s teste" % ('index', textfile_path)
                    run_time = get_run_time(run(search_command, print_output=True), runs=10)

                    plot_data[compression_algorithm + '-' + index_algorithm]['y'] += [run_time]
                    # plot_data[textfile_name] += [gzip_compressed_size]


                    # print(plot_data)
        colors = ['#EE3224', '#F78F1E', '#FFC222']
        legend = [compression_algorithm + '-' + index_algorithm
                                 for compression_algorithm in compression_algorithms
                                 for index_algorithm in index_algorithms]  # + ['gzip']
        PlotUtils.line_plot(plots=plot_data,
                            xlabel='x',
                            ylabel='y',
                            save_filename='asd')


def main():
    # plot_compression_time()
    # plot_compression_size()
    # plot_index_time()
    plot_search_time()


if __name__ == '__main__':
    main()
