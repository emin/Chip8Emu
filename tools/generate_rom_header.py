import os

def find_ch8_files(root_folder):
    ch8_files = []
    for dirpath, _, filenames in os.walk(root_folder):
        for filename in filenames:
            if filename.endswith('.ch8'):
                ch8_files.append(os.path.join(dirpath, filename))
    return ch8_files

def read_file_bytes(file_path):
    with open(file_path, 'rb') as file:
        return list(file.read())

def generate_cpp_header(files, output_header_path):
    filenames_vector = []
    data_declarations = []
    data_vectors = []

    for idx, file_path in enumerate(files):
        var_name = f"file_data_{idx}"
        filenames_vector.append(f'"{os.path.basename(file_path)}"')
        file_bytes = read_file_bytes(file_path)
        data_declarations.append(f"std::vector<uint8_t> {var_name} = {{ {', '.join(map(str, file_bytes))} }};")
        data_vectors.append(f"{var_name}")

    filenames_str = ",\n    ".join(filenames_vector)
    data_declarations_str = "\n".join(data_declarations)
    data_vectors_str = ",\n    ".join(data_vectors)

    header_content = f"""
#ifndef CH8_FILES_H
#define CH8_FILES_H

#include <vector>
#include <string>
#include <stdint.h>

#define NUM_FILES {len(files)}

const char* filenames[NUM_FILES] = {{
    {filenames_str}
}};

{data_declarations_str}

std::vector<std::vector<uint8_t>> file_data = {{
    {data_vectors_str}
}};

#endif // CH8_FILES_H
"""

    with open(output_header_path, 'w') as header_file:
        header_file.write(header_content)

def main():
    root_folder = './roms'  # Replace with the path to your folder
    output_header_path = 'roms.h'  # Replace with the desired output header file path

    ch8_files = find_ch8_files(root_folder)
    generate_cpp_header(ch8_files, output_header_path)

if __name__ == "__main__":
    main()
