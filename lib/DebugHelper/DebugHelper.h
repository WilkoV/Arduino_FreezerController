#pragma once

#ifndef _DEBUGHELPER_h
    #define _DEBUGHELPER_h

    // If DEBUG_HELPER is defined implement the macros
	#ifdef DEBUG_HELPER
        // Print timestamp (internal counter), line number, function name.
        // This macro shouldn't called directly
		#define DEBUG_LOCATION \
			Serial.print(millis()); \
			Serial.print(F("ms: (")); \
			Serial.print(__LINE__); \
			Serial.print(F("): ")); \
			Serial.print(__PRETTY_FUNCTION__); \
			Serial.print(F(": "));

        // Print location and the message followed by new line
		#define DEBUG_PRINTLN(message) \
            DEBUG_LOCATION; \
			Serial.println(message);

        // Print location and the message without by new line
		#define DEBUG_PRINT(message) \
			DEBUG_LOCATION; \
			Serial.print(message);

        // Append a message to the current line
		#define DEBUG_PRINT_APPEND(message) \
			Serial.print(message);

		#define DEBUG_PRINT_APPEND_HEX(message) \
			Serial.print(message, HEX);

        // Append a message and a new line construction to the current line
		#define DEBUG_PRINTLN_APPEND(message) \
			Serial.println(message);

        // Print location, key (label), value and a new line instruction. Key and values are separated by an equal sign
		#define DEBUG_PRINTLN_KEY_VALUE(key, value) \
			DEBUG_LOCATION; \
			Serial.print(key); \
			Serial.print(F(" = ")); \
			Serial.println(value);

        // Print location, key (label), value and a new line instruction. Key and values are separated by an equal sign. Value is
        // surrounded by >> and <<. This is usefull if values could be empty.
        #define DEBUG_PRINTLN_KEY_VALUE_START_END_SIGN(key, value) \
			DEBUG_LOCATION; \
			Serial.print(key); \
			Serial.print(F(" = ")); \
			Serial.print(F(">>")); \
			Serial.print(value); \
			Serial.println(F("<<"));

        // Print location, key (label), value without a new line instruction. Key and values are separated by an equal sign
		#define DEBUG_PRINT_KEY_VALUE(key, value) \
			DEBUG_LOCATION; \
			Serial.print(key); \
			Serial.print(F(" = ")); \
			Serial.print(value);

        // Print append local time
        #define DEBUG_PRINTLN_APPEND_TIME(localTime) \
            Serial.print("-"); \
            Serial.print(month(localTime)); \
            Serial.print("-"); \
            Serial.print(day(localTime)); \
            Serial.print(" "); \
            Serial.print(hour(localTime)); \
            Serial.print(":"); \
            Serial.print(minute(localTime)); \
            Serial.print(":"); \
            Serial.print(second(localTime)); \
            Serial.println("");

        // Print a empty row
        #define DEBUG_EMPTY_ROW \
            Serial.println("");

        // Initialize Serial port
    	#define DEBUG_BEGIN(baudRate) \
            Serial.begin(baudRate);

	#else
        // If DEBUG_HELPER is not defined generate empty macros and the compiler will do nothing
		#define DEBUG_LOCATION;
		#define DEBUG_PRINTLN(message);
		#define DEBUG_PRINT(message);
		#define DEBUG_PRINT_APPEND_HEX(message);
		#define DEBUG_PRINT_APPEND(message);
		#define DEBUG_PRINTLN_APPEND(message);
		#define DEBUG_PRINTLN_KEY_VALUE(key, value);
		#define DEBUG_PRINTLN_KEY_VALUE_START_END_SIGN(key, value);
		#define DEBUG_PRINT_KEY_VALUE(key, value);
        #define DEBUG_PRINTLN_APPEND_TIME(localTime);
        #define DEBUG_EMPTY_ROW;
    	#define DEBUG_BEGIN(baudRate);
    #endif

#endif