# TCP Receiver Implementation

## Overview
This repository contains the implementation of a TCP receiver as part of an assignment for CSE 232. The assignment is structured in three parts:
1. Building a `ByteStream` class.
2. Implementing a `Reassembler` for byte segments.
3. Creating a `TCPReceiver` that integrates both components and handles TCP sequence numbers.

## Components

### Part I: ByteStream
The `ByteStream` class serves as a container for storing bytes, which can be written to and read from. Key properties include:
- Finite capacity.
- Sequential byte writing and reading.
- Handling of end-of-file (EOF).

### Part II: Reassembler
The `Reassembler` class is responsible for assembling received byte segments into the correct order before writing them to the `ByteStream`. It manages:
- Buffered bytes that are out of order.
- Discarding bytes beyond the stream’s capacity.

### Part III: TCPReceiver
The `TCPReceiver` class integrates the `ByteStream` and `Reassembler`, managing TCP sequence numbers and handling:
- Synchronization (SYN) and Finish (FIN) flags.
- Sequence number wrapping.
- Acknowledgment numbers and window sizes for flow control.
