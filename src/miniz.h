/* miniz.c v1.16 beta r1 - public domain deflate/inflate, zlib-subset, ZIP reading/writing/appending, PNG writing
   See "unlicense" statement at the end of this file.
   Rich Geldreich <richgel99@gmail.com>, last updated Oct. 13, 2013
   Implements RFC 1950: http://www.ietf.org/rfc/rfc1950.txt and RFC 1951: http://www.ietf.org/rfc/rfc1951.txt

   Most API's defined in miniz.c are optional. For example, to disable the archive related functions just define
   MINIZ_NO_ARCHIVE_APIS, or to get rid of all stdio usage define MINIZ_NO_STDIO (see the list below for more macros).

   * Low-level Deflate/Inflate implementation notes:

     Compression: Use the "tdefl" API's. The compressor supports raw, static, and dynamic blocks, lazy or
     greedy parsing, match length filtering, RLE-only, and Huffman-only streams. It performs and compresses
     approximately as well as zlib.

     Decompression: Use the "tinfl" API's. The entire decompressor is implemented as a single function
     coroutine: see tinfl_decompress(). It supports decompression into a 32KB (or larger power of 2) wrapping buffer, or into a memory
     block large enough to hold the entire file.

     The low-level tdefl/tinfl API's do not make any use of dynamic memory allocation.

   * zlib-style API notes:

     miniz.c implements a fairly large subset of zlib. There's enough functionality present for it to be a drop-in
     zlib replacement in many apps:
        The z_stream struct, optional memory allocation callbacks
        deflateInit/deflateInit2/deflate/deflateReset/deflateEnd/deflateBound
        inflateInit/inflateInit2/inflate/inflateEnd
        compress, compress2, compressBound, uncompress
        CRC-32, Adler-32 - Using modern, minimal code size, CPU cache friendly routines.
        Supports raw deflate streams or standard zlib streams with adler-32 checking.

     Limitations:
      The callback API's are not implemented yet. No support for gzip headers or zlib static dictionaries.
      I've tried to closely emulate zlib's various flavors of stream flushing and return status codes, but
      there are no guarantees that miniz.c pulls this off perfectly.

   * PNG writing: See the tdefl_write_image_to_png_file_in_memory() function, originally written by
     Alex Evans. Supports 1-4 bytes/pixel images.

   * ZIP archive API notes:

     The ZIP archive API's where designed with simplicity and efficiency in mind, with just enough abstraction to
     get the job done with minimal fuss. There are simple API's to retrieve file information, read files from
     existing archives, create new archives, append new files to existing archives, or clone archive data from
     one archive to another. It supports archives located in memory or the heap, on disk (using stdio.h),
     or you can specify custom file read/write callbacks.

     - Archive reading: Just call this function to read a single file from a disk archive:

      void *zip_extract_archive_file_to_heap(const char *pZip_filename, const char *pArchive_name,
        size_t *pSize, uint zip_flags);

     For more complex cases, use the "zip_reader" functions. Upon opening an archive, the entire central
     directory is located and read as-is into memory, and subsequent file access only occurs when reading individual files.

     - Archives file scanning: The simple way is to use this function to scan a loaded archive for a specific file:

     int zip_reader_locate_file(zip_archive *pZip, const char *pName, const char *pComment, zip_flags);

     The locate operation can optionally check file comments too, which (as one example) can be used to identify
     multiple versions of the same file in an archive. This function uses a simple linear search through the central
     directory, so it's not very fast.

     Alternately, you can iterate through all the files in an archive (using zip_reader_get_num_files()) and
     retrieve detailed info on each file by calling zip_reader_file_stat().

     - Archive creation: Use the "zip_writer" functions. The ZIP writer immediately writes compressed file data
     to disk and builds an exact image of the central directory in memory. The central directory image is written
     all at once at the end of the archive file when the archive is finalized.

     The archive writer can optionally align each file's local header and file data to any power of 2 alignment,
     which can be useful when the archive will be read from optical media. Also, the writer supports placing
     arbitrary data blobs at the very beginning of ZIP archives. Archives written using either feature are still
     readable by any ZIP tool.

     - Archive appending: The simple way to add a single file to an archive is to call this function:

      bool zip_add_mem_to_archive_file_in_place(const char *pZip_filename, const char *pArchive_name,
        const void *pBuf, size_t buf_size, const void *pComment, uint16 comment_size, uint level_and_flags);

     The archive will be created if it doesn't already exist, otherwise it'll be appended to.
     Note the appending is done in-place and is not an atomic operation, so if something goes wrong
     during the operation it's possible the archive could be left without a central directory (although the local
     file headers and file data will be fine, so the archive will be recoverable).

     For more complex archive modification scenarios:
     1. The safest way is to use a zip_reader to read the existing archive, cloning only those bits you want to
     preserve into a new archive using using the zip_writer_add_from_zip_reader() function (which compiles the
     compressed file data as-is). When you're done, delete the old archive and rename the newly written archive, and
     you're done. This is safe but requires a bunch of temporary disk space or heap memory.

     2. Or, you can convert an zip_reader in-place to an zip_writer using zip_writer_init_from_reader(),
     append new files as needed, then finalize the archive which will write an updated central directory to the
     original archive. (This is basically what zip_add_mem_to_archive_file_in_place() does.) There's a
     possibility that the archive's central directory could be lost with this method if anything goes wrong, though.

     - ZIP archive support limitations:
     No zip64 or spanning support. Extraction functions can only handle unencrypted, stored or deflated files.
     Requires streams capable of seeking.

   * This is a header file library, like stb_image.c. To get only a header file, either cut and paste the
     below header, or create miniz.h, #define MINIZ_HEADER_FILE_ONLY, and then include miniz.c from it.

   * Important: For best perf. be sure to customize the below macros for your target platform:
     #define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 1
     #define MINIZ_LITTLE_ENDIAN 1
     #define MINIZ_HAS_64BIT_REGISTERS 1

   * On platforms using glibc, Be sure to "#define _LARGEFILE64_SOURCE 1" before including miniz.c to ensure miniz
     uses the 64-bit variants: fopen64(), stat64(), etc. Otherwise you won't be able to process large files
     (i.e. 32-bit stat() fails for me on files > 0x7FFFFFFF bytes).
*/
#pragma once


/* Defines to completely disable specific portions of miniz.c: 
   If all macros here are defined the only functionality remaining will be CRC-32, adler-32, tinfl, and tdefl. */

/* Define MINIZ_NO_STDIO to disable all usage and any functions which rely on stdio for file I/O. */
/*#define MINIZ_NO_STDIO */

/* If MINIZ_NO_TIME is specified then the ZIP archive functions will not be able to get the current time, or */
/* get/set file times, and the C run-time funcs that get/set times won't be called. */
/* The current downside is the times written to your archives will be from 1979. */
/*#define MINIZ_NO_TIME */

/* Define MINIZ_NO_ARCHIVE_APIS to disable all ZIP archive API's. */
/*#define MINIZ_NO_ARCHIVE_APIS */

/* Define MINIZ_NO_ARCHIVE_WRITING_APIS to disable all writing related ZIP archive API's. */
/*#define MINIZ_NO_ARCHIVE_WRITING_APIS */

/* Define MINIZ_NO_ZLIB_COMPATIBLE_NAME to disable zlib names, to prevent conflicts against stock zlib. */
/*#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES */

/* Define MINIZ_NO_MALLOC to disable all calls to malloc, free, and realloc. 
   Note if MINIZ_NO_MALLOC is defined then the user must always provide custom user alloc/free/realloc
   callbacks to the zlib and archive API's, and a few stand-alone helper API's which don't provide custom user
   functions (such as tdefl_compress_mem_to_heap() and tinfl_decompress_mem_to_heap()) won't work. */
/*#define MINIZ_NO_MALLOC */

#if defined(__TINYC__) && (defined(__linux) || defined(__linux__))
/* TODO: Work around "error: include file 'sys\utime.h' when compiling with tcc on Linux */
#define MINIZ_NO_TIME
#endif

#include <cstddef>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#if !defined(MINIZ_NO_TIME) && !defined(MINIZ_NO_ARCHIVE_APIS)
#include <ctime>
#endif


namespace miniz {

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__i386) || defined(__i486__) || defined(__i486) || defined(i386) || defined(__ia64__) || defined(__x86_64__)
/* MINIZ_X86_OR_X64_CPU is only used to help set the below macros. */
#define MINIZ_X86_OR_X64_CPU 1
#endif

#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || MINIZ_X86_OR_X64_CPU
/* Set MINIZ_LITTLE_ENDIAN to 1 if the processor is little endian. */
#define MINIZ_LITTLE_ENDIAN 1
#endif

#if MINIZ_X86_OR_X64_CPU
/* Set MINIZ_USE_UNALIGNED_LOADS_AND_STORES to 1 on CPU's that permit efficient integer loads and stores from unaligned addresses. */
#define MINIZ_USE_UNALIGNED_LOADS_AND_STORES 1
#endif

#if defined(_M_X64) || defined(_WIN64) || defined(__MINGW64__) || defined(_LP64) || defined(__LP64__) || defined(__ia64__) || defined(__x86_64__)
/* Set MINIZ_HAS_64BIT_REGISTERS to 1 if operations on 64-bit integers are reasonably fast (and don't involve compiler generated calls to helper functions). */
#define MINIZ_HAS_64BIT_REGISTERS 1
#endif

/* ------------------- zlib-style API Definitions. */

/* For more compatibility with zlib, miniz.c uses unsigned long for some parameters/struct members. Beware: ulong can be either 32 or 64-bits! */
using ulong = unsigned long;

/* free() internally uses the MZ_FREE() macro (which by default calls free() unless you've modified the MZ_MALLOC macro) to release a block allocated from the heap. */
void mz_free(void *p);

#define MZ_ADLER32_INIT (1)
/* adler32() returns the initial adler-32 value to use when called with ptr==NULL. */
ulong adler32(ulong adler, const unsigned char *ptr, size_t buf_len);

#define MZ_CRC32_INIT (0)
/* crc32() returns the initial CRC-32 value to use when called with ptr==NULL. */
ulong crc32(ulong crc, const unsigned char *ptr, size_t buf_len);



//	basic types
using uint8  = unsigned char ;
using int16  = signed short  ;
using uint16 = unsigned short;
using uint32 = unsigned int  ;
using uint   = unsigned int  ;
using int64  = int64_t       ;
using uint64 = uint64_t      ;


/* Compression strategies. */
enum class compression_strategy
{
    DEFAULT = 0,
    FILTERED = 1,
    HUFFMAN_ONLY = 2,
    RLE = 3,
    FIXED = 4
};

/* Method */
enum class compression_method : uint16
{
	NO_METHOD = 0,
	DEFLATED = 8
};

/* Heap allocation callbacks.
Note that alloc_func parameter types purpsosely differ from zlib's: items/size is size_t, not unsigned long. */
typedef void *(*alloc_func)(void *opaque, size_t items, size_t size);
typedef void(*free_func)(void *opaque, void *address);
typedef void *(*realloc_func)(void *opaque, void *address, size_t items, size_t size);

/* Compression levels: 0-9 are the standard zlib-style levels, 10 is best possible compression (not zlib compatible, and may be very slow), MZ_DEFAULT_COMPRESSION=MZ_DEFAULT_LEVEL. */
struct compression_level {
	enum level : int {
		NO_COMPRESSION = 0,
		BEST_SPEED = 1,
		BEST_COMPRESSION = 9,
		UBER_COMPRESSION = 10,
		DEFAULT_LEVEL = 6,
		DEFAULT_COMPRESSION = -1
	} lv;

	compression_level(int lvval) : lv((level)lvval) {}

	compression_level(level lv) : lv(lv) {}

	bool operator >(const compression_level& clv) const noexcept {
		return clv.lv > lv;
	}

	bool operator ==(const compression_level& clv) const noexcept {
		return clv.lv == lv;
	}

	explicit operator uint() const noexcept {
		return lv;
	}

	explicit operator int() const noexcept {
		return lv;
	}

};

constexpr auto VERSION = "10.0.0";
constexpr auto VERNUM = 0xA000;
constexpr auto VER_MAJOR = 10;
constexpr auto VER_MINOR = 0;
constexpr auto VER_REVISION = 0;
constexpr auto VER_SUBREVISION = 0;


/* Flush values. For typical usage you only need MZ_NO_FLUSH and MZ_FINISH. The other values are for advanced use (refer to the zlib docs). */
enum class flush_type
{
    NO_FLUSH = 0,
    PARTIAL_FLUSH = 1,
    SYNC_FLUSH = 2,
    FULL_FLUSH = 3,
    FINISH = 4,
    BLOCK = 5
};

/* Return status codes. MZ_PARAM_ERROR is non-standard. */
enum class return_status
{
    OK = 0,
    STREAM_END = 1,
    NEED_DICT = 2,
    ERRNO = -1,
    STREAM_ERROR = -2,
    DATA_ERROR = -3,
    MEM_ERROR = -4,
    BUF_ERROR = -5,
    VERSION_ERROR = -6,
    PARAM_ERROR = -10000
};

/* Window bits */
enum class window_bits
{
	DEFAULT = 15,
	DEFAULT_NEGATIVE = -DEFAULT
};

using memory_level = int;

struct internal_state;

/* Compression/decompression stream struct. */
struct stream
{
    const unsigned char *next_in; /* pointer to next byte to read */
    unsigned int avail_in;        /* number of bytes available at next_in */
    ulong total_in;            /* total number of bytes consumed so far */

    unsigned char *next_out; /* pointer to next byte to write */
    unsigned int avail_out;  /* number of bytes that can be written to next_out */
    ulong total_out;      /* total number of bytes produced so far */

    char *msg;                       /* error msg (unused) */
    struct internal_state *state; /* internal state, allocated by zalloc/zfree */

    alloc_func zalloc; /* optional heap allocation function (defaults to malloc) */
    free_func zfree;   /* optional heap free function (defaults to free) */
    void *opaque;         /* heap alloc function user pointer */

    int data_type;     /* data_type (unused) */
    ulong adler;    /* adler32 of the source or uncompressed data */
    ulong reserved; /* not used */
};

using stream_ptr = stream*;

/* Returns the version string of miniz.c. */
const char *version(void);

/* deflateInit() initializes a compressor with default options: */
/* Parameters: */
/*  pStream must point to an initialized stream struct. */
/*  level must be between [MZ_NO_COMPRESSION, MZ_BEST_COMPRESSION]. */
/*  level 1 enables a specially optimized compression function that's been optimized purely for performance, not ratio. */
/*  (This special func. is currently only enabled when MINIZ_USE_UNALIGNED_LOADS_AND_STORES and MINIZ_LITTLE_ENDIAN are defined.) */
/* Return values: */
/*  MZ_OK on success. */
/*  MZ_STREAM_ERROR if the stream is bogus. */
/*  MZ_PARAM_ERROR if the input parameters are bogus. */
/*  MZ_MEM_ERROR on out of memory. */
return_status deflateInit(stream_ptr, compression_level);

/* deflateInit2() is like deflate(), except with more control: */
/* Additional parameters: */
/*   method must be MZ_DEFLATED */
/*   window_bits must be MZ_DEFAULT_WINDOW_BITS (to wrap the deflate stream with zlib header/adler-32 footer) or -MZ_DEFAULT_WINDOW_BITS (raw deflate/no header or footer) */
/*   mem_level must be between [1, 9] (it's checked but ignored by miniz.c) */
return_status deflateInit2(stream_ptr, compression_level, compression_method, window_bits, memory_level, compression_strategy);

/* Quickly resets a compressor without having to reallocate anything. Same as calling deflateEnd() followed by deflateInit()/deflateInit2(). */
return_status deflateReset(stream_ptr);

/* deflate() compresses the input to output, consuming as much of the input and producing as much output as possible. */
/* Parameters: */
/*   pStream is the stream to read from and write to. You must initialize/update the next_in, avail_in, next_out, and avail_out members. */
/*   flush may be MZ_NO_FLUSH, MZ_PARTIAL_FLUSH/MZ_SYNC_FLUSH, MZ_FULL_FLUSH, or MZ_FINISH. */
/* Return values: */
/*   MZ_OK on success (when flushing, or if more input is needed but not available, and/or there's more output to be written but the output buffer is full). */
/*   MZ_STREAM_END if all input has been consumed and all output bytes have been written. Don't call deflate() on the stream anymore. */
/*   MZ_STREAM_ERROR if the stream is bogus. */
/*   MZ_PARAM_ERROR if one of the parameters is invalid. */
/*   MZ_BUF_ERROR if no forward progress is possible because the input and/or output buffers are empty. (Fill up the input buffer or free up some output space and try again.) */
return_status deflate(stream_ptr pStream, flush_type flush);

/* deflateEnd() deinitializes a compressor: */
/* Return values: */
/*  MZ_OK on success. */
/*  MZ_STREAM_ERROR if the stream is bogus. */
return_status deflateEnd(stream_ptr pStream);

/* deflateBound() returns a (very) conservative upper bound on the amount of data that could be generated by deflate(), assuming flush is set to only MZ_NO_FLUSH or MZ_FINISH. */
ulong deflateBound(stream_ptr pStream, ulong source_len);

/* Single-call compression functions compress() and compress2(): */
/* Returns MZ_OK on success, or one of the error codes from deflate() on failure. */
return_status compress(unsigned char *pDest, ulong *pDest_len, const unsigned char *pSource, ulong source_len);
return_status compress2(unsigned char *pDest, ulong *pDest_len, const unsigned char *pSource, ulong source_len, compression_level);

/* compressBound() returns a (very) conservative upper bound on the amount of data that could be generated by calling compress(). */
ulong compressBound(ulong source_len);

/* Initializes a decompressor. */
return_status inflateInit(stream_ptr);

/* inflateInit2() is like inflateInit() with an additional option that controls the window size and whether or not the stream has been wrapped with a zlib header/footer: */
/* window_bits must be MZ_DEFAULT_WINDOW_BITS (to parse zlib header/footer) or -MZ_DEFAULT_WINDOW_BITS (raw deflate). */
return_status inflateInit2(stream_ptr, window_bits);

/* Decompresses the input stream to the output, consuming only as much of the input as needed, and writing as much to the output as possible. */
/* Parameters: */
/*   pStream is the stream to read from and write to. You must initialize/update the next_in, avail_in, next_out, and avail_out members. */
/*   flush may be MZ_NO_FLUSH, MZ_SYNC_FLUSH, or MZ_FINISH. */
/*   On the first call, if flush is MZ_FINISH it's assumed the input and output buffers are both sized large enough to decompress the entire stream in a single call (this is slightly faster). */
/*   MZ_FINISH implies that there are no more source bytes available beside what's already in the input buffer, and that the output buffer is large enough to hold the rest of the decompressed data. */
/* Return values: */
/*   MZ_OK on success. Either more input is needed but not available, and/or there's more output to be written but the output buffer is full. */
/*   MZ_STREAM_END if all needed input has been consumed and all output bytes have been written. For zlib streams, the adler-32 of the decompressed data has also been verified. */
/*   MZ_STREAM_ERROR if the stream is bogus. */
/*   MZ_DATA_ERROR if the deflate stream is invalid. */
/*   MZ_PARAM_ERROR if one of the parameters is invalid. */
/*   MZ_BUF_ERROR if no forward progress is possible because the input buffer is empty but the inflater needs more input to continue, or if the output buffer is not large enough. Call inflate() again */
/*   with more input data, or with more room in the output buffer (except when using single call decompression, described above). */
return_status inflate(stream_ptr, flush_type);

/* Deinitializes a decompressor. */
return_status inflateEnd(stream_ptr pStream);

/* Single-call decompression. */
/* Returns MZ_OK on success, or one of the error codes from inflate() on failure. */
return_status uncompress(unsigned char *pDest, ulong *pDest_len, const unsigned char *pSource, ulong source_len);

/* Returns a string description of the specified error code, or NULL if the error code is invalid. */
const char* error(return_status err);


using Bytef = unsigned char;




/* Works around MSVC's spammy "warning C4127: conditional expression is constant" message. */
#ifdef _MSC_VER
#define MZ_MACRO_END while (0, 0)
#else
#define MZ_MACRO_END while (0)
#endif

#ifdef MINIZ_NO_STDIO
using FILE = void *;
#else
#include <stdio.h>
#endif /* #ifdef MINIZ_NO_STDIO */

#ifdef MINIZ_NO_TIME
struct time_t
{
    int m_dummy;
};
#endif

#define MZ_ASSERT(x) assert(x)

#ifdef MINIZ_NO_MALLOC
#define MZ_MALLOC(x) NULL
#define MZ_FREE(x) (void) x, ((void)0)
#define MZ_REALLOC(p, x) NULL
#else
#define MZ_MALLOC(x) malloc(x)
#define MZ_FREE(x) free(x)
#define MZ_REALLOC(p, x) realloc(p, x)
#endif

#define MZ_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MZ_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MZ_CLEAR_OBJ(obj) memset(&(obj), 0, sizeof(obj))

#if MINIZ_USE_UNALIGNED_LOADS_AND_STORES &&MINIZ_LITTLE_ENDIAN
#define MZ_READ_LE16(p) *((const uint16 *)(p))
#define MZ_READ_LE32(p) *((const uint32 *)(p))
#else
#define MZ_READ_LE16(p) ((uint32)(((const uint8 *)(p))[0]) | ((uint32)(((const uint8 *)(p))[1]) << 8U))
#define MZ_READ_LE32(p) ((uint32)(((const uint8 *)(p))[0]) | ((uint32)(((const uint8 *)(p))[1]) << 8U) | ((uint32)(((const uint8 *)(p))[2]) << 16U) | ((uint32)(((const uint8 *)(p))[3]) << 24U))
#endif

#define MZ_READ_LE64(p) (((uint64)MZ_READ_LE32(p)) | (((uint64)MZ_READ_LE32((const uint8 *)(p) + sizeof(uint32))) << 32U))

#ifdef _MSC_VER
#define MZ_FORCEINLINE __forceinline
#elif defined(__GNUC__)
#define MZ_FORCEINLINE __inline__ __attribute__((__always_inline__))
#else
#define MZ_FORCEINLINE inline
#endif


extern void *miniz_def_alloc_func(void *opaque, size_t items, size_t size);
extern void miniz_def_free_func(void *opaque, void *address);
extern void *miniz_def_realloc_func(void *opaque, void *address, size_t items, size_t size);

#define MZ_UINT16_MAX (0xFFFFU)
#define MZ_UINT32_MAX (0xFFFFFFFFU)




/* ------------------- Low-level Compression API Definitions */

/* Set TDEFL_LESS_MEMORY to 1 to use less memory (compression will be slightly slower, and raw/dynamic blocks will be output more frequently). */
#define TDEFL_LESS_MEMORY 0

/* tdefl_init() compression flags logically OR'd together (low 12 bits contain the max. number of probes per dictionary search): */
/* TDEFL_DEFAULT_MAX_PROBES: The compressor defaults to 128 dictionary probes per dictionary search. 0=Huffman only, 1=Huffman+LZ (fastest/crap compression), 4095=Huffman+LZ (slowest/best compression). */
enum
{
    TDEFL_HUFFMAN_ONLY = 0,
    TDEFL_DEFAULT_MAX_PROBES = 128,
    TDEFL_MAX_PROBES_MASK = 0xFFF
};

/* TDEFL_WRITE_ZLIB_HEADER: If set, the compressor outputs a zlib header before the deflate data, and the Adler-32 of the source data at the end. Otherwise, you'll get raw deflate data. */
/* TDEFL_COMPUTE_ADLER32: Always compute the adler-32 of the input data (even when not writing zlib headers). */
/* TDEFL_GREEDY_PARSING_FLAG: Set to use faster greedy parsing, instead of more efficient lazy parsing. */
/* TDEFL_NONDETERMINISTIC_PARSING_FLAG: Enable to decrease the compressor's initialization time to the minimum, but the output may vary from run to run given the same input (depending on the contents of memory). */
/* TDEFL_RLE_MATCHES: Only look for RLE matches (matches with a distance of 1) */
/* TDEFL_FILTER_MATCHES: Discards matches <= 5 chars if enabled. */
/* TDEFL_FORCE_ALL_STATIC_BLOCKS: Disable usage of optimized Huffman tables. */
/* TDEFL_FORCE_ALL_RAW_BLOCKS: Only use raw (uncompressed) deflate blocks. */
/* The low 12 bits are reserved to control the max # of hash probes per dictionary lookup (see TDEFL_MAX_PROBES_MASK). */
enum 
{
    TDEFL_WRITE_ZLIB_HEADER = 0x01000,
    TDEFL_COMPUTE_ADLER32 = 0x02000,
    TDEFL_GREEDY_PARSING_FLAG = 0x04000,
    TDEFL_NONDETERMINISTIC_PARSING_FLAG = 0x08000,
    TDEFL_RLE_MATCHES = 0x10000,
    TDEFL_FILTER_MATCHES = 0x20000,
    TDEFL_FORCE_ALL_STATIC_BLOCKS = 0x40000,
    TDEFL_FORCE_ALL_RAW_BLOCKS = 0x80000
};

/* High level compression functions: */
/* tdefl_compress_mem_to_heap() compresses a block in memory to a heap block allocated via malloc(). */
/* On entry: */
/*  pSrc_buf, src_buf_len: Pointer and size of source block to compress. */
/*  flags: The max match finder probes (default is 128) logically OR'd against the above flags. Higher probes are slower but improve compression. */
/* On return: */
/*  Function returns a pointer to the compressed data, or NULL on failure. */
/*  *pOut_len will be set to the compressed data's size, which could be larger than src_buf_len on uncompressible data. */
/*  The caller must free() the returned block when it's no longer needed. */
void* tdefl_compress_mem_to_heap(const void *pSrc_buf, size_t src_buf_len, size_t *pOut_len, int flags);

/* tdefl_compress_mem_to_mem() compresses a block in memory to another block in memory. */
/* Returns 0 on failure. */
size_t tdefl_compress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pSrc_buf, size_t src_buf_len, int flags);

/* Compresses an image to a compressed PNG file in memory. */
/* On entry: */
/*  pImage, w, h, and num_chans describe the image to compress. num_chans may be 1, 2, 3, or 4. */
/*  The image pitch in bytes per scanline will be w*num_chans. The leftmost pixel on the top scanline is stored first in memory. */
/*  level may range from [0,10], use MZ_NO_COMPRESSION, MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc. or a decent default is MZ_DEFAULT_LEVEL */
/*  If flip is true, the image will be flipped on the Y axis (useful for OpenGL apps). */
/* On return: */
/*  Function returns a pointer to the compressed data, or NULL on failure. */
/*  *pLen_out will be set to the size of the PNG image file. */
/*  The caller must free() the returned heap block (which will typically be larger than *pLen_out) when it's no longer needed. */
void* tdefl_write_image_to_png_file_in_memory_ex(const void *pImage, int w, int h, int num_chans, size_t *pLen_out, uint level, bool flip);
void* tdefl_write_image_to_png_file_in_memory(const void *pImage, int w, int h, int num_chans, size_t *pLen_out);

/* Output stream interface. The compressor uses this interface to write compressed data. It'll typically be called TDEFL_OUT_BUF_SIZE at a time. */
typedef bool (*tdefl_put_buf_func_ptr)(const void *pBuf, int len, void *pUser);

/* tdefl_compress_mem_to_output() compresses a block to an output stream. The above helpers use this function internally. */
bool tdefl_compress_mem_to_output(const void *pBuf, size_t buf_len, tdefl_put_buf_func_ptr pPut_buf_func, void *pPut_buf_user, int flags);

enum
{
    TDEFL_MAX_HUFF_TABLES = 3,
    TDEFL_MAX_HUFF_SYMBOLS_0 = 288,
    TDEFL_MAX_HUFF_SYMBOLS_1 = 32,
    TDEFL_MAX_HUFF_SYMBOLS_2 = 19,
    TDEFL_LZ_DICT_SIZE = 32768,
    TDEFL_LZ_DICT_SIZE_MASK = TDEFL_LZ_DICT_SIZE - 1,
    TDEFL_MIN_MATCH_LEN = 3,
    TDEFL_MAX_MATCH_LEN = 258
};

/* TDEFL_OUT_BUF_SIZE MUST be large enough to hold a single entire compressed output block (using static/fixed Huffman codes). */
#if TDEFL_LESS_MEMORY
enum
{
    TDEFL_LZ_CODE_BUF_SIZE = 24 * 1024,
    TDEFL_OUT_BUF_SIZE = (TDEFL_LZ_CODE_BUF_SIZE * 13) / 10,
    TDEFL_MAX_HUFF_SYMBOLS = 288,
    TDEFL_LZ_HASH_BITS = 12,
    TDEFL_LEVEL1_HASH_SIZE_MASK = 4095,
    TDEFL_LZ_HASH_SHIFT = (TDEFL_LZ_HASH_BITS + 2) / 3,
    TDEFL_LZ_HASH_SIZE = 1 << TDEFL_LZ_HASH_BITS
};
#else
enum
{
    TDEFL_LZ_CODE_BUF_SIZE = 64 * 1024,
    TDEFL_OUT_BUF_SIZE = (TDEFL_LZ_CODE_BUF_SIZE * 13) / 10,
    TDEFL_MAX_HUFF_SYMBOLS = 288,
    TDEFL_LZ_HASH_BITS = 15,
    TDEFL_LEVEL1_HASH_SIZE_MASK = 4095,
    TDEFL_LZ_HASH_SHIFT = (TDEFL_LZ_HASH_BITS + 2) / 3,
    TDEFL_LZ_HASH_SIZE = 1 << TDEFL_LZ_HASH_BITS
};
#endif

/* The low-level tdefl functions below may be used directly if the above helper functions aren't flexible enough. The low-level functions don't make any heap allocations, unlike the above helper functions. */
enum class tdefl_status
{
    BAD_PARAM = -2,
    PUT_BUF_FAILED = -1,
    OKAY = 0,
    DONE = 1
};

/* Must map to MZ_NO_FLUSH, MZ_SYNC_FLUSH, etc. enums */
typedef enum
{
    TDEFL_NO_FLUSH = 0,
    TDEFL_SYNC_FLUSH = 2,
    TDEFL_FULL_FLUSH = 3,
    TDEFL_FINISH = 4
} tdefl_flush;

/* tdefl's compression state structure. */
struct tdefl_compressor
{
    tdefl_put_buf_func_ptr m_pPut_buf_func;
    void *m_pPut_buf_user;
    uint m_flags, m_max_probes[2];
    int m_greedy_parsing;
    uint m_adler32, m_lookahead_pos, m_lookahead_size, m_dict_size;
    uint8 *m_pLZ_code_buf, *m_pLZ_flags, *m_pOutput_buf, *m_pOutput_buf_end;
    uint m_num_flags_left, m_total_lz_bytes, m_lz_code_buf_dict_pos, m_bits_in, m_bit_buffer;
    uint m_saved_match_dist, m_saved_match_len, m_saved_lit, m_output_flush_ofs, m_output_flush_remaining, m_finished, m_block_index, m_wants_to_finish;
    tdefl_status m_prev_return_status;
    const void *m_pIn_buf;
    void *m_pOut_buf;
    size_t *m_pIn_buf_size, *m_pOut_buf_size;
    tdefl_flush m_flush;
    const uint8 *m_pSrc;
    size_t m_src_buf_left, m_out_buf_ofs;
    uint8 m_dict[TDEFL_LZ_DICT_SIZE + TDEFL_MAX_MATCH_LEN - 1];
    uint16 m_huff_count[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
    uint16 m_huff_codes[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
    uint8 m_huff_code_sizes[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
    uint8 m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE];
    uint16 m_next[TDEFL_LZ_DICT_SIZE];
    uint16 m_hash[TDEFL_LZ_HASH_SIZE];
    uint8 m_output_buf[TDEFL_OUT_BUF_SIZE];
} ;

/* Initializes the compressor. */
/* There is no corresponding deinit() function because the tdefl API's do not dynamically allocate memory. */
/* pBut_buf_func: If NULL, output data will be supplied to the specified callback. In this case, the user should call the tdefl_compress_buffer() API for compression. */
/* If pBut_buf_func is NULL the user should always call the tdefl_compress() API. */
/* flags: See the above enums (TDEFL_HUFFMAN_ONLY, TDEFL_WRITE_ZLIB_HEADER, etc.) */
tdefl_status tdefl_init(tdefl_compressor *d, tdefl_put_buf_func_ptr pPut_buf_func, void *pPut_buf_user, int flags);

/* Compresses a block of data, consuming as much of the specified input buffer as possible, and writing as much compressed data to the specified output buffer as possible. */
tdefl_status tdefl_compress(tdefl_compressor *d, const void *pIn_buf, size_t *pIn_buf_size, void *pOut_buf, size_t *pOut_buf_size, tdefl_flush flush);

/* tdefl_compress_buffer() is only usable when the tdefl_init() is called with a non-NULL tdefl_put_buf_func_ptr. */
/* tdefl_compress_buffer() always consumes the entire input buffer. */
tdefl_status tdefl_compress_buffer(tdefl_compressor *d, const void *pIn_buf, size_t in_buf_size, tdefl_flush flush);

tdefl_status tdefl_get_prev_return_status(tdefl_compressor *d);
uint32 tdefl_get_adler32(tdefl_compressor *d);

/* Create tdefl_compress() flags given zlib-style compression parameters. */
/* level may range from [0,10] (where 10 is absolute max compression, but may be much slower on some files) */
/* window_bits may be -15 (raw deflate) or 15 (zlib) */
/* strategy may be either MZ_DEFAULT_STRATEGY, MZ_FILTERED, MZ_HUFFMAN_ONLY, MZ_RLE, or MZ_FIXED */
uint tdefl_create_comp_flags_from_zip_params(compression_level, window_bits, compression_strategy);

/* Allocate the tdefl_compressor structure in C so that */
/* non-C language bindings to tdefl_ API don't need to worry about */
/* structure size and allocation mechanism. */
tdefl_compressor *tdefl_compressor_alloc();
void tdefl_compressor_free(tdefl_compressor *pComp);


/* ------------------- Low-level Decompression API Definitions */

/* Decompression flags used by tinfl_decompress(). */
/* TINFL_FLAG_PARSE_ZLIB_HEADER: If set, the input has a valid zlib header and ends with an adler32 checksum (it's a valid zlib stream). Otherwise, the input is a raw deflate stream. */
/* TINFL_FLAG_HAS_MORE_INPUT: If set, there are more input bytes available beyond the end of the supplied input buffer. If clear, the input buffer contains all remaining input. */
/* TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF: If set, the output buffer is large enough to hold the entire decompressed stream. If clear, the output buffer is at least the size of the dictionary (typically 32KB). */
/* TINFL_FLAG_COMPUTE_ADLER32: Force adler-32 checksum computation of the decompressed bytes. */
enum
{
    TINFL_FLAG_PARSE_ZLIB_HEADER = 1,
    TINFL_FLAG_HAS_MORE_INPUT = 2,
    TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4,
    TINFL_FLAG_COMPUTE_ADLER32 = 8
};

/* High level decompression functions: */
/* tinfl_decompress_mem_to_heap() decompresses a block in memory to a heap block allocated via malloc(). */
/* On entry: */
/*  pSrc_buf, src_buf_len: Pointer and size of the Deflate or zlib source data to decompress. */
/* On return: */
/*  Function returns a pointer to the decompressed data, or NULL on failure. */
/*  *pOut_len will be set to the decompressed data's size, which could be larger than src_buf_len on uncompressible data. */
/*  The caller must call free() on the returned block when it's no longer needed. */
void *tinfl_decompress_mem_to_heap(const void *pSrc_buf, size_t src_buf_len, size_t *pOut_len, int flags);

/* tinfl_decompress_mem_to_mem() decompresses a block in memory to another block in memory. */
/* Returns TINFL_DECOMPRESS_MEM_TO_MEM_FAILED on failure, or the number of bytes written on success. */
#define TINFL_DECOMPRESS_MEM_TO_MEM_FAILED ((size_t)(-1))
size_t tinfl_decompress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pSrc_buf, size_t src_buf_len, int flags);

/* tinfl_decompress_mem_to_callback() decompresses a block in memory to an internal 32KB buffer, and a user provided callback function will be called to flush the buffer. */
/* Returns 1 on success or 0 on failure. */
typedef int (*tinfl_put_buf_func_ptr)(const void *pBuf, int len, void *pUser);
int tinfl_decompress_mem_to_callback(const void *pIn_buf, size_t *pIn_buf_size, tinfl_put_buf_func_ptr pPut_buf_func, void *pPut_buf_user, int flags);

struct tinfl_decompressor;

/* Allocate the tinfl_decompressor structure in C so that */
/* non-C language bindings to tinfl_ API don't need to worry about */
/* structure size and allocation mechanism. */

tinfl_decompressor *tinfl_decompressor_alloc();
void tinfl_decompressor_free(tinfl_decompressor *pDecomp);

/* Max size of LZ dictionary. */
#define TINFL_LZ_DICT_SIZE 32768

/* Return status. */
typedef enum
{
    /* This flags indicates the inflator needs 1 or more input bytes to make forward progress, but the caller is indicating that no more are available. The compressed data */
    /* is probably corrupted. If you call the inflator again with more bytes it'll try to continue processing the input but this is a BAD sign (either the data is corrupted or you called it incorrectly). */
    /* If you call it again with no input you'll just get TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS again. */
    TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS = -4,

    /* This flag indicates that one or more of the input parameters was obviously bogus. (You can try calling it again, but if you get this error the calling code is wrong.) */
    TINFL_STATUS_BAD_PARAM = -3,

    /* This flags indicate the inflator is finished but the adler32 check of the uncompressed data didn't match. If you call it again it'll return TINFL_STATUS_DONE. */
    TINFL_STATUS_ADLER32_MISMATCH = -2,

    /* This flags indicate the inflator has somehow failed (bad code, corrupted input, etc.). If you call it again without resetting via tinfl_init() it it'll just keep on returning the same status failure code. */
    TINFL_STATUS_FAILED = -1,

    /* Any status code less than TINFL_STATUS_DONE must indicate a failure. */

    /* This flag indicates the inflator has returned every byte of uncompressed data that it can, has consumed every byte that it needed, has successfully reached the end of the deflate stream, and */
    /* if zlib headers and adler32 checking enabled that it has successfully checked the uncompressed data's adler32. If you call it again you'll just get TINFL_STATUS_DONE over and over again. */
    TINFL_STATUS_DONE = 0,

    /* This flag indicates the inflator MUST have more input data (even 1 byte) before it can make any more forward progress, or you need to clear the TINFL_FLAG_HAS_MORE_INPUT */
    /* flag on the next call if you don't have any more source data. If the source data was somehow corrupted it's also possible (but unlikely) for the inflator to keep on demanding input to */
    /* proceed, so be sure to properly set the TINFL_FLAG_HAS_MORE_INPUT flag. */
    TINFL_STATUS_NEEDS_MORE_INPUT = 1,

    /* This flag indicates the inflator definitely has 1 or more bytes of uncompressed data available, but it cannot write this data into the output buffer. */
    /* Note if the source compressed data was corrupted it's possible for the inflator to return a lot of uncompressed data to the caller. I've been assuming you know how much uncompressed data to expect */
    /* (either exact or worst case) and will stop calling the inflator and fail after receiving too much. In pure streaming scenarios where you have no idea how many bytes to expect this may not be possible */
    /* so I may need to add some code to address this. */
    TINFL_STATUS_HAS_MORE_OUTPUT = 2
} tinfl_status;

/* Initializes the decompressor to its initial state. */
#define tinfl_init(r)     \
    do                    \
    {                     \
        (r)->m_state = 0; \
    }                     \
    MZ_MACRO_END
#define tinfl_get_adler32(r) (r)->m_check_adler32

/* Main low-level decompressor coroutine function. This is the only function actually needed for decompression. All the other functions are just high-level helpers for improved usability. */
/* This is a universal API, i.e. it can be used as a building block to build any desired higher level decompression API. In the limit case, it can be called once per every byte input or output. */
tinfl_status tinfl_decompress(tinfl_decompressor *r, const uint8 *pIn_buf_next, size_t *pIn_buf_size, uint8 *pOut_buf_start, uint8 *pOut_buf_next, size_t *pOut_buf_size, const uint32 decomp_flags);

/* Internal/private bits follow. */
enum
{
    TINFL_MAX_HUFF_TABLES = 3,
    TINFL_MAX_HUFF_SYMBOLS_0 = 288,
    TINFL_MAX_HUFF_SYMBOLS_1 = 32,
    TINFL_MAX_HUFF_SYMBOLS_2 = 19,
    TINFL_FAST_LOOKUP_BITS = 10,
    TINFL_FAST_LOOKUP_SIZE = 1 << TINFL_FAST_LOOKUP_BITS
};

struct tinfl_huff_table
{
    uint8 m_code_size[TINFL_MAX_HUFF_SYMBOLS_0];
    int16 m_look_up[TINFL_FAST_LOOKUP_SIZE], m_tree[TINFL_MAX_HUFF_SYMBOLS_0 * 2];
};

#if MINIZ_HAS_64BIT_REGISTERS
#define TINFL_USE_64BIT_BITBUF 1
#endif

#if TINFL_USE_64BIT_BITBUF
typedef uint64 tinfl_bit_buf_t;
#define TINFL_BITBUF_SIZE (64)
#else
typedef uint32 tinfl_bit_buf_t;
#define TINFL_BITBUF_SIZE (32)
#endif

struct tinfl_decompressor
{
    uint32 m_state, m_num_bits, m_zhdr0, m_zhdr1, m_z_adler32, m_final, m_type, m_check_adler32, m_dist, m_counter, m_num_extra, m_table_sizes[TINFL_MAX_HUFF_TABLES];
    tinfl_bit_buf_t m_bit_buf;
    size_t m_dist_from_out_buf_start;
    tinfl_huff_table m_tables[TINFL_MAX_HUFF_TABLES];
    uint8 m_raw_header[4], m_len_codes[TINFL_MAX_HUFF_SYMBOLS_0 + TINFL_MAX_HUFF_SYMBOLS_1 + 137];
};




/* ------------------- ZIP archive reading/writing */

#ifndef MINIZ_NO_ARCHIVE_APIS


enum
{
    /* Note: These enums can be reduced as needed to save memory or stack space - they are pretty conservative. */
    MZ_ZIP_MAX_IO_BUF_SIZE = 64 * 1024,
    MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE = 512,
    MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE = 512
};

struct zip_archive_file_stat
{
    /* Central directory file index. */
    uint32 m_file_index;

    /* Byte offset of this entry in the archive's central directory. Note we currently only support up to UINT_MAX or less bytes in the central dir. */
    uint64 m_central_dir_ofs;

    /* These fields are copied directly from the zip's central dir. */
    uint16 m_version_made_by;
    uint16 m_version_needed;
    uint16 m_bit_flag;
    compression_method m_method;

#ifndef MINIZ_NO_TIME
    time_t m_time;
#endif

    /* CRC-32 of uncompressed data. */
    uint32 m_crc32;

    /* File's compressed size. */
    uint64 m_comp_size;

    /* File's uncompressed size. Note, I've seen some old archives where directory entries had 512 bytes for their uncompressed sizes, but when you try to unpack them you actually get 0 bytes. */
    uint64 m_uncomp_size;

    /* Zip internal and external file attributes. */
    uint16 m_internal_attr;
    uint32 m_external_attr;

    /* Entry's local header file offset in bytes. */
    uint64 m_local_header_ofs;

    /* Size of comment in bytes. */
    uint32 m_comment_size;

    /* true if the entry appears to be a directory. */
    bool m_is_directory;

    /* true if the entry uses encryption/strong encryption (which miniz_zip doesn't support) */
    bool m_is_encrypted;

    /* true if the file is not encrypted, a patch file, and if it uses a compression method we support. */
    bool m_is_supported;

    /* Filename. If string ends in '/' it's a subdirectory entry. */
    /* Guaranteed to be zero terminated, may be truncated to fit. */
    char m_filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];

    /* Comment field. */
    /* Guaranteed to be zero terminated, may be truncated to fit. */
    char m_comment[MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE];

};

typedef size_t (*file_read_func)(void *pOpaque, uint64 file_ofs, void *pBuf, size_t n);
typedef size_t (*file_write_func)(void *pOpaque, uint64 file_ofs, const void *pBuf, size_t n);
typedef bool (*file_needs_keepalive)(void *pOpaque);

struct zip_internal_state;

enum class zip_mode : uint
{
    INVALID = 0,
    READING = 1,
    WRITING = 2,
    WRITING_HAS_BEEN_FINALIZED = 3
};

struct zip_flags {

	enum flag : uint32_t {
		NONE = 0,
		CASE_SENSITIVE = 0x0100,
		IGNORE_PATH = 0x0200,
		COMPRESSED_DATA = 0x0400,
		DO_NOT_SORT_CENTRAL_DIRECTORY = 0x0800,
		VALIDATE_LOCATE_FILE_FLAG = 0x1000, /* if enabled, zip_reader_locate_file() will be called on each file as its validated to ensure the func finds the file in the central dir (intended for testing) */
		VALIDATE_HEADERS_ONLY = 0x2000,     /* validate the local headers, but don't decompress the entire file and check the crc32 */
		WRITE_ZIP64 = 0x4000,               /* use the zip64 file format, instead of the original zip file format */
		WRITE_ALLOW_READING = 0x8000,
		ASCII_FILENAME = 0x10000
	} f;

	zip_flags(flag f) noexcept : f(f) {}
	
	explicit zip_flags(uint32_t fval) noexcept : f((flag)fval) {}

	bool operator &(const zip_flags& zf) const noexcept {
		return bool((uint)f & (uint)zf.f);
	}

	zip_flags operator |(const zip_flags& zf) const noexcept {
		return flag((uint)f | (uint)zf.f);
	}

	explicit operator uint32_t() const noexcept {
		return f;
	}

};


enum class zip_type : uint
{
    INVALID = 0,
    USER,
    MEMORY,
    HEAP,
    FILE,
    CFILE,
    TOTAL_TYPES
};

/* miniz error codes. Be sure to update zip_get_error_string() if you add or modify this enum. */
enum class zip_error : uint
{
    NO_ERROR = 0,
    UNDEFINED_ERROR,
    TOO_MANY_FILES,
    FILE_TOO_LARGE,
    UNSUPPORTED_METHOD,
    UNSUPPORTED_ENCRYPTION,
    UNSUPPORTED_FEATURE,
    FAILED_FINDING_CENTRAL_DIR,
    NOT_AN_ARCHIVE,
    INVALID_HEADER_OR_CORRUPTED,
    UNSUPPORTED_MULTIDISK,
    DECOMPRESSION_FAILED,
    COMPRESSION_FAILED,
    UNEXPECTED_DECOMPRESSED_SIZE,
    CRC_CHECK_FAILED,
    UNSUPPORTED_CDIR_SIZE,
    ALLOC_FAILED,
    FILE_OPEN_FAILED,
    FILE_CREATE_FAILED,
    FILE_WRITE_FAILED,
    FILE_READ_FAILED,
    FILE_CLOSE_FAILED,
    FILE_SEEK_FAILED,
    FILE_STAT_FAILED,
    INVALID_PARAMETER,
    INVALID_FILENAME,
    BUF_TOO_SMALL,
    INTERNAL_ERROR,
    FILE_NOT_FOUND,
    ARCHIVE_TOO_LARGE,
    VALIDATION_FAILED,
    WRITE_CALLBACK_FAILED,
    TOTAL_ERRORS
};

struct zip_archive
{
    uint64 m_archive_size;
    uint64 m_central_directory_file_ofs;

    /* We only support up to UINT32_MAX files in zip64 mode. */
    uint32 m_total_files;
    zip_mode m_zip_mode;
    zip_type m_zip_type;
    zip_error m_last_error;

    uint64 m_file_offset_alignment;

    alloc_func m_pAlloc;
    free_func m_pFree;
    realloc_func m_pRealloc;
    void *m_pAlloc_opaque;

    file_read_func m_pRead;
    file_write_func m_pWrite;
	file_needs_keepalive m_pNeeds_keepalive;
    void *m_pIO_opaque;

    zip_internal_state *m_pState;

};

/* -------- ZIP reading */

/* Inits a ZIP archive reader. */
/* These functions read and validate the archive's central directory. */
bool zip_reader_init(zip_archive *pZip, uint64 size, zip_flags);

bool zip_reader_init_mem(zip_archive *pZip, const void *pMem, size_t size, zip_flags);

#ifndef MINIZ_NO_STDIO
/* Read a archive from a disk file. */
/* file_start_ofs is the file offset where the archive actually begins, or 0. */
/* actual_archive_size is the true total size of the archive, which may be smaller than the file's actual size on disk. If zero the entire file is treated as the archive. */
bool zip_reader_init_file(zip_archive *pZip, const char *pFilename, zip_flags);
bool zip_reader_init_file_v2(zip_archive *pZip, const char *pFilename, zip_flags flags, uint64 file_start_ofs, uint64 archive_size);

/* Read an archive from an already opened FILE, beginning at the current file position. */
/* The archive is assumed to be archive_size bytes long. If archive_size is < 0, then the entire rest of the file is assumed to contain the archive. */
/* The FILE will NOT be closed when zip_reader_end() is called. */
bool zip_reader_init_cfile(zip_archive *pZip, FILE *pFile, uint64 archive_size, zip_flags);
#endif

/* Ends archive reading, freeing all allocations, and closing the input archive file if zip_reader_init_file() was used. */
bool zip_reader_end(zip_archive *pZip);

/* -------- ZIP reading or writing */

/* Clears a zip_archive struct to all zeros. */
/* Important: This must be done before passing the struct to any zip functions. */
void zip_zero_struct(zip_archive *pZip);

zip_mode zip_get_mode(zip_archive *pZip);
zip_type zip_get_type(zip_archive *pZip);

/* Returns the total number of files in the archive. */
uint zip_reader_get_num_files(zip_archive *pZip);

uint64 zip_get_archive_size(zip_archive *pZip);
uint64 zip_get_archive_file_start_offset(zip_archive *pZip);
FILE *zip_get_cfile(zip_archive *pZip);

/* Reads n bytes of raw archive data, starting at file offset file_ofs, to pBuf. */
size_t zip_read_archive_data(zip_archive *pZip, uint64 file_ofs, void *pBuf, size_t n);

/* Attempts to locates a file in the archive's central directory. */
/* Valid flags: MZ_ZIP_FLAG_CASE_SENSITIVE, MZ_ZIP_FLAG_IGNORE_PATH */
/* Returns -1 if the file cannot be found. */
int zip_locate_file(zip_archive *pZip, const char *pName, const char *pComment, zip_flags);
/* Returns false if the file cannot be found. */
bool zip_locate_file_v2(zip_archive *pZip, const char *pName, const char *pComment, zip_flags, uint32 *pIndex);

/* All zip funcs set the m_last_error field in the zip_archive struct. These functions retrieve/manipulate this field. */
/* Note that the m_last_error functionality is not thread safe. */
zip_error zip_set_last_error(zip_archive *pZip, zip_error err_num);
zip_error zip_peek_last_error(zip_archive *pZip);
zip_error zip_clear_last_error(zip_archive *pZip);
zip_error zip_get_last_error(zip_archive *pZip);
const char *zip_get_error_string(zip_error err);

/* true if the archive file entry is a directory entry. */
bool zip_reader_is_file_a_directory(zip_archive *pZip, uint file_index);

/* true if the file is encrypted/strong encrypted. */
bool zip_reader_is_file_encrypted(zip_archive *pZip, uint file_index);

/* true if the compression method is supported, and the file is not encrypted, and the file is not a compressed patch file. */
bool zip_reader_is_file_supported(zip_archive *pZip, uint file_index);

/* Retrieves the filename of an archive file entry. */
/* Returns the number of bytes written to pFilename, or if filename_buf_size is 0 this function returns the number of bytes needed to fully store the filename. */
uint zip_reader_get_filename(zip_archive *pZip, uint file_index, char *pFilename, uint filename_buf_size);

/* Attempts to locates a file in the archive's central directory. */
/* Valid flags: MZ_ZIP_FLAG_CASE_SENSITIVE, MZ_ZIP_FLAG_IGNORE_PATH */
/* Returns -1 if the file cannot be found. */
int zip_reader_locate_file(zip_archive *pZip, const char *pName, const char *pComment, zip_flags);
bool zip_reader_locate_file_v2(zip_archive *pZip, const char *pName, const char *pComment, zip_flags, uint32 *file_index);

/* Returns detailed information about an archive file entry. */
bool zip_reader_file_stat(zip_archive *pZip, uint file_index, zip_archive_file_stat *pStat);

/* true if the file is in zip64 format. */
/* A file is considered zip64 if it contained a zip64 end of central directory marker, or if it contained any zip64 extended file information fields in the central directory. */
bool zip_is_zip64(zip_archive *pZip);

/* Returns the total central directory size in bytes. */
/* The current max supported size is <= MZ_UINT32_MAX. */
size_t zip_get_central_dir_size(zip_archive *pZip);

/* Extracts a archive file to a memory buffer using no memory allocation. */
/* There must be at least enough room on the stack to store the inflator's state (~34KB or so). */
bool zip_reader_extract_to_mem_no_alloc(zip_archive *pZip, uint file_index, void *pBuf, size_t buf_size, zip_flags, void *pUser_read_buf, size_t user_read_buf_size);
bool zip_reader_extract_file_to_mem_no_alloc(zip_archive *pZip, const char *pFilename, void *pBuf, size_t buf_size, zip_flags, void *pUser_read_buf, size_t user_read_buf_size);

/* Extracts a archive file to a memory buffer. */
bool zip_reader_extract_to_mem(zip_archive *pZip, uint file_index, void *pBuf, size_t buf_size, zip_flags);
bool zip_reader_extract_file_to_mem(zip_archive *pZip, const char *pFilename, void *pBuf, size_t buf_size, zip_flags);

/* Extracts a archive file to a dynamically allocated heap buffer. */
/* The memory will be allocated via the zip_archive's alloc/realloc functions. */
/* Returns NULL and sets the last error on failure. */
void *zip_reader_extract_to_heap(zip_archive *pZip, uint file_index, size_t *pSize, zip_flags);
void *zip_reader_extract_file_to_heap(zip_archive *pZip, const char *pFilename, size_t *pSize, zip_flags);

/* Extracts a archive file using a callback function to output the file's data. */
bool zip_reader_extract_to_callback(zip_archive *pZip, uint file_index, file_write_func pCallback, void *pOpaque, zip_flags);
bool zip_reader_extract_file_to_callback(zip_archive *pZip, const char *pFilename, file_write_func pCallback, void *pOpaque, zip_flags);

#ifndef MINIZ_NO_STDIO
/* Extracts a archive file to a disk file and sets its last accessed and modified times. */
/* This function only extracts files, not archive directory records. */
bool zip_reader_extract_to_file(zip_archive *pZip, uint file_index, const char *pDst_filename, zip_flags);
bool zip_reader_extract_file_to_file(zip_archive *pZip, const char *pArchive_filename, const char *pDst_filename, zip_flags);

/* Extracts a archive file starting at the current position in the destination FILE stream. */
bool zip_reader_extract_to_cfile(zip_archive *pZip, uint file_index, FILE *File, zip_flags);
bool zip_reader_extract_file_to_cfile(zip_archive *pZip, const char *pArchive_filename, FILE *pFile, zip_flags);
#endif

#if 0
/* TODO */
	typedef void *zip_streaming_extract_state_ptr;
	zip_streaming_extract_state_ptr zip_streaming_extract_begin(zip_archive *pZip, uint file_index, zip_flags);
	uint64_t zip_streaming_extract_get_size(zip_archive *pZip, zip_streaming_extract_state_ptr pState);
	uint64_t zip_streaming_extract_get_cur_ofs(zip_archive *pZip, zip_streaming_extract_state_ptr pState);
	bool zip_streaming_extract_seek(zip_archive *pZip, zip_streaming_extract_state_ptr pState, uint64_t new_ofs);
	size_t zip_streaming_extract_read(zip_archive *pZip, zip_streaming_extract_state_ptr pState, void *pBuf, size_t buf_size);
	bool zip_streaming_extract_end(zip_archive *pZip, zip_streaming_extract_state_ptr pState);
#endif

/* This function compares the archive's local headers, the optional local zip64 extended information block, and the optional descriptor following the compressed data vs. the data in the central directory. */
/* It also validates that each file can be successfully uncompressed unless the MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY is specified. */
bool zip_validate_file(zip_archive *pZip, uint file_index, zip_flags);

/* Validates an entire archive by calling zip_validate_file() on each file. */
bool zip_validate_archive(zip_archive *pZip, zip_flags);

/* Misc utils/helpers, valid for ZIP reading or writing */
bool zip_validate_mem_archive(const void *pMem, size_t size, zip_flags, zip_error *pErr);
bool zip_validate_file_archive(const char *pFilename, zip_flags, zip_error *pErr);

/* Universal end function - calls either zip_reader_end() or zip_writer_end(). */
bool zip_end(zip_archive *pZip);

/* -------- ZIP writing */

#ifndef MINIZ_NO_ARCHIVE_WRITING_APIS

/* Inits a ZIP archive writer. */
bool zip_writer_init(zip_archive *pZip, uint64 existing_size);
bool zip_writer_init_v2(zip_archive *pZip, uint64 existing_size, zip_flags);
bool zip_writer_init_heap(zip_archive *pZip, size_t size_to_reserve_at_beginning, size_t initial_allocation_size);
bool zip_writer_init_heap_v2(zip_archive *pZip, size_t size_to_reserve_at_beginning, size_t initial_allocation_size, zip_flags);

#ifndef MINIZ_NO_STDIO
bool zip_writer_init_file(zip_archive *pZip, const char *pFilename, uint64 size_to_reserve_at_beginning);
bool zip_writer_init_file_v2(zip_archive *pZip, const char *pFilename, uint64 size_to_reserve_at_beginning, zip_flags);
bool zip_writer_init_cfile(zip_archive *pZip, FILE *pFile, zip_flags);
#endif

/* Converts a ZIP archive reader object into a writer object, to allow efficient in-place file appends to occur on an existing archive. */
/* For archives opened using zip_reader_init_file, pFilename must be the archive's filename so it can be reopened for writing. If the file can't be reopened, zip_reader_end() will be called. */
/* For archives opened using zip_reader_init_mem, the memory block must be growable using the realloc callback (which defaults to realloc unless you've overridden it). */
/* Finally, for archives opened using zip_reader_init, the zip_archive's user provided m_pWrite function cannot be NULL. */
/* Note: In-place archive modification is not recommended unless you know what you're doing, because if execution stops or something goes wrong before */
/* the archive is finalized the file's central directory will be hosed. */
bool zip_writer_init_from_reader(zip_archive *pZip, const char *pFilename);
bool zip_writer_init_from_reader_v2(zip_archive *pZip, const char *pFilename, zip_flags);

/* Adds the contents of a memory buffer to an archive. These functions record the current local time into the archive. */
/* To add a directory entry, call this method with an archive name ending in a forwardslash with an empty buffer. */
/* level_and_flags - compression level (0-10, see MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc.) logically OR'd with zero or more zip_flags, or just set to MZ_DEFAULT_COMPRESSION. */
bool zip_writer_add_mem(zip_archive *pZip, const char *pArchive_name, const void *pBuf, size_t buf_size, uint level_and_flags);

/* Like zip_writer_add_mem(), except you can specify a file comment field, and optionally supply the function with already compressed data. */
/* uncomp_size/uncomp_crc32 are only used if the MZ_ZIP_FLAG_COMPRESSED_DATA flag is specified. */
bool zip_writer_add_mem_ex(zip_archive *pZip, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, uint16 comment_size, uint level_and_flags,
                                 uint64 uncomp_size, uint32 uncomp_crc32);

bool zip_writer_add_mem_ex_v2(zip_archive *pZip, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, uint16 comment_size, uint level_and_flags,
                                    uint64 uncomp_size, uint32 uncomp_crc32, time_t *last_modified, const char *user_extra_data_local, uint user_extra_data_local_len,
                                    const char *user_extra_data_central, uint user_extra_data_central_len);

#ifndef MINIZ_NO_STDIO
/* Adds the contents of a disk file to an archive. This function also records the disk file's modified time into the archive. */
/* level_and_flags - compression level (0-10, see MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc.) logically OR'd with zero or more zip_flags, or just set to MZ_DEFAULT_COMPRESSION. */
bool zip_writer_add_file(zip_archive *pZip, const char *pArchive_name, const char *pSrc_filename, const void *pComment, uint16 comment_size, uint level_and_flags);

/* Like zip_writer_add_file(), except the file data is read from the specified FILE stream. */
bool zip_writer_add_cfile(zip_archive *pZip, const char *pArchive_name, FILE *pSrc_file, uint64 size_to_add,
                                const time_t *pFile_time, const void *pComment, uint16 comment_size, uint level_and_flags, const char *user_extra_data_local, uint user_extra_data_local_len,
                                const char *user_extra_data_central, uint user_extra_data_central_len);
#endif

/* Adds a file to an archive by fully cloning the data from another archive. */
/* This function fully clones the source file's compressed data (no recompression), along with its full filename, extra data (it may add or modify the zip64 local header extra data field), and the optional descriptor following the compressed data. */
bool zip_writer_add_from_zip_reader(zip_archive *pZip, zip_archive *pSource_zip, uint src_file_index);

/* Finalizes the archive by writing the central directory records followed by the end of central directory record. */
/* After an archive is finalized, the only valid call on the zip_archive struct is zip_writer_end(). */
/* An archive must be manually finalized by calling this function for it to be valid. */
bool zip_writer_finalize_archive(zip_archive *pZip);

/* Finalizes a heap archive, returning a poiner to the heap block and its size. */
/* The heap block will be allocated using the zip_archive's alloc/realloc callbacks. */
bool zip_writer_finalize_heap_archive(zip_archive *pZip, void **ppBuf, size_t *pSize);

/* Ends archive writing, freeing all allocations, and closing the output file if zip_writer_init_file() was used. */
/* Note for the archive to be valid, it *must* have been finalized before ending (this function will not do it for you). */
bool zip_writer_end(zip_archive *pZip);

/* -------- Misc. high-level helper functions: */

/* zip_add_mem_to_archive_file_in_place() efficiently (but not atomically) appends a memory blob to a ZIP archive. */
/* Note this is NOT a fully safe operation. If it crashes or dies in some way your archive can be left in a screwed up state (without a central directory). */
/* level_and_flags - compression level (0-10, see MZ_BEST_SPEED, MZ_BEST_COMPRESSION, etc.) logically OR'd with zero or more zip_flags, or just set to MZ_DEFAULT_COMPRESSION. */
/* TODO: Perhaps add an option to leave the existing central dir in place in case the add dies? We could then truncate the file (so the old central dir would be at the end) if something goes wrong. */
bool zip_add_mem_to_archive_file_in_place(const char *pZip_filename, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, uint16 comment_size, uint level_and_flags);
bool zip_add_mem_to_archive_file_in_place_v2(const char *pZip_filename, const char *pArchive_name, const void *pBuf, size_t buf_size, const void *pComment, uint16 comment_size, uint level_and_flags, zip_error *pErr);

/* Reads a single file from an archive into a heap block. */
/* If pComment is not NULL, only the file with the specified comment will be extracted. */
/* Returns NULL on failure. */
void *zip_extract_archive_file_to_heap(const char *pZip_filename, const char *pArchive_name, size_t *pSize, zip_flags);
void *zip_extract_archive_file_to_heap_v2(const char *pZip_filename, const char *pArchive_name, const char *pComment, size_t *pSize, zip_flags, zip_error *pErr);

#endif /* #ifndef MINIZ_NO_ARCHIVE_WRITING_APIS */

#endif /* MINIZ_NO_ARCHIVE_APIS */

} /* namespace mz */
