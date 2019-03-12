import argparse
from sys import stdin, stdout


def read_chunk(fd, chunk_width):
    """
    Reads a chunk from fd
    """
    chunk = fd.read(chunk_width)
    if type(chunk) == str:
        chunk = chunk.encode(encoding="ascii")
    return chunk


def append_chunk(fd, chunk):
    """
    Write a chunk to fd
    """
    if fd == stdout:
        stdout.buffer.write(chunk)
        stdout.flush()
        return
    fd.write(chunk)


def xor_chunk(data_chunk, key_chunk):
    """
    XOR two chunks
    """
    encrypted_chunk = bytearray()
    for i in range(len(data_chunk)):
        encrypted_chunk.append(data_chunk[i] ^ key_chunk[i])
    return encrypted_chunk


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='XOR encryption.')
    parser.add_argument("-i", dest="data_file", default=None,
            help="The file to de- or encrypt. Ommit to use stdin.")
    parser.add_argument("-k", dest="key_file", required=True,
            help="The key file to use for encryption")
    parser.add_argument("-o", dest="out_file", default=None,
            help="The file to write to. Ommit to use stdout.")
    parser.add_argument("--chunk_width", default=1024,  type=int,
            help="The width of the chunks to read and write")
    args = parser.parse_args()

    data_fd = None
    out_fd = None
    key_fd = None
    if args.data_file is not None:
        try:
            data_fd = open(args.data_file, "rb")
        except FileNotFoundError:
            print("Could not open: " + str(args.data_file))
            exit(-1)
    else:
        data_fd = stdin

    if args.out_file is not None:
        try:
            out_fd = open(args.out_file, "wb")
        except FileNotFoundError:
            print("Could not open: " + str(args.out_file))
            exit(-1)
    else:
        out_fd = stdout

    try:
        key_fd = open(args.key_file, "rb")
    except FileNotFoundError:
        print("Could not open: " + str(args.key_file))
        exit(-1)

    data_chunk = "magic"
    while len(data_chunk) > 0 and data_fd.closed is False:
        data_chunk = read_chunk(data_fd, args.chunk_width)
        key_chunk = read_chunk(key_fd, args.chunk_width)
        encrypted_chunk = xor_chunk(data_chunk, key_chunk)
        append_chunk(out_fd, encrypted_chunk)
