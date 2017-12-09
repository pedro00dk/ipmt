import functools
import os
import subprocess
import timeit

from plot_utils import PlotUtils

textfiles_dir = 'testFiles/'
# textfiles_names = ['code.h', 'shakespeare.txt', 'html.html', 'bin.exe']
# textfiles_names = ['code.h', 'html.html', 'bin.exe']
textfiles_names = ['code.py']
cpp_compression_source = '../src/compressor/CompressorTest.cpp'
cpp_ipmt_source = '../src/ipmt.cpp'
compile_base_command = 'g++ -std=c++11 -O2 '
run_base_command = './a.out '
# run_base_command = 'a.exe '
colors = ['#EE3224', '#F78F1E', '#FFC222', '#FFC222', '#FFC222', '#FFC222']
num_of_runs = 10

compression_algorithms = ['uncompressed', 'lz77', 'lz78']
index_algorithms = ['suffixtree']
pattern_sizes = [5, 10]


def run(command, print_output=True):
    print(command)
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    process.wait()
    if process.returncode != 0:
        raise Exception(process.stderr.read())
    if print_output: print(process.stdout.read())


def get_run_time(run_function, runs=1):
    timer = timeit.Timer(lambda: run_function())
    time = timer.timeit(number=runs) / runs
    return time


def plot_compression_size():
    compile_command = '%s %s' % (compile_base_command, cpp_compression_source)
    run(compile_command, print_output=True)

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name
        textfile_zip_path = textfile_path + ".myzip"

        plot_values = []
        plot_labels = []
        for algorithm in compression_algorithms:
            plot_labels += [algorithm]
        plot_labels += ['gzip']

        for algorithm in compression_algorithms:
            run_command = run_base_command + ' %s %s compress' % (algorithm, textfile_path)

            if os.path.exists(textfile_zip_path):
                os.remove(textfile_zip_path)

            run(run_command, print_output=True)
            compressed_size = os.path.getsize(textfile_zip_path)
            plot_values += [compressed_size]

        gzip_command = "gzip -k -f " + textfile_path
        run(gzip_command)
        textfile_gzip_path = textfile_path + ".gz"
        gzip_size = os.path.getsize(textfile_gzip_path)
        plot_values += [gzip_size]

        print(plot_values)

        PlotUtils.bar_plot(plot_values,
                           plot_labels=plot_labels,
                           colors=colors,
                           ylabel="size in bytes",
                           title="Compression size of " + textfile_name,
                           save_filename='plots/compress-size-' + textfile_name)


def plot_compression_time():
    compile_command = compile_base_command + " " + cpp_compression_source
    run(compile_command, print_output=True)

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name
        textfile_zip_path = textfile_path + ".myzip"

        plot_values = []
        plot_labels = []

        for algorithm in compression_algorithms:
            plot_labels += [algorithm]
        plot_labels += ['gzip']

        for algorithm in compression_algorithms:
            run_command = run_base_command + ' %s %s compress' % (algorithm, textfile_path)

            if os.path.exists(textfile_zip_path):
                os.remove(textfile_zip_path)

            r = functools.partial(run, run_command, print_output=True)
            run_time = get_run_time(r, runs=num_of_runs)

            plot_values += [run_time]

        gzip_command = "gzip -k -f " + textfile_path
        r = functools.partial(run, gzip_command, print_output=True)
        run_time = get_run_time(r, runs=num_of_runs)
        plot_values += [run_time]

        print(plot_values)

        PlotUtils.bar_plot(plot_values=plot_values,
                           plot_labels=plot_labels,
                           colors=colors,
                           ylabel="time in seconds",
                           title="Compression time of " + textfile_name,
                           save_filename='plots/compress-time' + textfile_name)


def plot_decompression_time():
    compile_command = compile_base_command + " " + cpp_compression_source
    run(compile_command, print_output=True)

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name
        textfile_zip_path = textfile_path + ".myzip"

        plot_values = []
        plot_labels = []

        for algorithm in compression_algorithms:
            plot_labels += [algorithm]
        plot_labels += ['gzip']

        for algorithm in compression_algorithms:
            compression_command = run_base_command + ' %s %s compress' % (algorithm, textfile_path)
            decompression_command = run_base_command + ' %s %s decompress' % (algorithm, textfile_zip_path)
            run(compression_command, print_output=True)

            r = functools.partial(run, decompression_command, print_output=True)
            run_time = get_run_time(r, runs=num_of_runs)
            plot_values += [run_time]

        textfile_gzip_path = textfile_path + ".gz"
        gzip_compress_command = "gzip -k -f " + textfile_path
        gzip_decompress_command = "gzip -k -f -d " + textfile_gzip_path
        run(gzip_compress_command)
        r = functools.partial(run, gzip_decompress_command, print_output=True)
        run_time = get_run_time(r, runs=num_of_runs)
        plot_values += [run_time]

        print(plot_values)

        PlotUtils.bar_plot(plot_values=plot_values,
                           plot_labels=plot_labels,
                           colors=colors,
                           ylabel="time in seconds",
                           title="Decompression time of " + textfile_name,
                           save_filename='plots/decompress-time-' + textfile_name)


def plot_index_size():
    compile_command = compile_base_command + " " + cpp_ipmt_source
    run(compile_command, print_output=True)

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name
        textfile_idx_path = textfiles_dir + textfile_name + ".idx"

        plot_values = []
        plot_labels = []
        for compression_algorithm in compression_algorithms:
            for index_algorithm in index_algorithms:
                plot_labels += [compression_algorithm + '-' + index_algorithm]

        for compression_algorithm in compression_algorithms:
            for index_algorithm in index_algorithms:
                index_command = "./a.out index %s --compression=%s --indextype=%s" % (textfile_path,
                                                                                      compression_algorithm,
                                                                                      index_algorithm)

                run(index_command)
                index_size = os.path.getsize(textfile_idx_path)
                plot_values += [index_size]

        PlotUtils.bar_plot(plot_values=plot_values,
                           plot_labels=plot_labels,
                           colors=colors,
                           ylabel="size in bytes",
                           title="Index size of " + textfile_name,
                           save_filename='plots/index-size-' + textfile_name)


def plot_index_time():
    compile_command = compile_base_command + " " + cpp_ipmt_source
    run(compile_command, print_output=True)

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name

        plot_values = []
        plot_labels = []
        for compression_algorithm in compression_algorithms:
            for index_algorithm in index_algorithms:
                plot_labels += [compression_algorithm + '-' + index_algorithm]

        for compression_algorithm in compression_algorithms:
            for index_algorithm in index_algorithms:
                index_command = "./a.out index %s --compression=%s --indextype=%s" % (textfile_path,
                                                                                      compression_algorithm,
                                                                                      index_algorithm)

                r = functools.partial(run, index_command, print_output=True)
                run_time = get_run_time(r, runs=num_of_runs)

                plot_values += [run_time]
                # plot_data[textfile_name] += [gzip_compressed_size]

        PlotUtils.bar_plot(plot_values=plot_values,
                           plot_labels=plot_labels,
                           colors=colors,
                           ylabel="time in seconds",
                           title="Index time of " + textfile_name,
                           save_filename='plots/index-time-' + textfile_name)


def plot_search_time():
    compile_command = compile_base_command + " " + cpp_ipmt_source
    run(compile_command, print_output=True)

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name
        textfile_idx_path = textfiles_dir + textfile_name + '.idx'

        plot_data = {}
        for compression_algorithm in compression_algorithms:
            for index_algorithm in index_algorithms:
                plot_data[compression_algorithm + '-' + index_algorithm] = {}
                plot_data[compression_algorithm + '-' + index_algorithm]['x'] = pattern_sizes
                plot_data[compression_algorithm + '-' + index_algorithm]['y'] = []

        plot_data['grep'] = {}
        plot_data['grep']['x'] = pattern_sizes
        plot_data['grep']['y'] = []

        for pattern_size in pattern_sizes:
            patterns_path = '%s-%d' % (textfile_path, pattern_size)

            for compression_algorithm in compression_algorithms:
                for index_algorithm in index_algorithms:
                    # index
                    index_command = "./a.out index %s --compression=%s --indextype=%s " % (textfile_path,
                                                                                           compression_algorithm,
                                                                                           index_algorithm)
                    run(index_command, print_output=True)

                    # search
                    search_command = "./a.out search %s %s -c" % ('pattern', textfile_idx_path)
                    # search_command = "./a.out search -p %s %s -c" % (patterns_path, textfile_idx_path)

                    r = functools.partial(run, search_command, print_output=True)
                    run_time = get_run_time(r, runs=num_of_runs)

                    plot_data[compression_algorithm + '-' + index_algorithm]['y'] += [run_time]

            grep_command = 'grep pattern "%s" -o | wc -l' % (textfile_path)
            # grep_command = 'grep -f "%s" "%s" -o | wc -l' % (patterns_path,
            #                                             textfile_path)
            r = functools.partial(run, grep_command)
            run_time = get_run_time(r, runs=num_of_runs)
            plot_data['grep']['y'] += [run_time]

        PlotUtils.line_plot(plots=plot_data,
                            xlabel='Pattern sizes',
                            ylabel='Time in seconds',
                            title='Search time ' + textfile_name,
                            save_filename='plots/search-time-' + textfile_name)


def main():
    os.makedirs('plots', exist_ok=True)
    plot_compression_size()
    plot_compression_time()
    plot_decompression_time()
    plot_index_size()
    plot_index_time()
    plot_search_time()


if __name__ == '__main__':
    main()
