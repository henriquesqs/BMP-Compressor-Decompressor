# BMP IMAGE COMPRESSION AND DESCOMPRESSION
This project allows you to compress and descompress a BMP image file (with some requisites of size) with <b>24 bits for colors</b>. This compressor and descompressor uses the **JPEG format** to apply compression and descompression and **uses C11 languague**.

### Image requisites
As said before, your image must accomplish this requisites listed below:
- Your image must has 24 bits for colors;
- Your image dimensions must be multiple of 8;
- Minimum allowed dimension is 8x8 pixels;
- Max allowed dimension is 1280 x 800 pixels.

### Instructions
- Install gcc;
- Install make;
- Place the image you want to compress/ descompress in **root folder** OR **images folder**;
- Compile the source code by entering `make all` in your terminal;
- Run the program by entering `make run`;
- Follow the instructions the program will show you.

### Credits
- Dennis L. Green ([@DennisLemkeGreen](https://github.com/DennisLemkeGreen));
- Henrique dos Santos ([@henriquesqs](https://github.com/henriquesqs));
