#ifndef __EFI_ACPI_SDT_H_INCLUDE__
#define __EFI_ACPI_SDT_H_INCLUDE__

#define SIGNATURE_16(A, B)		((A) | (B << 8))
#define SIGNATURE_32(A, B, C, D)	(SIGNATURE_16(A, B) | (SIGNATURE_16(C, D) << 16))

#define EFI_AML_NODE_LIST_SIGNATURE	SIGNATURE_32('E', 'A', 'M', 'L')
#define EFI_AML_HANDLE_SIGNATURE	SIGNATURE_32('E', 'A', 'H', 'S')
#define EFI_AML_ROOT_HANDLE_SIGNATURE	SIGNATURE_32 ('E', 'A', 'R', 'H')

#define AML_OP_PARSE_INDEX_GET_OPCODE	0
#define AML_OP_PARSE_INDEX_GET_TERM1	1
#define AML_OP_PARSE_INDEX_GET_TERM2	2
#define AML_OP_PARSE_INDEX_GET_TERM3	3
#define AML_OP_PARSE_INDEX_GET_TERM4	4
#define AML_OP_PARSE_INDEX_GET_TERM5	5
#define AML_OP_PARSE_INDEX_GET_TERM6	6
#define AML_OP_PARSE_INDEX_GET_SIZE	((AML_OP_PARSE_INDEX)-1)

/* #define AML_NONE		0x00 */
/* #define AML_OBJECT		0x02 */
#define AML_OPCODE		AML_TYPE1OPCODE
#define AML_UINT8		AML_BYTEDATA
#define AML_UINT16		AML_WORDDATA
#define AML_UINT32		AML_DWORDDATA
#define AML_UINT64		AML_QWORDDATA
#define AML_NAME		AML_SIMPLENAME
#define AML_STRING		AML_NAMESTRING

#define AML_HAS_PKG_LENGTH	0x1 /* if OpCode has PkgLength */
#define AML_IS_NAME_CHAR	0x2 /* if this is NameChar */
#define AML_HAS_CHILD_OBJ	0x4 /* if OpCode has Child Object */
/* It is UEFI SDT attribute - if OpCode will be in NameSpace
 * NOTE; Not all OBJECT will be in NameSpace
 * For example, BankField | CreateBitField | CreateByteField |
 *              CreateDWordField | CreateField | CreateQWordField |
 *              CreateWordField | Field | IndexField.
 */
#define AML_IN_NAMESPACE	0x10000

#ifndef __ASSEMBLY__
typedef uint32_t AML_OP_PARSE_INDEX;
typedef uint32_t AML_OP_PARSE_FORMAT;
typedef uint32_t AML_OP_ATTRIBUTE;
typedef void *ACPI_HANDLE;

struct aml_op_info {
	uint8_t opcode;
	uint8_t ext_opcode;
	AML_OP_PARSE_INDEX max_index;
	AML_OP_PARSE_FORMAT format[6];
	AML_OP_ATTRIBUTE attr;
};

struct aml_node_list {
	uint32_t signature;
	uint8_t *name;
	uint8_t *buffer;
	size_t size;
	struct list_head link;
	struct list_head children;
	struct aml_node_list *parent;
	struct aml_op_info *info;
};

struct aml_handle {
	uint32_t signature;
	uint8_t *buffer;
	size_t size;
	struct aml_op_info *info;
	bool modified;
};

#ifdef CONFIG_ACPI_SDT
acpi_status_t acpi_sdt_open_ex(uint8_t *buffer,
			       size_t buffer_size,
			       ACPI_HANDLE *paml);
acpi_status_t acpi_sdt_close(ACPI_HANDLE aml);
acpi_status_t acpi_sdt_find_path(ACPI_HANDLE aml,
				 char *asl_path,
				 ACPI_HANDLE *paml);
acpi_status_t acpi_sdt_get_child(ACPI_HANDLE aml,
				 ACPI_HANDLE *paml);
acpi_status_t acpi_sdt_get_option(ACPI_HANDLE aml,
				  int index,
				  AML_OP_PARSE_FORMAT *data_type,
				  uint8_t **data,
				  size_t *data_size);
acpi_status_t acpi_sdt_set_option(ACPI_HANDLE aml,
				  int index,
				  uint8_t *data,
				  size_t data_size);
acpi_status_t acpi_sdt_patch_STA(struct acpi_table_header *table,
				 char *path,
				 uint8_t sta);
#else
#define acpi_sdt_open_ex(buffer, size, paml)		AE_BAD_PARAMETER
#define acpi_sdt_close(aml)				AE_BAD_PARAMETER
#define acpi_sdt_find_path(aml, path, paml)		AE_BAD_PARAMETER
#define acpi_sdt_get_child(aml, paml)			AE_BAD_PARAMETER
#define acpi_sdt_set_option(aml, index, data, data_size)		\
							AE_BAD_PARAMETER
#define acpi_sdt_get_option(aml, index, data_type, data, data_size)	\
							AE_BAD_PARAMETER
#define acpi_sdt_patch_STA(table, path, sta)		AE_BAD_PARAMETER
#endif
#endif /* __ASSEMBLY__ */

#endif /* __EFI_ACPI_SDT_H_INCLUDE__ */
