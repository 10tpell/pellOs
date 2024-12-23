from os import listdir
from os.path import isfile, join, abspath, getsize, realpath, dirname

FILE_DIRECTORY_SIZE = 96
dummy_part_size = 0xffff
zero = 0

def genHeader(f, d):
    header_list = [0xF5, 0x7D, 0x00, 0x00] # magic number
    header_list += zero.to_bytes(4, 'little') # add a zero of 4 bytes for alignment
    header_list += dummy_part_size.to_bytes(8, 'little') # partition size (hardcoded for now)
    
    partition_name = "ramdisk"
    for ch in partition_name:
        header_list.append(ord(ch))
    for i in range(64 - len(partition_name)):
        header_list.append(0)

    header_list += f.to_bytes(8, "little")
    header_list += d.to_bytes(8, "little")

    # print(header_list)
    return bytes(header_list)

class directory:
    def __init__(self, path, name, children):
        self.path = path
        self.name = name
        self.children = children
        self.id = 0
    
    def add_child(self, child):
        self.children.append(child)

    def __str__(self):
        return f"path: {self.path}, name: {self.name}, children: {self.children}"
    
    def __repr__(self):
        return f"path: {self.path}, name: {self.name}, children: {self.children}"
    
    def parse(self):
        full_path = join(self.path, self.name)
        for node in listdir(full_path):
            if isfile(join(full_path, node)):
                # print(f"{full_path}/{node}")
                tmp_file = file(full_path, node)
                self.add_child(tmp_file)
            else:
                tmp_dir = directory(full_path, node, [])
                tmp_dir.parse()
                self.add_child(tmp_dir)

class file:
    def __init__(self, path, name):
        self.path = path
        self.name = name
        self.size = getsize(join(path, name))
        self.id = 0

    def __str__(self):
        return f"path: {self.path}, name: {self.name}, size: {self.size}"
    
    def __repr__(self):
        return f"path: {self.path}, name: {self.name}, size: {self.size}"

def flatten_dir(dir, start_id = 0):
    child_list = [dir]
    id = start_id
    for child in dir.children:
        child.id = id
        # print(f"Adding {child.name} with id {id}")
        if type(child) is file:
            child_list.append(child)
            id += 1
        elif type(child) is directory:
            id += 1
            (new_list, new_id) = flatten_dir(child, id)
            child_list += new_list
            id = new_id
    return (child_list, id)

def genFileDirectories(flist):
    file_entry_byte_array = bytes([])
    curr_offset = 0
    for (_, name, size, is_dir, fileId) in flist:
        # print(f"id: {fileId}, name: {name}")
        file_entry_byte_array += fileId.to_bytes(8, 'little')      # uint64_t file_id
        dir_type = 0
        if is_dir:
            dir_type = 1
        else:
            dir_type = 2
        file_entry_byte_array += dir_type.to_bytes(1, 'little')    # uint8_t file_type

        if name == '':
            name = "/"
        name_list = []
        for ch in name:
            name_list.append(ord(ch))
        for _ in range(71 - len(name)):
            name_list.append(0)
        file_entry_byte_array += bytes(name_list)               # char filename[64]

        file_entry_byte_array += curr_offset.to_bytes(8, 'little') # uint64_t data_block_offset
        file_entry_byte_array += size.to_bytes(8, 'little')        # uint64_t data_entry_size
        curr_offset += size
    return file_entry_byte_array

def genDataDirectories(flist, dlist):
    data = bytes([])
    i = 0
    for (path, name, size, isDir, _) in flist:
        filename = join(path, name)
        
        if isDir:
            for d in dlist[i].children:
                # print(f"{d.name} - id: {d.id}")
                data += d.id.to_bytes(8, 'little')
        else:
            with open(filename, 'rb') as f:
                data += bytes(f.read(size))
        i += 1
    return data



def main():
    # create a list of files and directories
    dir_path = dirname(realpath(__file__))
    root_path = abspath(join(dir_path, "../../ramdisk"))
    root_dir = directory(root_path, "", [])
    root_dir.parse()

    root_dir.id = 0
    (flat_list, _) = flatten_dir(root_dir, 1)
    
    bin_list = [None] * len(flat_list)
    dlist = [None] * len(flat_list)
    for e in flat_list:
        if type(e) is directory:
            size = len(e.children) * 8
            # print(f"id: {e.id}")
            bin_list[e.id] = (e.path, e.name, size, True, e.id)
            dlist[e.id] = e
        elif type(e) is file:
            bin_list[e.id] = (e.path, e.name, e.size, False, e.id)

    file_dir_size = len(bin_list) * FILE_DIRECTORY_SIZE
    data_dir_size = 0
    for (_, _, size, _, _) in bin_list:
        data_dir_size += size
    
    # print(f"file_dir_size: {file_dir_size}, data_dir_size: {data_dir_size}")

    byteArray = genHeader(file_dir_size, data_dir_size)
    byteArray += genFileDirectories(bin_list)
    byteArray += genDataDirectories(bin_list, dlist)

    with open(join(dir_path, '../../bin/ramdisk.bin'), 'wb') as rd:
        rd.write(byteArray)

    print("Written RAMDISK to file")

if __name__ == "__main__":
    main()