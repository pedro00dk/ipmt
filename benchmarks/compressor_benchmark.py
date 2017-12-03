import os
import subprocess
import timeit

textfiles_dir = 'testFiles/'
textfiles_names = ['code.h', 'shakespeare.txt', 'html.html', 'bin.exe']
cpp_source = '../src/compressor/CompressorTest.cpp'

compile_command = '%s %s "%s"' % ('g++', '-std=c++11 -O2', cpp_source)
run_command = "a.exe lz77 "


def run(command, print_output=False):
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    process.wait()
    if print_output: print(process.stdout.read())


def get_run_time(function, runs=1):
    timer = timeit.Timer(lambda: function)
    time = timer.timeit(number=runs) / runs
    return time


def main():
    run(compile_command)

    for textfile_name in textfiles_names:
        textfile_path = textfiles_dir + textfile_name
        textfile_zip_path = textfile_path + ".myzip"

        if os.path.exists(textfile_zip_path):
            os.remove(textfile_zip_path)

        time = get_run_time(run(run_command + " " + textfile_path, print_output=True))
        original_size = os.path.getsize(textfile_path)

        compressed_size = os.path.getsize(textfile_zip_path)
        print(textfile_name,
              "original_size:" + str(original_size),
              "compressed_size:" + str(compressed_size),
              "compression_ratio:" + str(compressed_size / original_size),
              "time:" + "{:.20f}".format(time))


if __name__ == '__main__':
    main()
