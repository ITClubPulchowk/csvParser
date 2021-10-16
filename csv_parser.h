/*! \file csv_parser.h
	\brief A Library for parsing CSV files. csvParser is header only library, to use this, just include this file.
*/

#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

/*! \def CSV_PARSER_API
	\brief The compilation configuration can be static and extern, by default it is extern. If static compilation for all the API is required, then CSV_PARSER_API_STATIC needs to get defined before including csv_parser.h
*/
#ifndef CSV_PARSER_API
#ifndef CSV_PARSER_API_STATIC
#define CSV_PARSER_API extern
#define CSV_PARSER_DEFN_API 
#else
#define CSV_PARSER_API static
#define CSV_PARSER_DEFN_API static
#endif
#else
#ifndef CSV_PARSER_DEFN_API 
#error "If CSV_PARSER_API is defined, then CSV_PARSER_DEFN_API must also be defined"
#endif
#endif

/*! \def CSV_PARSER_ASSERT
	\brief Custom assert can be used by defining CSV_PARSER_ASSERT before including csv_parser.h. When custom assert is used, assert.h is not included.
*/
#ifndef CSV_PARSER_ASSERT
#include <assert.h>
#define CSV_PARSER_ASSERT assert
#endif

/*! \def CSV_PARSER_MALLOC
	\brief To use custom allocators, CSV_PARSER_MALLOC and CSV_PARSER_FREE both needs to get redefined before including csv_parser.h. When custom allocators are used, stdlib.h is not included.
*/
#ifndef CSV_PARSER_MALLOC
#include <stdlib.h>
#define CSV_PARSER_MALLOC(size, context) malloc(size)
#define CSV_PARSER_FREE(ptr, context) free(ptr)
#else
#ifndef CSV_PARSER_FREE
#error "If CSV_PARSER_MALLOC is defined, then CSV_PARSER_FREE must also be defined"
#endif
#endif

/*! \def CSV_PARSER_MEMCPY
	\brief To use custom memcpy, CSV_PARSER_MEMCPY needs to get redefined before including csv_parser.h. When custom memcpy is used, string.h is not included.
*/
#ifndef CSV_PARSER_MEMCPY
#include <string.h>
#define CSV_PARSER_MEMCPY memcpy
#endif

/*! \def CSV_PARSER_STDIO_INCLUDED
	\brief To replace including stdio.h, define CSV_PARSER_NO_STDIO before including csv_parser.h
*/
#ifndef CSV_PARSER_NO_STDIO
#define CSV_PARSER_STDIO_INCLUDED
#include <stdio.h>
#endif

#include <stdint.h>


//
// [Declarations]
//

#if !defined(__cplusplus)
extern "C" {
#endif

	typedef int32_t CSV_PARSER_Bool;

	/*! \struct CSV_PARSER
		\brief [CSV_PARSER](@ref CSV_PARSER) is passed to Buffer Loading Procedures and gets filled by these functions
	*/
	typedef struct CSV_PARSER {
		// Public
		size_t columns; /*!< Number of columns present in the CSV buffer. Filled when [csv_parser_load_buffer](@ref csv_parser_load_buffer) is called */
		size_t lines; /*!< Number of rows (including the header) in the CSV buffer. Filled when [csv_parser_load_buffer](@ref csv_parser_load_buffer) is called */

		struct {
			char *reason; /*!< Reason for why the error happened */
			size_t column; /*!< Value is non zero if the CSV buffer is invalid and represent the column where the parsing error occured. For other error value is zero. */
			size_t line; /*!< Value is non zero if the CSV buffer is invalid and represent the row where the parsing error occured. For other error value is zero. */
		} error; /*!< Constains error information. Filled if [csv_parser_load_buffer](@ref csv_parser_load_buffer) returns false */

		// Internal
		uint8_t *buffer; /*!< The pointer to the CSV buffer. */
		uint8_t *position; /*!< The position upto where the CSV buffer is parsed. */
		size_t buffer_length; /*!< The length of the [CSV_PARSER::buffer](@ref CSV_PARSER::buffer). */

		void *allocator_context; /*!< User data which is passed to [csv_parser_malloc](@ref csv_parser_malloc) and [csv_parser_free](@ref csv_parser_free) */
	} CSV_PARSER;

	/*! \fn void csv_parser_init(CSV_PARSER *parser, void *allocator_context)
		\brief Initializes [CSV_PARSER](@ref CSV_PARSER) with the allocator context
		Allocator context is the user data which is passed to [csv_parser_malloc](@ref csv_parser_malloc) and [csv_parser_free](@ref csv_parser_free).
		Other values are set to default values. This procedure must be called before loading the CSV buffer.

		\param parser The pointer to [CSV_PARSER](@ref CSV_PARSER) to be initialized
		\param allocator_context The allocator context
	*/
	CSV_PARSER_API void csv_parser_init(CSV_PARSER *parser, void *allocator_context);

	/*! \fn void *csv_parser_malloc(size_t size, void *context)
		\brief CSV procedure to allocate memory. By default uses malloc from standard C library. To use custom allocator, see [here](@ref CSV_PARSER_MALLOC)

		\param size The size of the memory yo be allocated
		\param context The allocator context
		\return The pointer to allocated memory
	*/
	CSV_PARSER_API void *csv_parser_malloc(size_t size, void *context);


	/*! \fn void csv_parser_free(void *ptr, void *context)
		\brief CSV procedure to free memory allocated using [csv_parser_malloc](@ref csv_parser_malloc). By default uses free from standard C library. To use custom allocator, see [here](@ref CSV_PARSER_MALLOC)

		\param ptr The pointer to the memory to be freed. Must be the pointer returned by [csv_parser_malloc](@ref csv_parser_malloc)
		\param context The allocator context
	*/
	CSV_PARSER_API void csv_parser_free(void *ptr, void *context);

	/*! \fn uint8_t *csv_parser_duplicate_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length)
		\brief Allocates memory and copy and null terminates the given buffer using the allocator context present in given [CSV_PARSER](@ref CSV_PARSER)

		\param parser The parser whose allocator context is to be used for allocation
		\param buffer The buffer to be duplicated
		\param length The length of the given buffer
		\return The duplicated and null terminated buffer
	*/
	CSV_PARSER_API uint8_t *csv_parser_duplicate_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length);

	/*! \fn CSV_PARSER_Bool csv_parser_load_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length)
		\brief Loads the CSV buffer for parsing. The buffer gets modified by this function. If the given buffer should not be modified, use [csv_parser_load_duplicated](@ref csv_parser_load_duplicated) instead.

		\param parser Parser where the buffer is to be associated with
		\param buffer The CSV buffer. The buffer must be null terminated
		\param length The length of the buffer. The length of the buffer MUST not count the null terminator
		\return Non zero if the passed CSV buffer is valid. If zero, loading CSV buffer failed and error message is stored in [CSV_PARSER::error](@ref CSV_PARSER::error)
	*/
	CSV_PARSER_API CSV_PARSER_Bool csv_parser_load_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length);


	/*! \fn CSV_PARSER_Bool csv_parser_load_duplicated(CSV_PARSER *parser, uint8_t *buffer, size_t length)
		\brief Loads the CSV buffer by duplicating the original buffer by calling [csv_parser_duplicate_buffer](@ref csv_parser_duplicate_buffer)
		Allocates memory for buffer. To release that memory [csv_parser_release](@ref csv_parser_release) must be called.
		This procedure should be used if modification of the original buffer is not intended.
		Calling this procedure is equivalent to [csv_parser_load_buffer](@ref csv_parser_load_buffer) after duplicating buffer using [csv_parser_duplicate_buffer](@ref csv_parser_duplicate_buffer)

		\param parser Parser where the duplicated buffer is to be associated with
		\param buffer The CSV buffer. The buffer may or may not be null terminated
		\param length The length of the buffer. The length of the buffer MUST not count the null terminator if null terminator is present in the given buffer
		\return Non zero if the passed CSV buffer is valid. If zero, loading CSV buffer failed and error message is stored in [CSV_PARSER](@ref CSV_PARSER).error
	*/
	CSV_PARSER_API CSV_PARSER_Bool csv_parser_load_duplicated(CSV_PARSER *parser, uint8_t *buffer, size_t length);


#ifdef CSV_PARSER_STDIO_INCLUDED

	/*! \fn CSV_PARSER_Bool csv_parser_load_file(CSV_PARSER *parser, FILE *fp)
		\brief Loads the CSV buffer for parsing by reading from given FILE *
		Allocates memory for buffer. To release that memory [csv_parser_release](@ref csv_parser_release) must be called.
		The file is read as a whole even if the cursor of the file is not present at the start.
		The position of the cursor of the file handle will be at the end of the file if this procedure passes.
		The position of the cursor of the file handle is undefined if this procedure fails.

		\param parser Parser where the buffer is to be associated with
		\param fp The file handle which is to be read to load the buffer
		\return Non zero if the read CSV buffer is valid. If zero, loading CSV buffer failed and error message is stored in [CSV_PARSER::error](@ref CSV_PARSER::error)
	*/
	CSV_PARSER_API CSV_PARSER_Bool csv_parser_load_file(CSV_PARSER *parser, FILE *fp);

	/*! \fn CSV_PARSER_Bool csv_parser_load(CSV_PARSER *parser, const char *file_path)
		\brief Loads the file from the given file path, reads the CSV buffer from the file, loads the CSV buffer into [CSV_PARSER](@ref CSV_PARSER)
		Allocates memory for buffer. To release that memory [csv_parser_release](@ref csv_parser_release) must be called

		\param parser Parser where the CSV buffer is to be associated with
		\param file_path The path to the CSV file
		\return Non zero if the read CSV buffer is valid. If zero, loading CSV buffer failed and error message is stored in [CSV_PARSER::error](@ref CSV_PARSER::error)
	*/
	CSV_PARSER_API CSV_PARSER_Bool csv_parser_load(CSV_PARSER *parser, const char *file_path);

	/*! \fn void csv_parser_release(CSV_PARSER *parser)
		\brief Release the buffer allocated by using [csv_parser_load](@ref csv_parser_load), [csv_parser_load_file](@ref csv_parser_load_file) and [csv_parser_load_duplicated](@ref csv_parser_load_duplicated)

		\param parser The parser whose buffer is to be released
	*/
	CSV_PARSER_API void csv_parser_release(CSV_PARSER *parser);

#endif


	/*! \fn uint8_t *csv_parser_next(CSV_PARSER *parser, size_t *length)
		\brief Parses the next element in the CSV buffer.
		This procedure is expected to be called in a loop of [CSV_PARSER::lines](@ref CSV_PARSER::lines) and [CSV_PARSER::columns](@ref CSV_PARSER::columns).
		The first [CSV_PARSER::columns](@ref CSV_PARSER::columns) values are always the heading of the CSV buffer

		\param parser The parser whose next element should be parsed
		\param length Returns the length of the UTF-8 string returned
		\return The UTF-8 string of the next element
	*/
	CSV_PARSER_API uint8_t *csv_parser_next(CSV_PARSER *parser, size_t *length);

	/*! \fn void csv_parser_skip_row(CSV_PARSER *parser)
		\brief Skips the single row in the CSV buffer. Useful when it is wanted to skip the first row i.e. the titles

		\param parser The parser for which the row is to be skipped
	*/
	CSV_PARSER_API void csv_parser_skip_row(CSV_PARSER *parser);

	//
	// [IMPLEMENTATION]
	//

#ifdef CSV_PARSER_IMPLEMENTATION

	static size_t _csv_parser_get_file_size(FILE *fp) {
		fseek(fp, 0L, SEEK_END);
		long f_size = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		return f_size;
	}

#define _CSV_PARSER_ISSPACE(ch) ((ch) == ' ' || (ch) == '\f' || (ch) == '\n' || (ch) == '\r' || (ch) == '\t' || (ch) == '\v')

	static int64_t _csv_parser_count_columns(CSV_PARSER *parser) {
		int64_t count = 0;

		uint8_t *end = parser->buffer + parser->buffer_length;

		uint8_t *start = parser->position;

		while (parser->position < end && *parser->position != '\n') {
			switch (*parser->position) {
			case '"': // field inside double quotes
				while (parser->position < end && *parser->position != '"') {
					if (*parser->position == '\n' || *parser->position == '\r') {
						parser->error.reason = "Could not find matching \".";
						parser->error.column = parser->position - start;
						return -1;
					}
					parser->position += 1;
				}
				if (parser->position == end) {
					parser->error.reason = "Could not find matching \". Reached end of file.";
					parser->error.column = parser->position - start;
					return -1;
				}
				break;
			case '\'': // field inside single quotes
				while (parser->position < end && *parser->position != '\'') {
					if (*parser->position == '\n' || *parser->position == '\r') {
						parser->error.reason = "Could not find matching '.";
						parser->error.column = parser->position - start;
						return -1;
					}
					parser->position += 1;
				}
				if (parser->position == end) {
					parser->error.reason = "Could not find matching '. Reached end of file.";
					parser->error.column = parser->position - start;
					return -1;
				}
				break;
			case ',':
				*parser->position = '\0';
				start = parser->position + 1;
				count++;
				break;
			default:
				break;
			}

			parser->position += 1;
		}

		// If we have found some number of elements and '\n' at the end, 
		// the total number of columns needs to be increased by 1
		count += (count != 0);

		return count;
	}

	static CSV_PARSER_Bool _csv_parser_count_lines_and_columns(CSV_PARSER *parser) {
		CSV_PARSER_ASSERT(parser->buffer && parser->position);

		int64_t columns = _csv_parser_count_columns(parser);
		if (columns == -1) {
			parser->error.line = 1;
			return 0;
		}
		else if (columns == 0) {
			parser->error.line = 1;
			parser->error.reason = "Bad CSV file";
		}

		uint8_t *end = parser->buffer + parser->buffer_length;

		while (parser->position < end && _CSV_PARSER_ISSPACE(*parser->position)) {
			*parser->position = '\0';
			parser->position += 1;
		}

		int64_t next_columns;
		while ((next_columns = _csv_parser_count_columns(parser))) {
			if (next_columns == -1 || columns != next_columns) {
				parser->error.line = parser->lines + 1;
				if (columns != next_columns) {
					parser->error.reason = "Not enough number of values.";
				}
				return 0;
			}

			parser->lines += 1;

			while (parser->position < end && _CSV_PARSER_ISSPACE(*parser->position)) {
				*parser->position = '\0';
				parser->position += 1;
			}
		}

		parser->lines += 1;
		parser->columns = columns;
		parser->position = parser->buffer;
		return 1;
	}

	CSV_PARSER_DEFN_API void csv_parser_init(CSV_PARSER *parser, void *allocator_context) {
		parser->buffer = NULL;
		parser->buffer_length = 0;
		parser->columns = 0;
		parser->lines = 0;
		parser->position = 0;
		parser->allocator_context = allocator_context;

		parser->error.reason = NULL;
		parser->error.column = 0;
		parser->error.line = 0;
	}

	CSV_PARSER_DEFN_API void *csv_parser_malloc(size_t size, void *context) {
		return CSV_PARSER_MALLOC(size, context);
	}

	CSV_PARSER_DEFN_API void csv_parser_free(void *ptr, void *context) {
		CSV_PARSER_FREE(ptr, context);
	}

	CSV_PARSER_DEFN_API uint8_t *csv_parser_duplicate_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length) {
		uint8_t *dst = csv_parser_malloc((length + 1) * sizeof(*buffer), parser->allocator_context);
		if (dst) {
			CSV_PARSER_MEMCPY(dst, buffer, length);
			dst[length] = 0;
			return dst;
		}
		return NULL;
	}

	CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_parser_load_buffer(CSV_PARSER *parser, uint8_t *buffer, size_t length) {
		parser->buffer = buffer;
		parser->buffer_length = length;
		parser->position = buffer;
		parser->columns = 0;
		parser->lines = 0;

		return _csv_parser_count_lines_and_columns(parser);
	}

	CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_parser_load_duplicated(CSV_PARSER *parser, uint8_t *buffer, size_t length) {
		uint8_t *duplicate = csv_parser_duplicate_buffer(parser, buffer, length);
		if (duplicate) {
			return csv_parser_load_buffer(parser, duplicate, length);
		}
		return 0;
	}

#ifdef CSV_PARSER_STDIO_INCLUDED
	CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_parser_load_file(CSV_PARSER *parser, FILE *fp) {
		CSV_PARSER_ASSERT(fp);
		size_t buffer_length = _csv_parser_get_file_size(fp);
		uint8_t *buffer = csv_parser_malloc((buffer_length + 1) * sizeof(*buffer), parser->allocator_context);
		if (buffer == NULL) {
			parser->error.reason = "Allocation failed. Out of memory.";
			return 0;
		}
		buffer[buffer_length] = 0;
		size_t result = fread(buffer, buffer_length, 1, fp);

		if (result != 1) {
			parser->error.reason = "File could not be read.";
			return 0;
		}
		return csv_parser_load_buffer(parser, buffer, buffer_length);
	}

	CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_parser_load(CSV_PARSER *parser, const char *file_path) {
		FILE *fp = fopen(file_path, "rb");
		if (fp) {
			CSV_PARSER_Bool result = csv_parser_load_file(parser, fp);
			fclose(fp);
			return result;
		}
		parser->error.reason = "File could not be opened for reading.";
		return 0;
	}

	CSV_PARSER_DEFN_API void csv_parser_release(CSV_PARSER *parser) {
		csv_parser_free(parser->buffer, parser->allocator_context);
	}

#endif

	CSV_PARSER_DEFN_API uint8_t *csv_parser_next(CSV_PARSER *parser, size_t *length) {
		uint8_t *end = parser->buffer + parser->buffer_length;

		while (parser->position < end && !*parser->position)
			parser->position += 1;

		uint8_t *next_token = parser->position;

		while (parser->position < end && *parser->position)
			parser->position += 1;

		*length = parser->position - next_token;

		return next_token;
	}

	CSV_PARSER_DEFN_API void csv_parser_skip_row(CSV_PARSER *parser) {
		size_t length = 0;
		for (size_t col = 0; col < parser->columns; ++col) {
			csv_parser_next(&parser, &length);
		}
	}

#endif // CSV_PARSER_IMPLEMENTATION

#if !defined(__cplusplus)
}
#endif

#endif
