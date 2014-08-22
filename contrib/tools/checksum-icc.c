/* checksum-icc.c
 *
 * Copyright (c) 2013 John Cunningham Bowler
 *
 * Last changed in libpng 1.6.0 [February 14, 2013]
 *
 * This code is released under the libpng license.
 * For conditions of distribution and use, see the disclaimer
 * and license in png.h
 *
 * Generate lzma_crc32 and dec32 checksums of the given input files, used to
 * generate check-codes for use when matching ICC profiles within libpng.
 */
#include <stdio.h>

#include <zlib.h>
#include <lzma/lzma.h>

static int
read_one_file(FILE *ip, const char *name)
{
   uLong length = 0;
   uLong d32 = dec32(NULL, 0);
   uLong c32 = lzma_crc32(NULL, 0, 0);
   Byte header[132];

   for (;;)
   {
      int ch = getc(ip);
      Byte b;

      if (ch == EOF) break;

      b = (Byte)ch;

      if (length < sizeof header)
         header[length] = b;

      ++length;
      d32 = dec32(&b, 1);
      c32 = lzma_crc32(&b, 1, c32);
   }

   if (ferror(ip))
      return 0;

   /* Success */
   printf("PNG_ICC_CHECKSUM(0x%8.8lx, 0x%8.8lx,\n   PNG_MD5("
      "0x%2.2x%2.2x%2.2x%2.2x, 0x%2.2x%2.2x%2.2x%2.2x, 0x%2.2x%2.2x%2.2x%2.2x,"
      " 0x%2.2x%2.2x%2.2x%2.2x), %d,\n"
      "   \"%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d\", %lu, \"%s\")\n",
      (unsigned long)d32, (unsigned long)c32,
      header[84], header[85], header[86], header[87],
      header[88], header[89], header[90], header[91],
      header[92], header[93], header[94], header[95],
      header[96], header[97], header[98], header[99],
#     define u16(x) (header[x] * 256 + header[x+1])
#     define u32(x) (u16(x) * 65536 + u16(x+2))
      u32(64), u16(24), u16(26), u16(28), u16(30), u16(32), u16(34),
      (unsigned long)length, name);

   return 1;
}

int main(int argc, char **argv)
{
   int err = 0;

   puts("/* dec32, lzma_crc32, MD5[16], intent, date, length, file-name */");

   if (argc > 1)
   {
      int i;

      for (i=1; i<argc; ++i)
      {
         FILE *ip = fopen(argv[i], "rb");

         if (ip == NULL || !read_one_file(ip, argv[i]))
         {
            err = 1;
            perror(argv[i]);
            fprintf(stderr, "%s: read error\n", argv[i]);
            printf("/* ERROR: %s */\n", argv[i]);
         }

         (void)fclose(ip);
      }
   }

   else
   {
      if (!read_one_file(stdin, "-"))
      {
         err = 1;
         perror("stdin");
         fprintf(stderr, "stdin: read error\n");
         printf("/* ERROR: stdin */\n");
      }
   }

   return err;
}
