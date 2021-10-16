/*! \file csv_deserializer.h
	\brief A Library for deserializing CSV files. csvParser is header only library, to use this, just include this file.
*/

#ifndef CSV_DESERIALIZER_HPP
#define CSV_DESERIALIZER_HPP

#include "csv_parser.h"

/*! \def CSV_STRING_DUPLICATE
	\brief To use custom srtdup CSV_STRING_DUPLICATE needs to get redefined before including csv_deserializer.h. Doing this string.h will not be included
*/
#ifndef CSV_STRING_DUPLICATE
#include <string.h>
#ifdef _MSC_VER
#define CSV_STRING_DUPLICATE _strdup
#else
#define CSV_STRING_DUPLICATE strdup
#endif
#endif

/*! \def CSV_STRING_TO_INT64
	\brief To use custom strtoll, CSV_STRING_TO_INT64 needs to get redefined before including csv_deserializer.h. Doing this stdlib.h will not be included
*/
/*! \def CSV_STRING_TO_UINT64
	\brief To use custom strtoull, CSV_STRING_TO_UINT64 needs to get redefined before including csv_deserializer.h. Doing this stdlib.h will not be included
*/
/*! \def CSV_STRING_TO_DOUBLE
	\brief To use custom strtod, CSV_STRING_TO_DOUBLE needs to get redefined before including csv_deserializer.h. Doing this stdlib.h will not be included
*/
#if !defined(CSV_STRING_TO_INT64) || !defined(CSV_STRING_TO_UINT64) || !defined(CSV_STRING_TO_DOUBLE)
#include <stdlib.h>
#define CSV_STRING_TO_INT64 strtoll
#define CSV_STRING_TO_UINT64 strtoull
#define CSV_STRING_TO_DOUBLE strtod
#endif

/*! \struct CSV_PARSER_STRING
	\brief String representation with length. Useful instead of using strlen multiple times
*/
typedef struct CSV_PARSER_STRING
{
	uint8_t *data;	/*!< Pointer to the underlying c string */
	size_t len;		/*!< Length of the string */
} CSV_PARSER_STRING;

/*! \fn CSV_PARSER_Bool csv_deserialize_length_string(void *context, uint8_t *value, size_t len, CSV_PARSER_STRING *out)
	\brief Procedure that is intended as input to [CSV_DESERIALIZE_DESC](@ref CSV_DESERIALIZE_DESC) which deserializes [CSV_PARSER_STRING](@ref CSV_PARSER_STRING)

	\param context The user variable that is passed to the CSV_Deserializer
	\param value The string which is to be deserialized
	\param len The length of the string pointed by value
	\param out The deserialized [CSV_PARSER_STRING](@ref CSV_PARSER_STRING)
	\return Non zero value if the deserialization was successful
*/
CSV_PARSER_API CSV_PARSER_Bool csv_deserialize_length_string(void *context, uint8_t *value, size_t len, CSV_PARSER_STRING *out);

/*! \fn CSV_PARSER_Bool csv_deserialize_string(void *context, uint8_t *value, size_t len, char **out)
	\brief Procedure that is intended as input to [CSV_DESERIALIZE_DESC](@ref CSV_DESERIALIZE_DESC) which deserializes C string

	\param context The user variable that is passed to the CSV_Deserializer
	\param value The string which is to be deserialized
	\param len The length of the string pointed by value
	\param out The deserialized C string
	\return Non zero value if the deserialization was successful
*/
CSV_PARSER_API CSV_PARSER_Bool csv_deserialize_string(void *context, uint8_t *value, size_t len, char **out);

/*! \fn CSV_PARSER_Bool csv_deserialize_length_stringdup(void *context, uint8_t *value, size_t len, CSV_PARSER_STRING *out)
	\brief Same as [csv_deserialize_length_string](@ref csv_deserialize_length_string) but duplicates the string by [CSV_STRING_DUPLICATE](@ref CSV_STRING_DUPLICATE) which must be freed later

	\param context The user variable that is passed to the CSV_Deserializer
	\param value The string which is to be deserialized
	\param len The length of the string pointed by value
	\param out The deserialized [CSV_PARSER_STRING](@ref CSV_PARSER_STRING)
	\return Non zero value if the deserialization was successful
*/
CSV_PARSER_API CSV_PARSER_Bool csv_deserialize_length_stringdup(void *context, uint8_t *value, size_t len, CSV_PARSER_STRING *out);

/*! \fn csv_deserialize_stringdup(void *context, uint8_t *value, size_t len, char **out)
	\brief Same as [csv_deserialize_string](@ref csv_deserialize_string) but duplicates the string by [CSV_STRING_DUPLICATE](@ref CSV_STRING_DUPLICATE) which must be freed later

	\param context The user variable that is passed to the CSV_Deserializer
	\param value The string which is to be deserialized
	\param len The length of the string pointed by value
	\param out The deserialized C string
	\return Non zero value if the deserialization was successful
*/
CSV_PARSER_API CSV_PARSER_Bool csv_deserialize_stringdup(void *context, uint8_t *value, size_t len, char **out);

/*! \fn CSV_PARSER_Bool csv_deserialize_sint(void *context, uint8_t *value, size_t len, int64_t *out)
	\brief Procedure that is intended as input to [CSV_DESERIALIZE_DESC](@ref CSV_DESERIALIZE_DESC) which deserializes int64_t

	\param context The user variable that is passed to the CSV_Deserializer
	\param value The string which is to be deserialized
	\param len The length of the string pointed by value
	\param out The deserialized int64_t
	\return Non zero value if the deserialization was successful
*/
CSV_PARSER_API CSV_PARSER_Bool csv_deserialize_sint(void *context, uint8_t *value, size_t len, int64_t *out);

/*! \fn CSV_PARSER_Bool csv_deserialize_uint(void *context, uint8_t *value, size_t len, uint64_t *out)
	\brief Procedure that is intended as input to [CSV_DESERIALIZE_DESC](@ref CSV_DESERIALIZE_DESC) which deserializes uint64_t

	\param context The user variable that is passed to the CSV_Deserializer
	\param value The string which is to be deserialized
	\param len The length of the string pointed by value
	\param out The deserialized uint64_t
	\return Non zero value if the deserialization was successful
*/
CSV_PARSER_API CSV_PARSER_Bool csv_deserialize_uint(void *context, uint8_t *value, size_t len, uint64_t *out);

/*! \fn CSV_PARSER_Bool csv_deserialize_boolean(void *context, uint8_t *value, size_t len, CSV_PARSER_Bool *out)
	\brief Procedure that is intended as input to [CSV_DESERIALIZE_DESC](@ref CSV_DESERIALIZE_DESC) which deserializes CSV_PARSER_Bool

	\param context The user variable that is passed to the CSV_Deserializer
	\param value The string which is to be deserialized
	\param len The length of the string pointed by value
	\param out The deserialized CSV_PARSER_Bool
	\return Non zero value if the deserialization was successful
*/
CSV_PARSER_API CSV_PARSER_Bool csv_deserialize_boolean(void *context, uint8_t *value, size_t len, CSV_PARSER_Bool *out);

/*! \fn CSV_PARSER_Bool csv_deserialize_real(void *context, uint8_t *value, size_t len, double *out)
	\brief Procedure that is intended as input to [CSV_DESERIALIZE_DESC](@ref CSV_DESERIALIZE_DESC) which deserializes double

	\param context The user variable that is passed to the CSV_Deserializer
	\param value The string which is to be deserialized
	\param len The length of the string pointed by value
	\param out The deserialized double
	\return Non zero value if the deserialization was successful
*/
CSV_PARSER_API CSV_PARSER_Bool csv_deserialize_real(void *context, uint8_t *value, size_t len, double *out);

/*! \var typedef CSV_PARSER_Bool(*CSV_Deserializer)(void *, uint8_t *, size_t, void *)
	\brief A pointer to a procedure that is used in [CSV_DESERIALIZE_DESC](@ref CSV_DESERIALIZE_DESC) for deserializing strings into data required
*/
typedef CSV_PARSER_Bool(*CSV_Deserializer)(void *, uint8_t *, size_t, void *);

/*! \struct CSV_DESERIALIZE_DESC
	\brief String representation with length. Useful instead of using strlen multiple times
*/
typedef struct CSV_DESERIALIZE_DESC
{
	CSV_Deserializer *	deserializer;	/*!< Pointer to the procedures of type [CSV_Deserializer](@ref CSV_Deserializer) that deserializes the input string as required */
	size_t *			offset;			/*!< Provide offset of 0th member, 1st and so-on */
	size_t				length;			/*!< Count of the members of the array deserializer and offset */
} CSV_DESERIALIZE_DESC;

/*! \fn size_t csv_deserialize(void *context, void *ptr_to_struct, CSV_DESERIALIZE_DESC *desc, size_t stride, CSV_PARSER *parser, size_t no_of_records)
	\brief Deserializes the required number of records from the CSV buffer and places in the input buffer

	\param context The user variable that is passed to the CSV_Deserializer from desc
	\param ptr_to_struct The buffer where the data gets placed
	\param desc The layout of the buffer. The desc->length MUST be equal to parser->columns
	\param stride The value added to ptr_to_struct to get to the next place where the next data is to be placed
	\param parser The parser from which the CSV is to be deserialized
	\param no_of_records Total number of records to deserialize
	\return Number of records actually deserialized
*/
CSV_PARSER_API size_t csv_deserialize(void *context, void *ptr_to_struct, CSV_DESERIALIZE_DESC *desc, size_t stride, CSV_PARSER *parser, size_t no_of_records);

//
// [IMPLEMENTATION]
//

#ifdef CSV_DESERIALIZER_IMPLEMENTATION

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_deserialize_length_string(void *context, uint8_t *value, size_t len, CSV_PARSER_STRING *out)
{
	if (len) {
		if (value[0] != '"') {
			out->data = value;
			out->len = len;
			return 1;
		}

		value[len - 1] = 0; // removing quote at the end
		out->data = value + 1;
		out->len = len - 1;
		return 1;
	}
	return 0;
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_deserialize_string(void *context, uint8_t *value, size_t len, char **out)
{
	if (len) {
		if (value[0] != '"') {
			*out = value;
			return 1;
		}

		value[len - 1] = 0; // removing quote at the end
		*out = value + 1;
		return 1;
	}
	return 0;
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_deserialize_length_stringdup(void *context, uint8_t *value, size_t len, CSV_PARSER_STRING *out)
{
	if (len) {
		if (value[0] != '"') {
			out->data = CSV_STRING_DUPLICATE(value);
			out->len = len;
			return 1;
		}

		value[len - 1] = 0; // removing quote at the end
		out->data = value + 1;
		out->len = len - 1;
		return 1;
	}
	return 0;
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_deserialize_stringdup(void *context, uint8_t *str, size_t len, char **value)
{
	char *ptr = CSV_STRING_DUPLICATE(str);
	if (!ptr)
		return 0;
	// ---> Unsafe casting here <----- ; This is C land.. who cares 
	*value = ptr;
	// <------------------------------> 
	return 1;
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_deserialize_sint(void *context, uint8_t *value, size_t len, int64_t *out)
{
	char *end = NULL;
	long long num = CSV_STRING_TO_INT64(value, &end, 10);
	if (value + len == end) {
		*out = (int64_t)num;
		return 1;
	}
	return 0;
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_deserialize_uint(void *context, uint8_t *value, size_t len, uint64_t *out)
{
	char *end = NULL;
	long long num = CSV_STRING_TO_UINT64(value, &end, 10);
	if (value + len == end) {
		*out = (uint64_t)num;
		return 1;
	}
	return 0;
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_deserialize_boolean(void *context, uint8_t *value, size_t len, CSV_PARSER_Bool *out)
{
	switch (len) {
	case 1:
		*out = (value[0] == '1');
		return value[0] == '0' || value[0] == '1';

	case 4:
		if ((value[0] == 't' || value[0] == 'T') &&
			(value[1] == 'r' || value[1] == 'R') &&
			(value[2] == 'u' || value[2] == 'U') &&
			(value[3] == 'e' || value[3] == 'E')) {
			*out = 1;
			return 1;
		}

	case 5:
		if ((value[0] == 'f' || value[0] == 'F') &&
			(value[1] == 'a' || value[1] == 'A') &&
			(value[2] == 'l' || value[2] == 'L') &&
			(value[3] == 's' || value[3] == 'S') &&
			(value[4] == 'e' || value[4] == 'E')) {
			*out = 0;
			return 1;
		}
	}

	return 0;
}

CSV_PARSER_DEFN_API CSV_PARSER_Bool csv_deserialize_real(void *context, uint8_t *value, size_t len, double *out)
{
	char *end = NULL;
	double num = CSV_STRING_TO_DOUBLE(value, &end);
	if (value + len == end) {
		*out = num;
		return 1;
	}
	return 0;
}

// Macro of all the offsets

/* #define offsets(structure,...) \ */
/*   (sizeof((int[]){__VA_ARGS__})/sizeof(int))  */

// To make it typesafe, something like float, int should be included .. i.e. limited support of types
// function pointers

CSV_PARSER_DEFN_API size_t csv_deserialize(void *context, void *ptr_to_struct, CSV_DESERIALIZE_DESC *desc, size_t stride, CSV_PARSER *parser, size_t no_of_records) {
	CSV_PARSER_ASSERT(ptr_to_struct != NULL && desc != NULL);

	size_t length = 0;
	// Fill 1 row each a time

	CSV_PARSER_ASSERT(desc->length == parser->columns);

	size_t lines_to_parse = parser->lines - 1; // 1st row consumed by the header information

	size_t records_to_parse = lines_to_parse;
	if (no_of_records < records_to_parse)
	{
		records_to_parse = no_of_records;
	}

	for (size_t record = 0; record < records_to_parse; ++record)
	{

		for (size_t col = 0; col < parser->columns; ++col)
		{
			uint8_t *value = csv_parser_next(parser, &length);
			desc->deserializer[col](context, value, length, (char *)ptr_to_struct + desc->offset[col]);
		}
		ptr_to_struct = (char *)ptr_to_struct + stride;
	}

	return records_to_parse;
}

#endif // CSV_DESERIALIZER_IMPLEMENTATION

#endif
