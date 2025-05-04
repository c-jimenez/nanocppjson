# -*- coding: utf-8 -*-
#
# Copyright (c) 2025 c-jimenez
# SPDX-License-Identifier: MIT
#

import sys

# Entry point
if __name__ == '__main__':

    # Check args
    if len(sys.argv) != 3:
        print("Usage : branch_coverage_filter.py coverage_file_path output_file_path")
        sys.exit(1)

    # Get args
    coverage_file_path = sys.argv[1]
    output_file_path = sys.argv[2]

    print("Filtering branch coverage from {} to {}".format(
        coverage_file_path, output_file_path))

    #  Open coverage file and output file
    try:
        coverage_file = open(coverage_file_path)
        output_file = open(output_file_path, "w")
    except:
        print("Unable to open coverage file")
        sys.exit(1)

    #  Parse coverage file to look for source file entries
    eof = False
    source_file = None
    while not eof:
        line = coverage_file.readline()
        if not line:
            eof = True
        else:
            #  Source file entry begins with 'SF:'
            if line.startswith("SF:"):
                # Extract source file path
                source_file_path = line[3:].strip('\n')
                current_src_line = 0
                output_file.write(line)

                # Exclude system files
                if not source_file_path.startswith("/usr"):
                    # Open corresponding source file
                    try:
                        source_file = open(source_file_path)
                    except:
                        print("Unable to open  source file : ", source_file_path)
                        sys.exit(1)
                else:
                    source_file = None
            elif line.startswith("end_of_record"):
                # End of source file
                source_file = None
                output_file.write(line)
            elif not source_file is None and line.startswith("BRDA:"):
                # Branch coverage instruction, extract line of code
                source_line = int(line[5:].split(",")[0])

                # Extract corresponding line of code from the source file
                if source_line < current_src_line:
                    source_file.seek(0)
                    current_src_line = 0
                while current_src_line != source_line:
                    source_line_contents = source_file.readline()
                    current_src_line += 1
                    if not source_line_contents:
                        print(
                            "The coverage file does not match the source file : ", source_file_path)
                        sys.exit(1)

                # Check if the line contains a conditional instruction
                if ((source_line_contents.find("if ") >= 0) or
                    (source_line_contents.find("else") >= 0) or
                    (source_line_contents.find("while") >= 0) or
                    (source_line_contents.find("switch") >= 0) or
                    (source_line_contents.find("case") >= 0) or
                    (source_line_contents.find("for ") >= 0) or
                        (source_line_contents.find("?") >= 0)):
                    # It contains a conditional instruction so the line must be included for coverage
                    output_file.write(line)
            else:
                output_file.write(line)

    sys.exit(0)
