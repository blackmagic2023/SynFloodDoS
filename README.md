# Multi-threaded TCP SYN Flooder

A multi-threaded TCP SYN flood tool designed to stress-test networks by sending a flood of SYN packets to overwhelm a target's connection queue. This tool allows users to specify the number of threads for sending SYN packets concurrently, enhancing the speed and intensity of the attack. 

⚠️ **Important:** This tool is intended for educational purposes and authorized network testing **only**. Unauthorized use of this tool is illegal and unethical.

## Features
- Sends raw TCP SYN packets to a target IP and port.
- Multi-threaded for enhanced speed and efficiency.
- User-friendly CLI for packet customization.
- Signal handling (Ctrl+C) to stop the attack gracefully.

## Requirements
- C compiler (GCC recommended)
- Root/Administrator privileges (required for raw sockets)

## Compilation

To compile the program, ensure you have `gcc` installed, then run:

```bash
gcc -o synflood synflood.c -lpthread
```

# Usage

To run the tool, you need to have root or administrator privileges. Example usage:

```bash
sudo ./synflood
```

You will be prompted to enter:

   - The remote host's IP address.
   - The target port.
   - The number of threads to use for the attack.

# Example

```
Enter the remote host's IP address: 192.168.1.100
Enter the remote host's port: 80
Enter the number of threads to use: 10
```

The tool will then start sending SYN packets to the target, utilizing the specified number of threads.

# Ethical Considerations

This tool should only be used in controlled environments where you have explicit permission from the network owner. Unauthorized usage can lead to serious legal consequences.

# License

This project is licensed under the MIT License. See the `LICENSE` file for details.

# Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

# Disclaimer

This software is provided "as-is" without any warranties. The author is not responsible for any damages caused by the misuse of this tool.
