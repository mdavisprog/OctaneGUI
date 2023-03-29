/*

MIT License

Copyright (c) 2023 Mitchell Davis <mdavisprog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


#ifndef __LSTALK_H__
#define __LSTALK_H__

#if LSTALK_LIB
    #if LSTALK_STATIC
        #define LSTALK_API
    #else
        #ifndef LSTALK_API
            #if defined(_WIN32) || defined(_WIN64)
                #if LSTALK_EXPORT
                    #define LSTALK_API __declspec(dllexport)
                #else
                    #define LSTALK_API __declspec(dllimport)
                #endif
            #else
                #define LSTALK_API
            #endif
        #endif
    #endif
#else
    #define LSTALK_API
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Boolean type defined to aid in documenting property types.
 */
typedef unsigned char lstalk_bool;
#define lstalk_false 0
#define lstalk_true 1

/**
 * Data container for a LSTalk session. Must be created using lstalk_init
 * and destroyed with lstalk_shutdown.
 */
struct LSTalk_Context;

/**
 * ID represting a connection to a language server. The LSTalk_Context
 * object will hold these values when a connection is opened.
 */
typedef int LSTalk_ServerID;
#define LSTALK_INVALID_SERVER_ID -1

/**
 * The connection status to a server.
 */
typedef enum {
    LSTALK_CONNECTION_STATUS_NOT_CONNECTED,
    LSTALK_CONNECTION_STATUS_CONNECTING,
    LSTALK_CONNECTION_STATUS_CONNECTED,
} LSTalk_ConnectionStatus;

/**
 * Level of verbosity with which the server systematically reports its execution
 */
typedef enum {
    LSTALK_TRACE_OFF,
    LSTALK_TRACE_MESSAGES,
    LSTALK_TRACE_VERBOSE,
} LSTalk_Trace;

/**
 * Flags to set to aid in debugging the library.
 */
typedef enum {
    LSTALK_DEBUGFLAGS_NONE = 0,
    LSTALK_DEBUGFLAGS_PRINT_REQUESTS = 1 << 0,
    LSTALK_DEBUGFLAGS_PRINT_RESPONSES = 1 << 1,
} LSTalk_DebugFlags;

/**
 * Parameters to set when initially connecting to a language server.
 */
typedef struct LSTalk_ConnectParams {
    /**
     * The rootUri of the workspace. Is null if no folder is open.
     */
    char* root_uri;

    /**
     * The initial trace setting. If omitted trace is disabled ('off').
     */
    LSTalk_Trace trace;

    /**
     * Will seek through the PATH environment variable that fits the
     * given URI in lstalk_connect.
     */
    int seek_path_env;
} LSTalk_ConnectParams;

/**
 * Forward declaraction with the defintion defined below the API.
 */
struct LSTalk_Notification;

/**
 * Initializes a LSTalk_Context object to be used with all of the API functions.
 * 
 * @return A heap-allocated LSTalk_Context object. Must be freed with lstalk_shutdown.
 */
LSTALK_API struct LSTalk_Context* lstalk_init();

/**
 * Cleans up a LSTalk_Context object. This will close any existing connections to servers
 * and send shutdown/exit requests to them. The context object memory is then freed.
 * 
 * @param context - The context object to shutdown.
 */
LSTALK_API void lstalk_shutdown(struct LSTalk_Context* context);

/**
 * Retrieves the current version number for the LSTalk library.
 * 
 * @param major - A pointer to store the major version number.
 * @param minor - A pointer to store the minor version number.
 * @param revision - A pointer to store the revision number.
 */
LSTALK_API void lstalk_version(int* major, int* minor, int* revision);

/**
 * Sets the client information for the given LSTalk_Context object. The default
 * name is "lstalk" and the default version is the library's version number.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param name - A pointer to the name of the client. This function will allocate
 *               its own copy of the string.
 * @param version - A pointer to the version of the client. This function will
 *                  allocate its own copy of the string.
 */
LSTALK_API void lstalk_set_client_info(struct LSTalk_Context* context, const char* name, const char* version);

/**
 * Sets the locale of the client. The default value is 'en'.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param locale - This is an IETF tag. The string is copied.
 */
LSTALK_API void lstalk_set_locale(struct LSTalk_Context* context, const char* locale);

/**
 * Sets debug flags for the given context object.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param flags - Bitwise flags set from LSTalk_DebugFlags.
 */
LSTALK_API void lstalk_set_debug_flags(struct LSTalk_Context* context, int flags);

/**
 * Attempts to connect to a language server at the given URI. This should be a path on the machine to an
 * executable that can be started by the library.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param uri - File path to the language server executable.
 * 
 * @return - Server ID representing a connection to the language server. Will be LSTALK_INVALID_SERVER_ID if
 *           no connection can be made.
 */
LSTALK_API LSTalk_ServerID lstalk_connect(struct LSTalk_Context* context, const char* uri, LSTalk_ConnectParams* connect_params);

/**
 * Retrieve the current connection status given a Server ID.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param id - A LSTalk_ServerID to check the connection status for.
 * 
 * @return - The LSTalk_ConnectionStatus of the given server ID.
 */
LSTALK_API LSTalk_ConnectionStatus lstalk_get_connection_status(struct LSTalk_Context* context, LSTalk_ServerID id);

/**
 * Information about the server.
 *
 * @since 3.15.0
 */
typedef struct LSTalk_ServerInfo {
    /**
     * The name of the server as defined by the server.
     */
    char* name;

    /**
     * The server's version as defined by the server.
     */
    char* version;
} LSTalk_ServerInfo;

/**
 * Retrieve the server information given a LSTalker_ServerID.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param id - A LSTalk_ServerID to check the connection status for.
 * 
 * @return - The LSTalk_ServerInfo containing information about the server.
 */
LSTALK_API LSTalk_ServerInfo* lstalk_get_server_info(struct LSTalk_Context* context, LSTalk_ServerID id);

/**
 * Requests to close a connection to a connected language server given the LSTalk_ServerID.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param id - The LSTalk_ServerID connection to close.
 * 
 * @return - A non-zero value if closed successfully. 0 if there was an error.
 */
LSTALK_API int lstalk_close(struct LSTalk_Context* context, LSTalk_ServerID id);

/**
 * Process responses for all connected server.
 * 
 * @param context - An initialized LSTalk_Context object.
 * 
 * @return - A non-zero value if response were processed. 0 if nothing was processed.
 */
LSTALK_API int lstalk_process_responses(struct LSTalk_Context* context);

/**
 * Polls for any notifications received from the given server. The context will hol any
 * memory allocated for the notification. Once the notification has been polled, the
 * memory will be freed on the next lstalk_process_responses call. The caller will
 * need to create their own copy of any data they wish to hold onto.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param id - The server connection to poll.
 * @param notification - Next notification received from the server.
 * 
 * @return - A non-zero value if a notification was polled. 0 if no notification was polled.
 */
LSTALK_API int lstalk_poll_notification(struct LSTalk_Context* context, LSTalk_ServerID id, struct LSTalk_Notification* notification);

/**
 * A notification that should be used by the client to modify the trace setting of the server.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param id - The LSTalk_ServerID connection to set the trace setting on.
 * @param trace - A LSTalk_Trace value to set the new setting.
 * 
 * @return - Non-zero if the request was sent. 0 if it failed.
 */
LSTALK_API int lstalk_set_trace(struct LSTalk_Context* context, LSTalk_ServerID id, LSTalk_Trace trace);

/**
 * Calls lstalk_set_trace by converting the string into an LSTalk_Trace value.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param id - The LSTalk_ServerID connection to set the trace setting on.
 * @param trace - A string representation of a LSTalk_Trace value.
 * 
 * @return - Non-zero if the request was sent. 0 if it failed.
 */
LSTALK_API int lstalk_set_trace_from_string(struct LSTalk_Context* context, LSTalk_ServerID id, const char* trace);

/**
 * The document open notification is sent from the client to the server to
 * signal newly opened text documents. The library will attempt to open the
 * file to send the contents to the server. The contents will be properly
 * escaped to fit the JSON rpc format.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param id - The LSTalk_ServerID connection to open the document on.
 * @param path - The absolute path to the file that is opened on the client.
 * 
 * @return - Non-zero if the request was sent. 0 if it failed.
 */
LSTALK_API int lstalk_text_document_did_open(struct LSTalk_Context* context, LSTalk_ServerID id, const char* path);

/**
 * The document close notification is sent from the client to the server
 * when the document got closed in the client.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param id - The LSTalk_ServerID connection to open the document on.
 * @param path - The absolute path to the file that is opened on the client.
 * 
 * @return - Non-zero if the request was sent. 0 if it failed.
 */
LSTALK_API int lstalk_text_document_did_close(struct LSTalk_Context* context, LSTalk_ServerID id, const char* path);

/**
 * Retrieves all symbols for a given document.
 * 
 * @param context - An initialized LSTalk_Context object.
 * @param id - The LSTalk_ServerID connection to open the document on.
 * @param path - The absolute path to the file that is opened on the client.
 * 
 * @return - Non-zero if the request was sent. 0 if it failed.
 */
LSTALK_API int lstalk_text_document_symbol(struct LSTalk_Context* context, LSTalk_ServerID id, const char* path);

//
// The section below contains the definitions of interfaces used in communicating
// with the language server.
//

/**
 * A symbol kind.
 */
typedef enum {
    LSTALK_SYMBOLKIND_NONE = 0,
    LSTALK_SYMBOLKIND_FILE = 1 << 0,
    LSTALK_SYMBOLKIND_MODULE = 1 << 1,
    LSTALK_SYMBOLKIND_NAMESPACE = 1 << 2,
    LSTALK_SYMBOLKIND_PACKAGE = 1 << 3,
    LSTALK_SYMBOLKIND_CLASS = 1 << 4,
    LSTALK_SYMBOLKIND_METHOD = 1 << 5,
    LSTALK_SYMBOLKIND_PROPERTY = 1 << 6,
    LSTALK_SYMBOLKIND_FIELD = 1 << 7,
    LSTALK_SYMBOLKIND_CONSTRUCTOR = 1 << 8,
    LSTALK_SYMBOLKIND_ENUM = 1 << 9,
    LSTALK_SYMBOLKIND_INTERFACE = 1 << 10,
    LSTALK_SYMBOLKIND_FUNCTION = 1 << 11,
    LSTALK_SYMBOLKIND_VARIABLE = 1 << 12,
    LSTALK_SYMBOLKIND_CONSTANT = 1 << 13,
    LSTALK_SYMBOLKIND_STRING = 1 << 14,
    LSTALK_SYMBOLKIND_NUMBER = 1 << 15,
    LSTALK_SYMBOLKIND_BOOLEAN = 1 << 16,
    LSTALK_SYMBOLKIND_ARRAY = 1 << 17,
    LSTALK_SYMBOLKIND_OBJECT = 1 << 18,
    LSTALK_SYMBOLKIND_KEY = 1 << 19,
    LSTALK_SYMBOLKIND_NULL = 1 << 20,
    LSTALK_SYMBOLKIND_ENUMMEMBER = 1 << 21,
    LSTALK_SYMBOLKIND_STRUCT = 1 << 22,
    LSTALK_SYMBOLKIND_EVENT = 1 << 23,
    LSTALK_SYMBOLKIND_OPERATOR = 1 << 24,
    LSTALK_SYMBOLKIND_TYPEPARAMETER = 1 << 25,
    LSTALK_SYMBOLKIND_ALL =  LSTALK_SYMBOLKIND_FILE
        | LSTALK_SYMBOLKIND_MODULE
        | LSTALK_SYMBOLKIND_NAMESPACE
        | LSTALK_SYMBOLKIND_PACKAGE
        | LSTALK_SYMBOLKIND_CLASS
        | LSTALK_SYMBOLKIND_METHOD
        | LSTALK_SYMBOLKIND_PROPERTY
        | LSTALK_SYMBOLKIND_FIELD
        | LSTALK_SYMBOLKIND_CONSTRUCTOR
        | LSTALK_SYMBOLKIND_ENUM
        | LSTALK_SYMBOLKIND_INTERFACE
        | LSTALK_SYMBOLKIND_FUNCTION
        | LSTALK_SYMBOLKIND_VARIABLE
        | LSTALK_SYMBOLKIND_CONSTANT
        | LSTALK_SYMBOLKIND_STRING
        | LSTALK_SYMBOLKIND_NUMBER
        | LSTALK_SYMBOLKIND_BOOLEAN
        | LSTALK_SYMBOLKIND_ARRAY
        | LSTALK_SYMBOLKIND_OBJECT
        | LSTALK_SYMBOLKIND_KEY
        | LSTALK_SYMBOLKIND_NULL
        | LSTALK_SYMBOLKIND_ENUMMEMBER
        | LSTALK_SYMBOLKIND_STRUCT
        | LSTALK_SYMBOLKIND_EVENT
        | LSTALK_SYMBOLKIND_OPERATOR
        | LSTALK_SYMBOLKIND_TYPEPARAMETER,
} LSTalk_SymbolKind;

/**
 * Returns a string representation of a given symbol kind.
 * 
 * @param kind - The kind of symbol to return as a string.
 * 
 * @return - String representing the given symbol kind.
 */
LSTALK_API char* lstalk_symbol_kind_to_string(LSTalk_SymbolKind kind);

/**
 * Symbol tags are extra annotations that tweak the rendering of a symbol.
 *
 * @since 3.16
 */
typedef enum {
    LSTALK_SYMBOLTAG_DEPRECATED = 1 << 0,
} LSTalk_SymbolTag;

/**
 * Position in a text document expressed as zero-based line and zero-based
 * character offset. A position is between two characters like an ‘insert’
 * cursor in an editor. Special values like for example -1 to denote the end
 * of a line are not supported.
 */
typedef struct LSTalk_Position {
    /**
     * Line position in a document (zero-based).
     */
    unsigned int line;

    /**
     * Character offset on a line in a document (zero-based). The meaning of this
     * offset is determined by the negotiated `PositionEncodingKind`.
     *
     * If the character value is greater than the line length it defaults back
     * to the line length.
     */
    unsigned int character;
} LSTalk_Position;

/**
 * A range in a text document expressed as (zero-based) start and end positions.
 * A range is comparable to a selection in an editor. Therefore the end position
 * is exclusive. If you want to specify a range that contains a line including the
 * line ending character(s) then use an end position denoting the start of the
 * next line.
 */
typedef struct LSTalk_Range {
    /**
     * The range's start position.
     */
    LSTalk_Position start;

    /**
     * The range's end position.
     */
    LSTalk_Position end;
} LSTalk_Range;

/**
 * Represents a location inside a resource, such as a line inside a text file.
 */
typedef struct LSTalk_Location {
    char* uri;
    LSTalk_Range range;
} LSTalk_Location;

/**
 * A diagnostic's severity.
 */
typedef enum {
    /**
     * Reports an error.
     */
    LSTALK_DIAGNOSTICSEVERITY_ERROR = 1,

    /**
     * Reports a warning.
     */
    LSTALK_DIAGNOSTICSEVERITY_WARNING = 2,

    /**
     * Reports an information.
     */
    LSTALK_DIAGNOSTICSEVERITY_INFORMATION = 3,

    /**
     * Reports a hint.
     */
    LSTALK_DIAGNOSTICSEVERITY_HINT = 4,
} LSTalk_DiagnosticSeverity;

/**
 * Structure to capture a description for an error code.
 *
 * @since 3.16.0
 */
typedef struct LSTalk_CodeDescription {
    /**
     * An URI to open with more information about the diagnostic error.
     */
    char* href;
} LSTalk_CodeDescription;

/**
 * Represents a related message and source code location for a diagnostic.
 * This should be used to point to code locations that cause or are related to
 * a diagnostics, e.g when duplicating a symbol in a scope.
 */
typedef struct LSTalk_DiagnosticRelatedInformation {
    /**
     * The location of this related diagnostic information.
     */
    LSTalk_Location location;

    /**
     * The message of this related diagnostic information.
     */
    char* message;
} LSTalk_DiagnosticRelatedInformation;

/**
 * Represents a diagnostic, such as a compiler error or warning. Diagnostic
 * objects are only valid in the scope of a resource.
 */
typedef struct LSTalk_Diagnostic {
    /**
     * The range at which the message applies.
     */
    LSTalk_Range range;

    /**
     * The diagnostic's severity. Can be omitted. If omitted it is up to the
     * client to interpret diagnostics as error, warning, info or hint.
     */
    LSTalk_DiagnosticSeverity severity;

    /**
     * The diagnostic's code, which might appear in the user interface.
     */
    char* code;

    /**
     * An optional property to describe the error code.
     *
     * @since 3.16.0
     */
    LSTalk_CodeDescription code_description;

    /**
     * A human-readable string describing the source of this
     * diagnostic, e.g. 'typescript' or 'super lint'.
     */
    char* source;

    /**
     * The diagnostic's message.
     */
    char* message;

    /**
     * Additional metadata about the diagnostic.
     * 
     * This will contain flags from LSTalk_DiagnosticTag.
     *
     * @since 3.15.0
     */
    int tags;

    /**
     * An array of related diagnostic information, e.g. when symbol-names within
     * a scope collide all definitions can be marked via this property.
     */
    LSTalk_DiagnosticRelatedInformation* related_information;
    int related_information_count;
} LSTalk_Diagnostic;

/**
 * Diagnostics notification are sent from the server to the client to signal
 * results of validation runs.
 */
typedef struct LSTalk_PublishDiagnostics {
    /**
     * The URI for which diagnostic information is reported.
     */
    char* uri;

    /**
     * Optional the version number of the document the diagnostics are published
     * for.
     *
     * @since 3.15.0
     */
    int version;

    /**
     * An array of diagnostic information items.
     */
    LSTalk_Diagnostic* diagnostics;
    int diagnostics_count;
} LSTalk_PublishDiagnostics;

/**
 * Represents programming constructs like variables, classes, interfaces etc.
 * that appear in a document. Document symbols can be hierarchical and they
 * have two ranges: one that encloses its definition and one that points to its
 * most interesting range, e.g. the range of an identifier.
 */
typedef struct LSTalk_DocumentSymbol {
    /**
     * The name of this symbol. Will be displayed in the user interface and
     * therefore must not be an empty string or a string only consisting of
     * white spaces.
     */
    char* name;
    
    /**
     * More detail for this symbol, e.g the signature of a function.
     */
    char* detail;

    /**
     * The kind of this symbol.
     */
    LSTalk_SymbolKind kind;

    /**
     * Tags for this document symbol. 
     *
     * @since 3.16.0
     */
    int tags;

    /**
     * The range enclosing this symbol not including leading/trailing whitespace
     * but everything else like comments. This information is typically used to
     * determine if the clients cursor is inside the symbol to reveal in the
     * symbol in the UI.
     */
    LSTalk_Range range;

    /**
     * The range that should be selected and revealed when this symbol is being
     * picked, e.g. the name of a function. Must be contained by the `range`.
     */
    LSTalk_Range selection_range;

    /**
     * Children of this symbol, e.g. properties of a class.
     */
    struct LSTalk_DocumentSymbol* children;
    int children_count;
} LSTalk_DocumentSymbol;

/**
 * Response received from a document_symbol request.
 */
typedef struct LSTalk_DocumentSymbolNotification {
    char* uri;
    LSTalk_DocumentSymbol* symbols;
    int symbols_count;
} LSTalk_DocumentSymbolNotification;

/**
 * Different types of notifications/responses from the server.
 */
typedef enum {
    LSTALK_NOTIFICATION_NONE,
    LSTALK_NOTIFICATION_TEXT_DOCUMENT_SYMBOLS,
    LSTALK_NOTIFICATION_PUBLISHDIAGNOSTICS,
} LSTalk_NotificationType;

/**
 * A notification/response from the server or library. The struct will contain
 * a type and the data associated with the type.
 */
typedef struct LSTalk_Notification {
    union {
        LSTalk_DocumentSymbolNotification document_symbols;
        LSTalk_PublishDiagnostics publish_diagnostics;
    } data;

    LSTalk_NotificationType type;
    int polled;
} LSTalk_Notification;

#ifdef LSTALK_TESTS
LSTALK_API void lstalk_tests(int argc, char** argv);
LSTALK_API void lstalk_test_server(int argc, char** argv);
#endif

#if defined(__cplusplus)
}
#endif

#endif
