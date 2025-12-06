import sys

def xor_encrypt(data: bytes, key: bytes) -> bytes:
    """
    Applies XOR encryption to the given data using the provided key.

    data : bytes
        Raw binary data to encrypt.
    key : bytes
        XOR key that will be repeated throughout the data.

    Returns
    -------
    bytes
        Encrypted (or decrypted) byte sequence.
    """
    encrypted = bytearray()
    key_length = len(key)

    for index, byte in enumerate(data):
        key_byte = key[index % key_length]
        encrypted.append(byte ^ key_byte)

    return bytes(encrypted)


def main():
    # Ensure a file was provided
    if len(sys.argv) < 2:
        print("Usage: python xorencrypt.py <input_file>")
        return

    input_filename = sys.argv[1]

    # Read input file in binary mode
    try:
        with open(input_filename, "rb") as file:
            plaintext = file.read()
    except FileNotFoundError:
        print(f"[-] Error: File '{input_filename}' not found.")
        return

    # XOR encryption key (change as needed)
    key = b"key"

    # Encrypt the file contents
    ciphertext = xor_encrypt(plaintext, key)

    # Create output filename
    output_filename = f"{input_filename.rsplit('.', 1)[0]}_encrypted.bin"

    # Write encrypted data to output file
    with open(output_filename, "wb") as file:
        file.write(ciphertext)

    print(f"[+] XOR encryption complete: {output_filename}")


if __name__ == "__main__":
    main()
