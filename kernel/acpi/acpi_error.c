#include "acpi_int.h"

struct acpi_exception_info {
	char *name;
#ifdef CONFIG_ACPI_ERROR_DETAILS
	char *description;
#endif
};

#ifdef CONFIG_ACPI_ERROR_DETAILS
#define EXCEP_TXT(name, description)	{name, description}
#else
#define EXCEP_TXT(name, description)	{name}
#endif

/*
 * String versions of the exception codes above
 * These strings must match the corresponding defines exactly
 */
static const struct acpi_exception_info acpi_gbl_exception_names_env[] =
{
	EXCEP_TXT("AE_OK",
		  "No error"),
	EXCEP_TXT("AE_ERROR",
		  "Unspecified error"),
	EXCEP_TXT("AE_NO_ACPI_TABLES",
		  "ACPI tables could not be found"),
	EXCEP_TXT("AE_NO_NAMESPACE",
		  "A namespace has not been loaded"),
	EXCEP_TXT("AE_NO_MEMORY",
		  "Insufficient dynamic memory"),
	EXCEP_TXT("AE_NOT_FOUND",
		  "A requested entity is not found"),
	EXCEP_TXT("AE_NOT_EXIST",
		  "A required entity does not exist"),
	EXCEP_TXT("AE_ALREADY_EXISTS",
		  "An entity already exists"),
	EXCEP_TXT("AE_TYPE",
		  "The object type is incorrect"),
	EXCEP_TXT("AE_NULL_OBJECT",
		  "A required object was missing"),
	EXCEP_TXT("AE_NULL_ENTRY",
		  "The requested object does not exist"),
	EXCEP_TXT("AE_BUFFER_OVERFLOW",
		  "The buffer provided is too small"),
	EXCEP_TXT("AE_STACK_OVERFLOW",
		  "An internal stack overflowed"),
	EXCEP_TXT("AE_STACK_UNDERFLOW",
		  "An internal stack underflowed"),
	EXCEP_TXT("AE_NOT_IMPLEMENTED",
		  "The feature is not implemented"),
	EXCEP_TXT("AE_SUPPORT",
		  "The feature is not supported"),
	EXCEP_TXT("AE_LIMIT",
		  "A predefined limit was exceeded"),
	EXCEP_TXT("AE_TIME",
		  "A time limit or timeout expired"),
	EXCEP_TXT("AE_ACQUIRE_DEADLOCK",
		  "Internal error, attempt was made to acquire a mutex in improper order"),
	EXCEP_TXT("AE_RELEASE_DEADLOCK",
		  "Internal error, attempt was made to release a mutex in improper order"),
	EXCEP_TXT("AE_NOT_ACQUIRED",
		  "An attempt to release a mutex or Global Lock without a previous acquire"),
	EXCEP_TXT("AE_ALREADY_ACQUIRED",
		  "Internal error, attempt was made to acquire a mutex twice"),
	EXCEP_TXT("AE_NO_HARDWARE_RESPONSE",
		  "Hardware did not respond after an I/O operation"),
	EXCEP_TXT("AE_NO_GLOBAL_LOCK",
		  "There is no FACS Global Lock"),
	EXCEP_TXT("AE_ABORT_METHOD",
		  "A control method was aborted"),
	EXCEP_TXT("AE_SAME_HANDLER",
		  "Attempt was made to install the same handler that is already installed"),
	EXCEP_TXT("AE_NO_HANDLER",
		  "A handler for the operation is not installed"),
	EXCEP_TXT("AE_OWNER_ID_LIMIT",
		  "There are no more Owner IDs available for ACPI tables or control methods"),
	EXCEP_TXT("AE_NOT_CONFIGURED",
		  "The interface is not part of the current subsystem configuration"),
	EXCEP_TXT("AE_ACCESS",
		  "Permission denied for the requested operation")
};

static const struct acpi_exception_info acpi_gbl_exception_names_pgm[] =
{
	EXCEP_TXT(NULL, NULL),
	EXCEP_TXT("AE_BAD_PARAMETER",
		  "A parameter is out of range or invalid"),
	EXCEP_TXT("AE_BAD_CHARACTER",
		  "An invalid character was found in a name"),
	EXCEP_TXT("AE_BAD_PATHNAME",
		  "An invalid character was found in a pathname"),
	EXCEP_TXT("AE_BAD_DATA",
		  "A package or buffer contained incorrect data"),
	EXCEP_TXT("AE_BAD_HEX_CONSTANT",
		  "Invalid character in a Hex constant"),
	EXCEP_TXT("AE_BAD_OCTAL_CONSTANT",
		  "Invalid character in an Octal constant"),
	EXCEP_TXT("AE_BAD_DECIMAL_CONSTANT",
		  "Invalid character in a Decimal constant"),
	EXCEP_TXT("AE_MISSING_ARGUMENTS",
		  "Too few arguments were passed to a control method"),
	EXCEP_TXT("AE_BAD_ADDRESS",
		  "An illegal null I/O address")
};

static const struct acpi_exception_info acpi_gbl_exception_names_tbl[] =
{
	EXCEP_TXT(NULL, NULL),
	EXCEP_TXT("AE_BAD_SIGNATURE",
		  "An ACPI table has an invalid signature"),
	EXCEP_TXT("AE_BAD_HEADER",
		  "Invalid field in an ACPI table header"),
	EXCEP_TXT("AE_BAD_CHECKSUM",
		  "An ACPI table checksum is not correct"),
	EXCEP_TXT("AE_BAD_VALUE",
		  "An invalid value was found in a table"),
	EXCEP_TXT("AE_INVALID_TABLE_LENGTH",
		  "The FADT or FACS has improper length")
};

static const struct acpi_exception_info acpi_gbl_exception_names_aml[] =
{
	EXCEP_TXT(NULL, NULL),
	EXCEP_TXT("AE_AML_BAD_OPCODE",
		  "Invalid AML opcode encountered"),
	EXCEP_TXT("AE_AML_NO_OPERAND",
		  "A required operand is missing"),
	EXCEP_TXT("AE_AML_OPERAND_TYPE",
		  "An operand of an incorrect type was encountered"),
	EXCEP_TXT("AE_AML_OPERAND_VALUE",
		  "The operand had an inappropriate or invalid value"),
	EXCEP_TXT("AE_AML_UNINITIALIZED_LOCAL",
		  "Method tried to use an uninitialized local variable"),
	EXCEP_TXT("AE_AML_UNINITIALIZED_ARG",
		  "Method tried to use an uninitialized argument"),
	EXCEP_TXT("AE_AML_UNINITIALIZED_ELEMENT",
		  "Method tried to use an empty package element"),
	EXCEP_TXT("AE_AML_NUMERIC_OVERFLOW",
		  "Overflow during BCD conversion or other"),
	EXCEP_TXT("AE_AML_REGION_LIMIT",
		  "Tried to access beyond the end of an Operation Region"),
	EXCEP_TXT("AE_AML_BUFFER_LIMIT",
		  "Tried to access beyond the end of a buffer"),
	EXCEP_TXT("AE_AML_PACKAGE_LIMIT",
		  "Tried to access beyond the end of a package"),
	EXCEP_TXT("AE_AML_DIVIDE_BY_ZERO",
		  "During execution of AML Divide operator"),
	EXCEP_TXT("AE_AML_BAD_NAME",
		  "An ACPI name contains invalid character(s)"),
	EXCEP_TXT("AE_AML_NAME_NOT_FOUND",
		  "Could not resolve a named reference"),
	EXCEP_TXT("AE_AML_INTERNAL",
		  "An internal error within the interprete"),
	EXCEP_TXT("AE_AML_INVALID_SPACE_ID",
		  "An Operation Region SpaceID is invalid"),
	EXCEP_TXT("AE_AML_STRING_LIMIT",
		  "String is longer than 200 characters"),
	EXCEP_TXT("AE_AML_NO_RETURN_VALUE",
		  "A method did not return a required value"),
	EXCEP_TXT("AE_AML_METHOD_LIMIT",
		  "A control method reached the maximum reentrancy limit of 255"),
	EXCEP_TXT("AE_AML_NOT_OWNER",
		  "A thread tried to release a mutex that it does not own"),
	EXCEP_TXT("AE_AML_MUTEX_ORDER",
		  "Mutex SyncLevel release mismatch"),
	EXCEP_TXT("AE_AML_MUTEX_NOT_ACQUIRED",
		  "Attempt to release a mutex that was not previously acquired"),
	EXCEP_TXT("AE_AML_INVALID_RESOURCE_TYPE",
		  "Invalid resource type in resource list"),
	EXCEP_TXT("AE_AML_INVALID_INDEX",
		  "Invalid Argx or Localx (x too large)"),
	EXCEP_TXT("AE_AML_REGISTER_LIMIT",
		  "Bank value or Index value beyond range of register"),
	EXCEP_TXT("AE_AML_NO_WHILE",
		  "Break or Continue without a While"),
	EXCEP_TXT("AE_AML_ALIGNMENT",
		  "Non-aligned memory transfer on platform that does not support this"),
	EXCEP_TXT("AE_AML_NO_RESOURCE_END_TAG",
		  "No End Tag in a resource list"),
	EXCEP_TXT("AE_AML_BAD_RESOURCE_VALUE",
		  "Invalid value of a resource element"),
	EXCEP_TXT("AE_AML_CIRCULAR_REFERENCE",
		  "Two references refer to each other"),
	EXCEP_TXT("AE_AML_BAD_RESOURCE_LENGTH",
		  "The length of a Resource Descriptor in the AML is incorrect"),
	EXCEP_TXT("AE_AML_ILLEGAL_ADDRESS",
		  "A memory, I/O, or PCI configuration address is invalid"),
	EXCEP_TXT("AE_AML_INFINITE_LOOP",
		  "An apparent infinite AML While loop, method was aborted")
};

static const struct acpi_exception_info acpi_gbl_exception_names_ctrl[] =
{
	EXCEP_TXT(NULL, NULL),
	EXCEP_TXT("AE_CTRL_RETURN_VALUE",
		  "A Method returned a value"),
	EXCEP_TXT("AE_CTRL_PENDING",
		  "Method is calling another method"),
	EXCEP_TXT("AE_CTRL_TERMINATE",
		  "Terminate the executing method"),
	EXCEP_TXT("AE_CTRL_TRUE",
		  "An If or While predicate result"),
	EXCEP_TXT("AE_CTRL_FALSE",
		  "An If or While predicate result"),
	EXCEP_TXT("AE_CTRL_DEPTH",
		  "Maximum search depth has been reached"),
	EXCEP_TXT("AE_CTRL_END",
		  "An If or While predicate is false"),
	EXCEP_TXT("AE_CTRL_TRANSFER",
		  "Transfer control to called method"),
	EXCEP_TXT("AE_CTRL_BREAK",
		  "A Break has been executed"),
	EXCEP_TXT("AE_CTRL_CONTINUE",
		  "A Continue has been executed"),
	EXCEP_TXT("AE_CTRL_SKIP",
		  "Not currently used"),
	EXCEP_TXT("AE_CTRL_PARSE_CONTINUE",
		  "Used to skip over bad opcodes"),
	EXCEP_TXT("AE_CTRL_PARSE_PENDING",
		  "Used to implement AML While loops")
};

const char *acpi_error_string(acpi_status_t status, boolean detail)
{
	uint32_t sub_status;
	const struct acpi_exception_info *exception = NULL;
	
	sub_status = (status & ~AE_CODE_MASK);
	
	switch (status & AE_CODE_MASK) {
	case AE_CODE_ENVIRONMENTAL:
		if (sub_status <= AE_CODE_ENV_MAX)
			exception = &acpi_gbl_exception_names_env[sub_status];
		break;
	case AE_CODE_PROGRAMMER:
		if (sub_status <= AE_CODE_PGM_MAX)
			exception = &acpi_gbl_exception_names_pgm[sub_status];
		break;
	case AE_CODE_ACPI_TABLES:
		if (sub_status <= AE_CODE_TBL_MAX)
			exception = &acpi_gbl_exception_names_tbl[sub_status];
		break;
	case AE_CODE_AML:
		if (sub_status <= AE_CODE_AML_MAX)
			exception = &acpi_gbl_exception_names_aml[sub_status];
		break;
	case AE_CODE_CONTROL:
		if (sub_status <= AE_CODE_CTRL_MAX)
			exception = &acpi_gbl_exception_names_ctrl[sub_status];
		break;
	default:
		break;
	}
	
#ifdef CONFIG_ACPI_ERROR_DETAILS
	if (!exception || !exception->name)
		return detail ? "Invalid error" : "AE_UNKNOWN";
	return detail ? exception->description ? exception->name;
#else
	if (!exception || !exception->name)
		return "AE_UNKNOWN";
	return exception->name;
#endif
}
