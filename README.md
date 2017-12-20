# Compressor
Server based compressor created in c. Developed by Zach Clewell and [Aditya Pillai](https://github.com/adityapillai)

## Purpose
Developed as a way to gain experience with networking as well as multithreaded applications

## Use Case
1. User uploads file to be compressed to server instance
2. Server makes a local copy of this file
3. Server runs several compression alogrithms in parallel
4. Server selects most efficient algorithm and sends compressed file back to client

## Algorithms Used
We did not design these algorithms however we did implement them ourselves  
  * [Huffman Encoding](https://en.wikipedia.org/wiki/Huffman_coding)
  * [LZW](https://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch)
  * [Run Length](https://en.wikipedia.org/wiki/Run-length_encoding)

## Libraries Used
[glib](https://developer.gnome.org/glib/) - Imported data types so we didn't have to write them ourselves
